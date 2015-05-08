#ifndef CALLS_H
#define CALLS_H

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 30

#include "position.h"
#include "imapdownloader.h"
#include "inavitipc.h"

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <type_traits>
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

template<typename ReturnType, typename... Args>
struct AMessage {
    typedef ReturnType RType;
};

struct ZoomMessageTag {
    typedef std::function<int ()> Parser;
    typedef typename Parser::result_type ReturnType;
};

struct MoveByMessageTag {
    typedef std::function<void (int,int)> Parser;
    typedef typename Parser::result_type ReturnType;
};

struct SetOrientationMessageTag {
    typedef std::function<void (int)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct ZoomByMessageTag {
    typedef std::function<void (int)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct PositionMessageTag {
    typedef std::function<NXE::Position ()> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct RenderMessageTag {
    typedef std::function<void ()> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct ExitMessageTag {
    typedef std::function<void ()> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct OrientationMessageTag {
    typedef std::function<int ()> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct SetCenterMessageTag {
    typedef std::function<void (double, double)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct DownloadMessageTag {
    typedef std::function<void (const std::string& )> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct CancelDownloadMessageTag {
    typedef std::function<void (const std::string&)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct MapsMessageTag {
    typedef std::function<std::vector<NXE::MapInfo> ()> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct SetDestinationMessageTag {
    typedef std::function<void (double, double, const char*)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct ClearDestinationMessageTag {
    typedef std::function<void ()> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct SetPositionMessageTag {
    typedef std::function<void (double, double)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct SetPositionByIntMessageTag {
    typedef std::function<void (int,int)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct SetSchemeMessageTag {
    typedef std::function<void (const std::string&)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct StartSearchTag {
    typedef std::function<void ()> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct SearchCountryLocationTag {
    typedef std::function<std::vector<NXE::Country> (const std::string&)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct SearchCityLocationTag {
    typedef std::function<std::vector<NXE::City> (const std::string&)> Parser;
    typedef typename Parser::result_type ReturnType;
};
struct FinishSearchTag {
    typedef std::function<void ()> Parser;
    typedef typename Parser::result_type ReturnType;
};

struct AddWaypointMessageTag {
    typedef std::function<void (double, double)> Parser;
    typedef typename Parser::result_type ReturnType;
};

struct ResizeMessageTag {
    typedef std::function<void (int, int)> Parser;
    typedef typename Parser::result_type ReturnType;
};


// Containter
typedef boost::mpl::vector<MoveByMessageTag, ZoomByMessageTag, ZoomMessageTag, SetOrientationMessageTag,
      PositionMessageTag, RenderMessageTag, ExitMessageTag, OrientationMessageTag, SetCenterMessageTag,
      DownloadMessageTag, CancelDownloadMessageTag, MapsMessageTag,SetDestinationMessageTag,
      ClearDestinationMessageTag, SetPositionMessageTag,SetPositionByIntMessageTag,
      SetSchemeMessageTag,
      StartSearchTag, SearchCountryLocationTag, SearchCityLocationTag,FinishSearchTag,
      AddWaypointMessageTag, ResizeMessageTag
    > Messages_type;

template <typename T>
struct make_cb_pair {
    typedef typename boost::fusion::result_of::make_pair<T, typename T::Parser>::type type;
};

typedef typename boost::fusion::result_of::as_map<
    typename boost::mpl::transform<Messages_type,
        make_cb_pair<boost::mpl::_1> >::type>::type map_cb_type;

#endif
