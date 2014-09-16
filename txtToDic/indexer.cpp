#include "indexer.h"
#include <QDebug>
int Index::getOrAddWordID(WordMap& map, QString word)
{
    int id = getWordID(map, word);
    if(id < 0)
    {
        id = map.size();
        map.insert(word, id);
    }
    return id;
}

int Index::getWordID(const WordMap& map, QString word)
{
    int id = -1;
    auto it = map.find(word);
    if(it != map.end())
    {
        id = it.value();
    }
    return id;
}

QString Index::getWordByID(const WordMap& map, int id)
{
    foreach(QString key, map.keys())
    {
        if(map[key] == id)
        {
            return key;
        }
    }
    return QString();
}

void Index::findRun(QString query, const Callback& callback) const
{
    QString AND = "and";
    QString OR = "or";
    QString NOT = "not";

    QStringList list = processString(query);
    Operation op = Operation::Or;
    bool op_found = true;
    bool op_not = false;

    foreach(QString word, list)
    {
        if(word.size())
        {
            if(word == AND || word == OR)
            {
                //Operation meet
                op_found = true;
                op_not = false;
                if(word == AND)
                {
                    op = Operation::And;
                }
                else
                {
                    op = Operation::Or;
                }
            }
            else if(word == NOT)
            {
                //Not
                op_not = true;
            }
            else
            {
                //Word
                if(!op_found)
                {
                    op_not = false;
                    op = Operation::And;
                }
                callback(word, op, op_not);
                op_found = false;
                op_not = false;

            }
        }
    }
}

MatrixIndex::MatrixIndex(QStringList files)
{
    //Add file to the list
    foreach(QString file, files)
    {
        getOrAddWordID(_files, file);
    }
    _empty_row = Row(_files.size(), false);
}

void MatrixIndex::saveWord(int file_id, int word_id)
{
    if(word_id >= _container.size())
    {
        _container.resize(word_id*2 + 1, _empty_row);
    }
    _container[word_id][file_id] = true;
}

void MatrixIndex::addToIndex(const QString& file_name,
                const Dictionary& dictionary)
{
    int file_id = getWordID(_files, file_name);
    if(file_id < 0)
    {
        qDebug() << "Wrong file" << file_name;
        return;
    }

    foreach(QString word, dictionary)
    {
        int word_id = getOrAddWordID(_words, word);
        saveWord(file_id, word_id);
    }
}

template<class T>
T searchHelper(const T& first,
               QString query,
               std::function<void(QString, Index::Callback)> findRun,
               std::function<T(const QString&)> search_word,
               std::function<void (T&,const T&)> f_and,
               std::function<void (T&,const T&)> f_or,
               std::function<void (T&)> f_not)
{
    T acum = first;
    findRun(query, [=, &acum](QString word, Operation op, bool is_not){
        T w = search_word(word);
        if(is_not)
        {
            f_not(w);
        }
        if(op == Operation::And)
        {
            f_and(acum, w);
        }
        else
        {
            f_or(acum, w);
        }
    });
    return acum;
}

MatrixIndex::Row MatrixIndex::findWord(QString word) const
{
    int word_id = getWordID(_words, word);
    if(word_id < 0)
    {
        return _empty_row;
    }
    else
    {
        return _container[word_id];
    }
}

QStringList MatrixIndex::find(QString query) const
{
    auto AND = [](Row& a, const Row& b) {
        std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::logical_and<bool>());
    };
    auto OR = [](Row& a, const Row& b) {
        std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::logical_or<bool>());
    };
    auto NOT = [](Row& a) {
        std::transform(a.begin(), a.end(), a.begin(), std::logical_not<bool>());
    };

    auto find_run = [this](QString a, Index::Callback b) {
        findRun(a, b);
    };

    auto find_word = [this](QString w)->Row{
        return this->findWord(w);
    };

    Row res = searchHelper<Row>(_empty_row, query, find_run, find_word, AND, OR, NOT);


    QStringList files;
    for(int i=0; i<res.size(); ++i)
    {
        if(res[i])
            files.append(getWordByID(_files, i));
    }

    return files;
}

void InvertedIndex::saveWord(int file_id, int word_id)
{
    if(word_id >= _container.size())
    {
        _container.resize(word_id*2 + 1);
    }
    List& list = _container[word_id];
    list.push_back(file_id);
    if(list.size() >= 2 && list[list.size() - 1] < list[list.size() - 2])
    {
        std::sort(list.begin(), list.end());
    }
}

void InvertedIndex::addToIndex(const QString& file_name,
                const Dictionary& dictionary)
{
    int file_id = getOrAddWordID(_files, file_name);
    foreach(QString word, dictionary)
    {
        int word_id = getOrAddWordID(_words, word);
        saveWord(file_id, word_id);
    }
}
InvertedIndex::List InvertedIndex::findWord(QString word) const
{
    int id = getWordID(_words, word);
    if(id < 0)
    {
        return List();
    }
    else
    {
        return _container[id];
    }
}

QStringList InvertedIndex::find(QString query) const
{
    List file_list = _files.values().toVector().toStdVector();
    std::sort(file_list.begin(), file_list.end());
    auto AND = [](List& a, const List& b) {
        List res(a.size()+b.size());
        auto it= std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), res.begin());
        res.resize(it-res.begin());
        //std::sort(res.begin(), res.end());
        a = res;
    };
    auto OR = [](List& a, const List& b) {
        List res(a.size()+b.size());
        auto it= std::set_union(a.begin(), a.end(), b.begin(), b.end(), res.begin());
        res.resize(it-res.begin());
        //std::sort(res.begin(), res.end());
        a = res;
    };
    auto NOT = [&file_list](List& a) {
        List res(file_list.size());
        auto it= std::set_difference (file_list.begin(), file_list.end(), a.begin(), a.end(), res.begin());
        res.resize(it-res.begin());
        //std::sort(res.begin(), res.end());
        a = res;
    };

    auto find_run = [this](QString a, Index::Callback b) {
        findRun(a, b);
    };

    auto find_word = [this](QString w)->List{
        return this->findWord(w);
    };

    List res = searchHelper<List>(List(), query, find_run, find_word, AND, OR, NOT);

    QStringList files;
    foreach(int id, res)
    {
        files.append(getWordByID(_files, id));
    }

    return files;
}

