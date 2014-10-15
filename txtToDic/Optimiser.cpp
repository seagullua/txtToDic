#include "Optimiser.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <fstream>
#include <iostream>
#include <stdint.h>
inline void writeNumber(std::ostream& os, unsigned int number)
{
    unsigned int SEPARATOR = 1 << 7;
    while(number > 0)
    {
        uint8_t byte = number % SEPARATOR;
        if(number < SEPARATOR)
        {
            byte |= SEPARATOR;
        }
        number /= SEPARATOR;
        os.write((char*)&byte, 1);
    }
}

void optimiseIndex(QString input_file, QString output_file)
{
    std::ofstream file(output_file.toStdString(), std::ios::out | std::ios::binary);

    QFile f(input_file);
    if (f.open(QIODevice::ReadOnly))
    {
        int words_processed = 0;
        QTextStream stream(&f);
        while(!stream.atEnd())
        {
            words_processed++;
            QString line = stream.readLine();
            QStringList ids = line.split(' ');

            if(ids.size() >= 2)
            {

                int word_id = ids[0].toInt();
                int last_doc_id = 0;
                for(int i=1; i<ids.size(); ++i)
                {
                    int doc_id = ids[i].toInt();
                    int diff = doc_id - last_doc_id;
                    last_doc_id = doc_id;

                    writeNumber(file, diff);
                }

                if(words_processed % 500 == 0)
                {
                    qDebug() << words_processed << word_id;
                }
            }

        }
    }

}
