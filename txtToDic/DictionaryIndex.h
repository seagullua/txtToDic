#ifndef DICTIONARYINDEX_H
#define DICTIONARYINDEX_H
#include <QString>
#include <QMap>
#include <QSet>
#include "txtToDic.h"
#include <memory>

class DictionaryIndex
{
public:
    virtual ~DictionaryIndex() {}
    virtual void addWord(QString word) = 0;
    virtual QStringList findWord(QString part) const;
protected:
    virtual QStringList findWord(QString starts_from, QString ends_in) const
    {
        return QStringList();
    }
    bool isWordFits(QString word, QString pattern) const;
    QStringList ensureWordFits(const QStringList& list, QString pattern) const;
};

class KGramsIndex : public DictionaryIndex
{
public:
    KGramsIndex(int n)
        : _n(n)
    {
        for(int i=1; i<_n; ++i)
        {
            _prefix.push_back(" ");
        }
    }

    void addWord(QString word);
    /**
     * @brief supports asterisk search
     * @param part
     * @return
     */
    QStringList findWord(QString starts_from, QString ends_in) const;
private:
    QString _prefix;
    QStringList cutToKGrams(QString word) const;
    typedef QSet<QString> Words;
    Words getWordsWithKGrapms(QString word) const;
    int _n;
    typedef QMap<QString, QSet<QString> > Dictionary;
    Dictionary _dictionary;
};

class Tree
{
public:
    Tree();
    void addWord(QString word, QString payload);
    QSet<QString> startsFrom(QString string) const;
private:

    struct Node;
    typedef std::shared_ptr<Node> NodePtr;
    typedef QMap<QChar, NodePtr> Children;
    struct Node
    {
        Node()
        {}
        QSet<QString> _word;
        Children _children;
    };

    QSet<QString> getChildWords(NodePtr node) const;
    void insertWord(QString part, QString full_word, NodePtr node);
    NodePtr findNode(NodePtr start, QString string) const;
    NodePtr _root;
};

class TreeIndex : public DictionaryIndex
{
public:
    void addWord(QString word);
    QStringList findWord(QString starts_from, QString ends_in) const;
private:
    Tree _prefix;
    Tree _suffix;
};

class PermutermIndex : public DictionaryIndex
{
public:
    void addWord(QString word);
    QStringList findWord(QString part) const;
private:
    QString makePermutation(QString word) const;
    Tree _tree;
};

#endif // DICTIONARYINDEX_H
