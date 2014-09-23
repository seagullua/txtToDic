#ifndef INDEXER_H
#define INDEXER_H
#include "txtToDic.h"
#include <QMap>
#include <vector>
#include <set>
#include <functional>

typedef QMap<QString, int> WordMap;

enum class Operation
{
    Or,
    And
};

class Index
{
public:
    typedef std::function<void(QString, Operation, bool)> Callback;
    virtual ~Index(){}

    virtual void addToIndex(const QString& file_name,
                            const Dictionary& dictionary)=0;
    virtual QStringList find(QString) const
    {
        return QStringList();
    }
    virtual QStringList findPhrase(QString) const
    {
        return QStringList();
    }

protected:

    void findRun(QString query, const Callback& callback) const;
    static int getOrAddWordID(WordMap& map, QString word);
    static int getWordID(const WordMap& map, QString word);
    static QString getWordByID(const WordMap& map, int id);
};

class MatrixIndex : public Index
{
public:
    MatrixIndex(QStringList files);

    void addToIndex(const QString& file_name,
                    const Dictionary& dictionary);
    QStringList find(QString query) const;

private:
    typedef std::vector<bool> Row;
    typedef std::vector<Row> MatrixIndexContainer;

    Row findWord(QString word) const;
    Row _empty_row;
    void saveWord(int file_id, int word_id);
    WordMap _files;
    WordMap _words;
    MatrixIndexContainer _container;
};

class InvertedIndex : public Index
{
public:
    void addToIndex(const QString& file_name,
                    const Dictionary& dictionary);
    QStringList find(QString query) const;
private:
    void saveWord(int file_id, int word_id);

    WordMap _files;
    WordMap _words;

    typedef std::vector<int> List;
    typedef std::vector<List> InvertedIndexContainer;
    List findWord(QString word) const;
    InvertedIndexContainer _container;
};

class TwoWordInvertedIndex : public Index
{
public:
    void addToIndex(const QString& file_name,
                    const Dictionary& dictionary);
    QStringList findPhrase(QString str) const;
private:
    void saveWord(int file_id, int word_id);

    WordMap _files;
    WordMap _words;

    QStringList splitToPairs(const Dictionary& dictionary) const;
    typedef QSet<int> List;
    typedef QMap<int, List> InvertedIndexContainer;
    InvertedIndexContainer _container;
};

class CordinateInvertedIndex : public Index
{
public:
    void addToIndex(const QString& file_name,
                    const Dictionary& dictionary);
    QStringList findPhrase(QString str) const;
private:
    void saveWord(int file_id, int word_id, int coordinate);

    WordMap _files;
    WordMap _words;

    QStringList splitToPairs(const Dictionary& dictionary) const;
    typedef QSet<int> Coordinates;
    typedef QMap<int, Coordinates> List;
    typedef QMap<int, List> InvertedIndexContainer;
    InvertedIndexContainer _container;
};


#endif // INDEXER_H
