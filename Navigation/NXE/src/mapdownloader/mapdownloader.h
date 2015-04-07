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

struct MapDownloaderPrivate;
class MapDownloader {
public:
    // We can bind in to member function pointer, lambda etc
    //                              url                 error
    typedef std::function<void(const std::string&, const std::string&)> CbOnError;
    //                          url                 progress    all
    typedef std::function<void(const std::string&, long, long)> CbOnProgress;

    MapDownloader();
    virtual ~MapDownloader();

    std::string download(const std::string& name);

    void cancel();

    long getEstimatedSize(const std::string& name);

    void enableReportProgress(bool flag);
    void setMapFileDir(const std::string& dir);

    void setMapDescFilePath(const std::string& path);

    inline void setCbError(CbOnError cb)
    {
        cbOnError = cb;
    }

    inline void setCbProgress(CbOnProgress cb)
    {
        cbOnProgress = cb;
    }

    CbOnError cbOnError;
    CbOnProgress cbOnProgress;

private:
    static size_t dataWrite(void* buffer, size_t size, size_t nmemb, void* stream);
    void onDownloadError(const std::string& url, CURLcode err);
    static int onDownloadProgress(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

    static std::string getDownloadErrorStr(CURLcode err);

    std::string createMapRequestString(const std::string& name);

    bool mapDirOk() const;

    std::unique_ptr<MapDownloaderPrivate> d;


    bool m_reportProgess;
};

#endif /* MAPDOWNLOADER_H_ */
