#ifndef CURLDOWNLOADER_H
#define CURLDOWNLOADER_H

#include <QDateTime>
#include <QNetworkRequest>
#include <QObject>
#include <QProcess>

struct CurlProgress{
    static CurlProgress fromLine(const QString &line)
    {
        CurlProgress p;
        auto list = line.split(" ", Qt::SkipEmptyParts);
        int i = 0;
        p.percentTotal = list.at(i++).toInt();
        p.total = list.at(i++);
        p.percentReceived = list.at(i++).toInt();
        p.received = list.at(i++);
        p.percentXferd = list.at(i++).toInt();
        p.Xferd = list.at(i++);
        p.averageDload = list.at(i++);
        p.averageUpload = list.at(i++);
        p.timeTotal = list.at(i++);
        p.timeSpent = list.at(i++);
        p.timeLeft = list.at(i++);
        p.currentSpeed = list.at(i++);
        return p;
    }

    int percentTotal;
    QString total;

    int percentReceived;
    QString received;

    int percentXferd;
    QString Xferd;

    QString averageDload;
    QString averageUpload;

    QString timeTotal;
    QString timeSpent;
    QString timeLeft;

    QString currentSpeed;
};

class CurlDownloader : public QObject
{
    Q_OBJECT
public:
    explicit CurlDownloader(QObject *parent = nullptr);

    void download(QNetworkRequest request, QString filename, QByteArray data);
signals:
    void updateDownloadProgress(CurlProgress progress);
    void finished();

private slots:
    void readyReadStandardError();

private:
    QProcess process_;
    bool finished_ = false;
};

#endif // CURLDOWNLOADER_H
