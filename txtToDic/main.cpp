#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QTime>
#include <QDir>
#include "txtToDic.h"

Dictionary dictionary;
int total_words = 0;

void processFile(QString fileName)
{
    int words = 0;
    QStringList vec = createDictionaryFromFile(fileName, words);


    total_words+=words;
    addToDictionary(dictionary, vec);
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();
    if(args.size() < 3)
    {
        qDebug() << "Wrong usage: dir_to_process output_file";
    }


    QString dirname = args[1];
    QDir dir(dirname);
    QStringList files = dir.entryList();

    QTime myTimer;
    myTimer.start();

    int files_processed = 0;
    foreach(QString file, files)
    {
        if(file != "." && file != "..")
        {
            files_processed++;
            processFile(dir.filePath(file));
        }
    }

    saveDictionaryToFile(dictionary, args[2]);
    int nMilliseconds = myTimer.elapsed();
    qDebug() << "Files: \t" << files_processed;
    qDebug() << "Words unique: " << dictionary.size();
    qDebug() << "Time: \t" << nMilliseconds;
    qDebug() << "Words: \t" << total_words;
    qDebug() << "Words per s:  " << (double(total_words) * 1000 / nMilliseconds);

    return 0;
}
