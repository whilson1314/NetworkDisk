#include "tcpclient.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
//#include "friend.h"
//#include "sharedfilefriendlist.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TcpClient_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    TcpClient::getInstance().show();

//    sharedFileFriendList m;
//    m.show();

    return a.exec();
}
