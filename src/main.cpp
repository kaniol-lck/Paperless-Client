#include "ui/client.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("kaniol");
    a.setApplicationName("Paperless Client");
    a.setApplicationDisplayName(QObject::tr("Paperless Client"));

    auto font = a.font();
    font.setPixelSize(15);
    a.setFont(font);

    //setup translator
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    Client w;
    w.show();
    return a.exec();
}
