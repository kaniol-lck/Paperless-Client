#include "curldownloader.h"

#include <QDebug>

CurlDownloader::CurlDownloader(QObject *parent)
    : QObject{parent}
{
    // process_.setReadChannel(QProcess::StandardOutput);
    process_.setReadChannel(QProcess::StandardError);
    connect(&process_, &QProcess::readyReadStandardError, this, &CurlDownloader::readyReadStandardError);
}

void CurlDownloader::download(QNetworkRequest request, QString filename, QByteArray data)
{
    QStringList opts;
    for(auto header : request.rawHeaderList()){
        opts << "-H"
             << header + ": " + request.rawHeader(header);
    }
    if(!data.isEmpty()){
        opts << "-X"
             << "POST"
             << "-d"
             << data;
    }
    opts << request.url().toString();
    opts << "--output"
         << filename;
    process_.start("curl", opts);
}

void CurlDownloader::readyReadStandardError()
{
    while(process_.canReadLine()){
        auto line = process_.readLine();
        if(!line.startsWith('\r')) continue;
        for(QString out : line.trimmed().split('\r')){
            auto progress = CurlProgress::fromLine(out);
            emit updateDownloadProgress(progress);
            if(progress.percentTotal == 100 && !finished_){
                emit finished();
                finished_ = true;
                qDebug() << "finished";
            }
        }
    }
}
