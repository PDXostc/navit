
#include "log.h"
#include "inavitipc.h"

std::ostream& operator<<(std::ostream &os, const NXE::PointClicked &p)
{
    os << p.position << " items = ";

    std::for_each(p.items.begin(), p.items.end(), [&os]( const std::pair<std::string, std::string>& pair) {
        os << "[" << pair.first << "] = [" << pair.second << "]";
    });
    return os;
}


std::ostream &operator<<(std::ostream &os, const NXE::Position &position)
{
    os << " [Point] " << "lat= " << position.latitude << " lon= " << position.longitude;
    os << " ";
    return os;
}
