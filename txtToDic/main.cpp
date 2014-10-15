#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QTime>
#include <QDir>
#include "Optimiser.h"
#include "txtToDic.h"
#include "ADThreadPool.h"
#include <thread>
#include "indexer.h"
#include "DictionaryIndex.h"
#include "Merger.h"
typedef QVector<Index*> Indexes;
typedef QVector<DictionaryIndex*> DictionaryIndexes;
std::mutex lock;

void processFile(QString fileName, const Indexes& indexes)
{
    int words = 0;
    QStringList vec = createDictionaryFromFile(fileName, words);
    //QStringList unique = makeUnique(vec);

    {
        std::unique_lock<std::mutex>
                locker(lock);
        foreach(Index* index, indexes)
        {
            index->addToIndex(fileName, vec);
        }
    }
}

void processFileDic(QString fileName, const DictionaryIndexes& indexes)
{
    int words = 0;
    QStringList vec = createDictionaryFromFile(fileName, words);
    QStringList unique = makeUnique(vec);

    {
        std::unique_lock<std::mutex>
                locker(lock);
        qDebug() << fileName;
        foreach(DictionaryIndex* index, indexes)
        {
            foreach(QString word, unique)
            {
                index->addWord(word);
            }
        }
    }
}

void findInIndex(QString index_name, QString query, Index* index)
{
    QTextStream out(stdout);

    out << "*" << index_name << endl;
    QStringList list = index->findPhrase(query);
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

void findInIndexDic(QString index_name, QString query, DictionaryIndex* index)
{
    QTextStream out(stdout);

    out << "*" << index_name << endl;
    QStringList list = index->findWord(query.trimmed());
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
#include "PairStorage.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    if(args.size() > 1)
    {
        QString command = args[1];
        if(command == "LIST")
        {
            processDocuments(args[2], args[3], 2500000, 4);
            return 0;
        }
        else if(command == "INVERT")
        {
            QStringList list;
            QString dirname = args[2];
            QDir dir(dirname);
            QStringList files = dir.entryList();
            for(int i=0; i<files.size(); ++i)
            {
                QString name = files[i];
                if(name != "." && name != ".." && name != "dictionary.txt")
                {
                    list.append(dir.filePath(name));
                }

            }
            //qDebug() << list << args[3];
            Merger merger(list);

            merger.saveToFile(args[3]);

            return 0;
        }
        else if(command == "OPTIMISE")
        {
            optimise(args[2], args[3]);
            return 0;
        }
        else if(command == "OPTIMISE_INDEX")
        {
            optimiseIndex(args[2], args[2]+".bin");
            return 0;
        }
    }

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

    TwoWordInvertedIndex two_inverted;
    CordinateInvertedIndex cordinate_inverted;


    //indexes.push_back(&matrix);
    //indexes.push_back(&inverted);
    indexes.push_back(&two_inverted);
    indexes.push_back(&cordinate_inverted);


    DictionaryIndexes dic_index;
    TreeIndex tree_index;
    KGramsIndex kgrams(3);
    PermutermIndex permut_index;

    dic_index.push_back(&tree_index);
    dic_index.push_back(&kgrams);
    //dic_index.push_back(&permut_index);

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
                processFileDic(file, dic_index);
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
            findInIndexDic("Tree  ", query, &tree_index);
            findInIndexDic("KGrams  ", query, &kgrams);
            findInIndexDic("Permut  ", query, &permut_index);
        }
    }

    //saveDictionaryToFile(dictionary, args[2]);
    int nMilliseconds = myTimer.elapsed();
    out << "---------------------" << endl;
    out << "Time: \t" << nMilliseconds << endl;

    return 0;
}
