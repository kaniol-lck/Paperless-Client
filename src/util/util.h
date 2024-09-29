#ifndef UTIL_H
#define UTIL_H

#include <QIcon>
#include <QString>

void openFolder(const QString &path);
void openFileInFolder(const QString &filePath);
void openFileInFolder(const QString &fileName, const QString &filePath);

QIcon reverseIcon(const QIcon& icon, int extent = 32);

#endif // UTIL_H
