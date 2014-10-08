#ifndef MERGER_H
#define MERGER_H
#include "PairStorage.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <memory>
#include <QVector>

class FileReader
{
public:
    FileReader(QString file_name, bool read_header=true);
    ~FileReader();
    bool hasNext() const
    {
        return _doc.term != 0;
    }
    TermDoc next() const
    {
        return _doc;
    }
    void pop();
    int _readed;
private:
    TermDoc _doc;
    QFile _file;
    QTextStream* _stream;
    int _total;

};

class Merger
{
public:
    Merger(QStringList files);

    void saveToFile(QString output);
private:
    QVector<std::shared_ptr<FileReader> > _readers;
};


void optimise(QString input, QString output);
#endif // MERGER_H
