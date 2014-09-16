#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QTime>
#include <QDir>
#include "txtToDic.h"
#include "ADThreadPool.h"
#include <thread>
#include "indexer.h"
typedef QVector<Index*> Indexes;
std::mutex lock;

void processFile(QString fileName, const Indexes& indexes)
{
    int words = 0;
    QStringList vec = createDictionaryFromFile(fileName, words);


    {
        std::unique_lock<std::mutex>
                locker(lock);
        foreach(Index* index, indexes)
        {
            index->addToIndex(fileName, vec);
        }
    }
}

void findInIndex(QString index_name, QString query, Index* index)
{
    QTextStream out(stdout);

    out << "*" << index_name << endl;
    QStringList list = index->find(query);
    if(list.size() == 0)
    {
        out << "None" << endl;
    }
    else
    {
        for(QString res: list)
        {
            out << "- " << res << endl;
        }
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();
    if(args.size() < 3)
    {
        qDebug() << "Wrong usage: dir_to_process queries_file";
        return 1;
    }


    QString dirname = args[1];
    QDir dir(dirname);
    QStringList files = dir.entryList();
    for(int i=0; i<files.size(); ++i)
    {
        files[i] = dir.filePath(files[i]);
    }

    Indexes indexes;
    MatrixIndex matrix(files);
    InvertedIndex inverted;

    indexes.push_back(&matrix);
    indexes.push_back(&inverted);

    ADThreadPool pool(8);


    QTime myTimer;
    myTimer.start();

    int files_processed = 0;
    foreach(QString file, files)
    {
        if(file != "." && file != "..")
        {
            files_processed++;
            pool.addTask([=](){
                processFile(file, indexes);
            });
        }
    }

    pool.start();
    pool.join();

    QStringList queries = readFile(args[2]).split("\n");

    QTextStream out(stdout);

    foreach(QString query, queries)
    {
        if(query.size())
        {
            out << "===================" << endl;
            out << query << endl;
            findInIndex("Matrix  ", query, &matrix);
            findInIndex("Inverted", query, &inverted);
        }
    }

    //saveDictionaryToFile(dictionary, args[2]);
    int nMilliseconds = myTimer.elapsed();
    out << "---------------------" << endl;
    out << "Time: \t" << nMilliseconds << endl;

    return 0;
}
