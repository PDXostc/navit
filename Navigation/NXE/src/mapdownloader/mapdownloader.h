/*
 * MapDownloader.h
 *
 *  Created on: Mar 31, 2015
 *      Author:
 */

#ifndef MAPDOWNLOADER_H_
#define MAPDOWNLOADER_H_

#include <curl/curl.h>
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include "mapdesc.h"

struct MapEntry {
    const std::string name;
    const std::uint64_t size;
    const bool isDownloaded;
    const std::string continent;
};

struct MapDownloaderPrivate;
class MapDownloader {
public:
    // We can bind in to member function pointer, lambda etc
    //                              url                 error
    typedef std::function<void(const std::string&, const std::string&)> CbOnError;
    //                          url                 progress    all
    typedef std::function<void(const std::string&, long, long)> CbOnProgress;

    typedef std::function<void(const std::string&)> CbOnFinished;

    MapDownloader();
    virtual ~MapDownloader();

    std::string download(const std::string& name);
    std::vector<MapEntry> maps() const;

    void cancel(const std::string &reqUrl);

    long getEstimatedSize(const std::string& name);

    void enableReportProgress(bool flag);
    bool setMapFileDir(const std::string& dir);

    inline void setCbError(CbOnError cb)
    {
        cbOnError = cb;
    }

    inline void setCbProgress(CbOnProgress cb)
    {
        cbOnProgress = cb;
    }

    inline void setCbOnFinished(const CbOnFinished &cb)
    {
        cbOnFinished = cb;
    }

    void removeMaps();

    CbOnError cbOnError;
    CbOnProgress cbOnProgress;
    CbOnFinished cbOnFinished;

private:
    static size_t dataWrite(void* buffer, size_t size, size_t nmemb, void* stream);
    static size_t headersWrite(void* buffer, size_t size, size_t nmemb, void* userp);
    void onDownloadError(const std::string& url, CURLcode err);
    static int onDownloadProgress(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
    static int onHeaderCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

    static std::string getDownloadErrorStr(CURLcode err);
    std::string createMapRequestString(const std::string& name, const std::string& timestamp = "");

    std::unique_ptr<MapDownloaderPrivate> d;

    bool m_reportProgess;
};

#endif /* MAPDOWNLOADER_H_ */
