#ifndef CALLS_H
#define CALLS_H

#include <string>
#include <map>
#include <functional>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/fusion/container.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/container/map/convert.hpp>
#include <boost/fusion/include/as_map.hpp>
#include <boost/fusion/sequence/intrinsic.hpp>
#include <boost/fusion/include/mpl.hpp>

// All messages
struct MoveByMessage {};
struct ZoomByMessage {};
struct ZoomMessage {};

// Container of all registered messages
typedef boost::mpl::vector<MoveByMessage, ZoomByMessage, ZoomMessage> Messages_type;
typedef std::function<void (const std::string &data)> JSONMessageParser_type;

template<typename T>
struct make_sig_pair
{
    typedef typename boost::fusion::result_of::make_pair <T, std::string>::type type;
};

typedef typename boost::fusion::result_of::as_map
    <
        typename boost::mpl::transform
            <
                Messages_type,
                make_sig_pair<boost::mpl::_1>
            >::type
    >::type map_type;


template<typename T>
struct make_cb_pair
{
    typedef typename boost::fusion::result_of::make_pair <T, JSONMessageParser_type>::type type;
};

typedef typename boost::fusion::result_of::as_map
    <
        typename boost::mpl::transform
            <
                Messages_type,
                make_cb_pair<boost::mpl::_1>
            >::type
    >::type map_cb_type;
#endif
