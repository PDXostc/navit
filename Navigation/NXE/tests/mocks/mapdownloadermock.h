#ifndef MAPDOWNLOADERMOCK_H 
#define MAPDOWNLOADERMOCK_H 

#include "imapdownloader.h"

struct MapDownloaderMock : public NXE::IMapDownloader {
    using Inject = MapDownloaderMock ();
    MOCK_METHOD0(maps, std::vector<NXE::MapInfo>());
    MOCK_METHOD1(download, bool(const std::string &));
    MOCK_METHOD1(setListener, void(const NXE::MapDownloaderListener& listener));
    MOCK_METHOD1(cancel, void(const std::string&));
    MOCK_METHOD1(setOutputDirectory, bool(const std::string&));
};

#endif // MAPDOWNLOADERMOCK_H 
