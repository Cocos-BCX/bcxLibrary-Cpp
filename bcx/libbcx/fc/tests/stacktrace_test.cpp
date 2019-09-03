#include <boost/test/unit_test.hpp>

#include <signal.h>
#include <fc/stacktrace.hpp>
#include <fc/thread/thread.hpp>

BOOST_AUTO_TEST_SUITE(fc_stacktrace)

BOOST_AUTO_TEST_CASE(stacktrace_test)
{
   // print the stack trace
   std::stringstream ss;
   fc::print_stacktrace(ss);
   std::string results = ss.str();
#if BOOST_VERSION / 100000 >= 1 && ((BOOST_VERSION / 100) % 1000) >= 65
   BOOST_CHECK(!results.empty());
   BOOST_CHECK(results.find("fc::print_stacktrace") != std::string::npos);
#else
   BOOST_CHECK(results.empty());
#endif
}

BOOST_AUTO_TEST_CASE(threaded_stacktrace_test)
{
   fc::thread test_thread("a_thread");
   std::string results = test_thread.async(
         [] ()->std::string {
               // cause a pause
               for(int i = 0; i < 10000; i++);
               std::stringstream ss;
               fc::print_stacktrace(ss);
               return ss.str();
            }
         ).wait();
#if BOOST_VERSION / 100000 >= 1 && ((BOOST_VERSION / 100) % 1000) >= 65
   BOOST_CHECK(!results.empty());
   BOOST_CHECK(results.find("fc::print_stacktrace") != std::string::npos);
#else
   BOOST_CHECK(results.empty());
#endif
}

/* this test causes a segfault on purpose to test the event handler
BOOST_AUTO_TEST_CASE(cause_segfault)
{
   fc::print_stacktrace_on_segfault();
   ::raise(SIGSEGV);
}
*/
BOOST_AUTO_TEST_SUITE_END()
