#include "util.h"

#include <QDesktopServices>
#include <QDir>
#include <QProcess>
#include <QUrl>

void openFolder(const QString &path)
{
    QFileInfo info(path);
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isFile()? info.absolutePath() : path));
}

void openFileInFolder(const QString &filePath)
{
    QProcess process;
#ifdef Q_OS_UNIX
    QString fileManager;
    process.start("xdg-mime", { "query", "default", "inode/directory" });
    process.waitForFinished();
    fileManager = process.readAllStandardOutput();
    if(fileManager.contains("dolphin")){
        process.startDetached("dolphin", { "--select", filePath });
        return;
    } else if(fileManager.contains("dde-file-manager")){
        process.startDetached("dde-file-manager", { "--show-item", filePath });
        return;
    } else if(fileManager.contains("nautilus")){
        process.startDetached("nautilus", { filePath });
        return;
    } else if(fileManager.contains("nemo")){
        process.startDetached("nemo", { filePath });
        return;
    } else if(fileManager.contains("elementary")){
        process.startDetached("elementary", { filePath });
        return;
    }
#endif
#ifdef Q_OS_WIN
    if(QFileInfo(filePath).isFile()){
        process.startDetached("explorer.exe", { "/select," + QDir::toNativeSeparators(filePath) });
        return;
    }
#endif
#ifdef Q_OS_MACOS
    process.startDetached("open", { "-R",  filePath });
    return;
#endif
    openFolder(filePath);
}

void openFileInFolder(const QString &fileName, const QString &filePath)
{
    if(fileName.isEmpty())
        openFolder(filePath);
    else
        openFileInFolder(QDir(filePath).absoluteFilePath(fileName));
}

QIcon reverseIcon(const QIcon &icon, int extent)
{
    auto pixmap = icon.pixmap(extent, QIcon::Mode::Selected);
    QIcon reversed(pixmap);
    auto pixmap2 =  icon.pixmap(extent);
    reversed.addPixmap(pixmap2, QIcon::Mode::Selected);
    return reversed;
}
