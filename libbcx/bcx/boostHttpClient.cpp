#include "./boostHttpClient.hpp"
#include "./bcxlog.hpp"

NS_BCX_BEGIN

void boostHttpClient::httpPost(const std::string& url, const std::string& json,
                     std::function<void(const std::string& respose, const std::string& error)> callback) {
    boost::asio::io_service io_service;
    boostHttpClient c(io_service, url, json, callback);
    io_service.run();
}

boostHttpClient::boostHttpClient(boost::asio::io_service& io_service,
                                 const std::string& url, const std::string& json,
                                 std::function<void(const std::string& respose, const std::string& error)>& callback)
        :resolver_(io_service),socket_(io_service) {
    this->callback = callback;
    std::string server;
    std::string port;
    std::string path;
    
    parseUrl(url, server, port, path);

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.

    contentLength = 0;
    std::ostream request_stream(&request_);
    request_stream << "POST " << path << " HTTP/1.1\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Accept: application/json\r\n";
    request_stream << "Content-Type: application/json\r\n";
    request_stream << "Content-Length: " << json.length() << "\r\n";
    request_stream << "Authorization: YnVmZW5nQDIwMThidWZlbmc=\r\n\r\n";
    // request_stream << "Connection: keep-alive\r\n\r\n";
    request_stream << json ;

    // Start an asynchronous resolve to translate the server and service names
    // into a list of endpoints.
    
    boost::asio::ip::tcp::resolver::query query(server, port);
    resolver_.async_resolve(query,
                            boost::bind(&boostHttpClient::handle_resolve,
                                        this, boost::asio::placeholders::error,
                                        boost::asio::placeholders::iterator));
}

void boostHttpClient::handle_resolve(const boost::system::error_code& err,
                                     boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
    if (err) {
        bcx::log("boostHttpClient handle_resolve:%d", err.value());
        this->close_connect("boostHttpClient handle_resolve failed");
        return;
    }
    // Attempt a connection to each endpoint in the list until we
    // successfully establish a connection.
    
    boost::asio::async_connect(socket_,
                               endpoint_iterator,
                               boost::bind(&boostHttpClient::handle_connect, this, boost::asio::placeholders::error));
}

void boostHttpClient::handle_connect(const boost::system::error_code& err) {
    if (err) {
        bcx::log("boostHttpClient handle_connect:%d", err.value());
        this->close_connect("boostHttpClient handle_connect failed");
        return;
    }
    
    // The connection was successful. Send the request.
    boost::asio::async_write(socket_,
                             request_,
                             boost::bind(&boostHttpClient::handle_write_request, this, boost::asio::placeholders::error));
}

void boostHttpClient::handle_write_request(const boost::system::error_code& err) {
    if (err) {
        bcx::log("boostHttpClient handle_write_request:%d", err.value());
        this->close_connect("boostHttpClient handle_write_request failed");
        return;
    }
    
    // Read the response status line. The response_ streambuf will
    // automatically grow to accommodate the entire line. The growth may be
    // limited by passing a maximum size to the streambuf constructor.
    boost::asio::async_read_until(socket_,
                                  response_,
                                  "\r\n",
                                  boost::bind(&boostHttpClient::handle_read_status_line, this, boost::asio::placeholders::error));
}

void boostHttpClient::handle_read_status_line(const boost::system::error_code& err) {
    if (err) {
        bcx::log("boostHttpClient handle_read_status_line:%d", err.value());
        this->close_connect("boostHttpClient handle_read_status_line failed");
        return;
    }
    
    // Check that response is OK.
    std::istream response_stream(&response_);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    
    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        bcx::log("Invalid response");
        this->close_connect("boostHttpClient Invalid response");
        return;
    }
    if (status_code != 200) {
        bcx::log("Response returned with status code:%d", status_code);
        this->close_connect("boostHttpClient Response returned with status code");
        return;
    }
    // Read the response headers, which are terminated by a blank line.
    boost::asio::async_read_until(socket_,
                                  response_,
                                  "\r\n\r\n",
                                  boost::bind(&boostHttpClient::handle_read_headers, this, boost::asio::placeholders::error));
}

void boostHttpClient::handle_read_headers(const boost::system::error_code& err) {
    if (err) {
        bcx::log("boostHttpClient handle_read_headers:%d", err.value());
        this->close_connect("boostHttpClient handle_read_headers failed");
        return;
    }

    // Process the response headers.
    std::istream response_stream(&response_);
    std::string header;
    std::string::size_type pos = 0;
    std::string contentTag = "Content-Length:";
    while (std::getline(response_stream, header) && header != "\r") {
        pos = header.find(contentTag);
        if (std::string::npos != pos) {
            std::stringstream ss(header.substr(pos + contentTag.length(), header.length() - pos - contentTag.length()));
            ss >> contentLength;
        }
    }
    
    // Start reading remaining data until EOF.
    boost::asio::async_read(socket_,
                            response_,
                            boost::asio::transfer_at_least(1),
                            boost::bind(&boostHttpClient::handle_read_content, this, boost::asio::placeholders::error));
}

void boostHttpClient::handle_read_content(const boost::system::error_code& err) {
    if (err == boost::asio::error::eof) {
        this->close_connect("");
        return;
    }
    if (err) {
        bcx::log("boostHttpClient handle_read_content:%d", err.value());
        this->close_connect("boostHttpClient handle_read_content failed");
        return;
    }
    
    if (response_.size() >= contentLength) {
        this->close_connect("");
    } else {
        // Continue reading remaining data until EOF.
        boost::asio::async_read(socket_,
                                response_,
                                boost::asio::transfer_at_least(1),
                                boost::bind(&boostHttpClient::handle_read_content, this, boost::asio::placeholders::error));
    }
    
}

void boostHttpClient::close_connect(const std::string& error) {
    if (this->callback) {
        if (error.empty()) {
            boost::asio::streambuf::const_buffers_type bufs = response_.data();
            std::string s(boost::asio::buffers_begin(bufs),
                          boost::asio::buffers_begin(bufs) + response_.size());
            this->callback(s, error);
        } else {
            this->callback("", error);
        }
    }
    socket_.close();
    resolver_.cancel();
}

int boostHttpClient::parseUrl(const std::string& url,
                              std::string& out_server,
                              std::string& out_port,
                              std::string& out_path) {
    const std::string& http___ = "http://";
    const std::string& https___ = "https://";
    std::string temp_data = url;

    if (temp_data.find(http___) == 0) {
        temp_data = temp_data.substr(http___.length());
    } else if (temp_data.find(https___) == 0) {
        temp_data = temp_data.substr(https___.length());
    } else {
        return 1;
    }

    std::size_t idx = temp_data.find('/');
    if (std::string::npos == idx) {
        out_path = "/";
        idx = temp_data.size();
    } else {
        out_path = temp_data.substr(idx);
    }
    
    out_server = temp_data.substr(0, idx);
    
    idx = out_server.find(':');
    if (std::string::npos == idx) {
        out_port = "80";
    } else {
        out_port = out_server.substr(idx + 1);
        out_server = out_server.substr(0, idx);
    }
    
    return 0;
}

NS_BCX_END

