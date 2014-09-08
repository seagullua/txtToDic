#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QTime>
#include "txtToDic.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString fileName = "../examples/Chornyj-Voron.txt";



    QTime myTimer;
    myTimer.start();

    QStringList vec = createDictionaryFromFile(fileName);

    // do something..
    int nMilliseconds = myTimer.elapsed();
    for(int i=0; i<10; ++i)
    {
        qDebug() << vec[i];
    }
    qDebug() << vec.size();
    qDebug() << "Time: " << nMilliseconds;

    return 0;
}
