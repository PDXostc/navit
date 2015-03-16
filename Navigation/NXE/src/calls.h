#ifndef CALLS_H
#define CALLS_H

#include "jsonmessage.h"

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
#include <boost/property_tree/ptree.hpp>

// Messages handled by application
struct MoveByMessage {};
struct ZoomByMessage {};
struct ZoomMessage {};
struct PositionMessage {};
struct RenderMessage {};

// Container of all registered messages
typedef boost::mpl::vector<MoveByMessage, ZoomByMessage, ZoomMessage, PositionMessage, RenderMessage> Messages_type;
typedef std::function<void (const NXE::JSONMessage &data)> JSONMessageParser_type;

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
