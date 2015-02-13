#ifndef LOG_H
#define LOG_H

#include <boost/log/trivial.hpp>
#include <boost/current_function.hpp>

#define LOG(log) BOOST_LOG_TRIVIAL(debug) << log
#define nDebug() BOOST_LOG_TRIVIAL(debug) << BOOST_CURRENT_FUNCTION << " "
#define nInfo() BOOST_LOG_TRIVIAL(info) << BOOST_CURRENT_FUNCTION << " "
#define nFatal() BOOST_LOG_TRIVIAL(fatal) << BOOST_CURRENT_FUNCTION << " "


#endif // LOG_H

