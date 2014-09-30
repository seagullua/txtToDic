#include "DictionaryIndex.h"
#include <QDebug>
QString reverse(const QString s)
{
    QString result = s;
    for(int i=0; i<result.size()/2; ++i)
    {
        result[i] = result[result.size()-i-1];
    }
    return result;
}

bool DictionaryIndex::isWordFits(QString word, QString pattern) const
{
    QStringList list = pattern.split("*");
    QString start_with = list[0];
    QString ends_with = list[list.size()-1];


    return word.mid(0, start_with.size()) == start_with &&
            word.mid(word.size()-ends_with.size()) == ends_with;
}
QStringList DictionaryIndex::findWord(QString word) const
{
    QStringList parts = word.split("*");
    QString first = parts[0];
    QString last = parts[parts.size() - 1];
    QStringList found = findWord(first, last);
    return ensureWordFits(found, word);
}
QStringList DictionaryIndex::ensureWordFits(const QStringList& list, QString pattern) const
{
    QStringList res;
    foreach(QString a , list)
    {
        if(isWordFits(a, pattern))
        {
            res.push_back(a);
        }
    }
    return res;
}

void KGramsIndex::addWord(QString word)
{
    QStringList kgrams = cutToKGrams(_prefix + word + _prefix);
    foreach(QString kgram, kgrams)
    {
        Words& set = _dictionary[kgram];
        set.insert(word);
    }
}

QStringList KGramsIndex::findWord(QString first, QString last) const
{
    Words first_word = getWordsWithKGrapms(_prefix + first);
    Words second_word = getWordsWithKGrapms(last + _prefix);


    QStringList words = first_word.intersect(second_word).toList();
    //qDebug() << first_word;
    //qDebug() << second_word;
    //qDebug() << second_word;
    return words;
}

KGramsIndex::Words KGramsIndex::getWordsWithKGrapms(QString word) const
{
    Words res;
    QStringList kgrams = cutToKGrams(word);
    foreach(QString kgram, kgrams)
    {
        res.unite(_dictionary[kgram]);
    }
    return res;
}
QStringList KGramsIndex::cutToKGrams(QString word) const
{
    QStringList res;
    word = word;
    for(int i=_n; i<=word.size(); ++i)
    {
        res.push_back(word.mid(i-_n, _n));
    }
    return res;
}

void Tree::insertWord(QString part, QString full_word, NodePtr node)
{
    if(part.length() == 0)
    {
        node->_word.insert(full_word);
    }
    else
    {
        QChar ch = part[0];
        QString other = part.mid(1);

        NodePtr& next_node = node->_children[ch];
        if(next_node.get() == 0)
        {
            next_node = std::make_shared<Node>();
        }

        insertWord(other, full_word, next_node);
    }
}
Tree::Tree()
    : _root(new Node)
{

}

void Tree::addWord(QString word, QString payload)
{
    insertWord(word, payload, _root);
}

QSet<QString> Tree::getChildWords(NodePtr node) const
{
    QSet<QString> list = node->_word;
    foreach(NodePtr n, node->_children)
    {
        list = list.unite(getChildWords(n));
    }
    return list;
}
Tree::NodePtr Tree::findNode(NodePtr start, QString string) const
{
    if(start.get() == 0)
    {
        return start;
    }
    if(string.length() == 0)
    {
        return start;
    }
    else
    {
        NodePtr n = start->_children[string[0]];
        return findNode(n, string.mid(1));
    }
}

QSet<QString> Tree::startsFrom(QString string) const
{

    NodePtr node = findNode(_root, string);
    if(node.get())
    {
        return getChildWords(node);
    }
    return QSet<QString>();
}

void TreeIndex::addWord(QString word)
{
    _prefix.addWord(word, word);
    QString inversed = reverse(word);
    _suffix.addWord(inversed, word);
}

QStringList TreeIndex::findWord(QString first, QString last) const
{
    QString inversed = reverse(last);

    QSet<QString> set = _prefix.startsFrom(first);
    QSet<QString> set2 = _suffix.startsFrom(inversed);

    return set.intersect(set2).toList();
}

QString PermutermIndex::makePermutation(QString word) const
{
    QString res = word[word.size()-1] + word.mid(0, word.size()-1);
    return res;
}

void PermutermIndex::addWord(QString word)
{
    QString permutated = word+"$";
    QString next = permutated;

    do
    {
        _tree.addWord(next, word);
        next = makePermutation(next);
    } while(next != permutated);
}

QStringList PermutermIndex::findWord(QString word) const
{
    if(word.contains("*"))
    {
        QString token = word + "$";
        while(token[0] != '*')
        {
            token = makePermutation(token);
        }

        QString search = token.mid(1);
        return _tree.startsFrom(search).toList();
    }
    return QStringList();
}
