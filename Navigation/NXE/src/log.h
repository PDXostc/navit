#ifndef LOG_H
#define LOG_H

#include <boost/log/trivial.hpp>

#define LOG(log) BOOST_LOG_TRIVIAL(debug) << log

#endif // LOG_H

