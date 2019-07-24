#ifndef __BOOST_HTTPCLIENT_H__
#define __BOOST_HTTPCLIENT_H__

#include <string>
#include <iostream>
#include <istream>
#include <ostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
//#include <boost/regex.hpp>

#include "platform/PlatformMacros.h"

NS_BCX_BEGIN

class boostHttpClient {
public:

    static void httpPost(const std::string& url, const std::string& json,
                         std::function<void(const std::string& respose, const std::string& error)> callback);

    boostHttpClient(boost::asio::io_service& io_service,
                    const std::string& url, const std::string& json,
                    std::function<void(const std::string& respose, const std::string& error)>& callback);

private:

    void handle_resolve(const boost::system::error_code& err,
                        boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void handle_connect(const boost::system::error_code& err);
    void handle_write_request(const boost::system::error_code& err);
    void handle_read_status_line(const boost::system::error_code& err);
    void handle_read_headers(const boost::system::error_code& err);
    void handle_read_content(const boost::system::error_code& err);
    void close_connect(const std::string& error);

    int parseUrl(const std::string& url,
                                  std::string& out_server,
                                  std::string& out_port,
                                  std::string& out_path);

    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
    
    int contentLength;
    
    std::function<void(const std::string& respose, const std::string& error)> callback;
};

NS_BCX_END

#endif /* __BOOST_HTTPCLIENT_H__ */

