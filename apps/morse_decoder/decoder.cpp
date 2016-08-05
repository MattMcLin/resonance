#include "decoder.h"
#include <QDebug>

static const DecodeTable decodeTable[] = {
    {" ", "  "}, // when translating from text to morse, make word gaps obvious
    {" ", " "},   // this spacer is for converting in other direction
    {" ", ","},
    {" ", ""},
    {"a", ".-"},
    {"b", "-..."},
    {"c", "-.-."},
    {"d", "-.."},
    {"e", "."},
    {"f", "..-."},
    {"g", "--."},
    {"h", "...."},
    {"i", ".."},
    {"j", ".---"},
    {"k", "-.-"},
    {"l", ".-.."},
    {"m", "--"},
    {"n", "-."},
    {"o", "---"},
    {"p", ".--."},
    {"q", "--.-"},
    {"r", ".-."},
    {"s", "..."},
    {"t", "-"},
    {"u", "..-"},
    {"v", "...-"},
    {"w", ".--"},
    {"x", "-..-"},
    {"y", "-.--"},
    {"z", "--.."},
    {"0", "-----"},
    {"1", ".----"},
    {"2", "..---"},
    {"3", "...--"},
    {"4", "....-"},
    {"5", "....."},
    {"6", "-...."},
    {"7", "--..."},
    {"8", "---.."},
    {"9", "----."},
    {".", ".-.-.-"},
    {",", "--..--"},
    {"?", "..--.."},
    {"!", "-.-.--"},
    {"'", ".----."},
    {":", "---..."},
    {"+", ".-.-."},
    {"-", "-....-"},
    {"<===> ", "-...-"},
    {"<START> ", "-.-.-"}, // starting signal
    {"<END> ", "...-.-"}, // end of work (SK)
    {"<SK> ", "...-.-"}, // end of work (SK)
    {"<ACK> ", "...-."}, // understood
    {"<WAIT> ", ".-..."}, // wait
    {"<SOS> ", "...---..."},
    {"<AR> ", ".-.-."},     // end of transmission
    {"<KN> ", "-.--."},     // done, wait for reply from you only
    {"<error> ", "......."},
    {"<error> ", "........"},
    {"<error> ", "........."},
    {"<error> ", ".........."},
    {"<error> ", "..........."},
};
static const int numDecodeTableEntries = sizeof(decodeTable) / sizeof(DecodeTable);

Decoder::Decoder(QObject *parent) :
    QObject(parent)
{
}

void Decoder::reset()
{
    m_strOut.clear();
    m_dashdotSequence.clear();;
}

void Decoder::decodeString(const QString &sequence)
{
    for (int i=0; i<sequence.length(); i++)
    {
        newInput(sequence[i].toLatin1());
    }
}

void Decoder::decodeChar(const QString& sequence)
{
    // If it mattered, we could do a faster search than this linear one,
    // but for morse-code speeds, it really doesn't matter.
    qDebug() << "decoding sequence:" << sequence;
    for (int i=0; i<numDecodeTableEntries; i++)
    {
        if (sequence == QString(decodeTable[i].code))
        {
            m_strOut.append(decodeTable[i].c);
            emit stringUpdated(m_strOut);
            break;
        }
    }
}

QString Decoder::encodePulseSequenceFromText(const QString &text)
{
    QString sequence;

    int i = 0;
    while (i < text.length())
    {
        QString charToTranslate;
        if (text[i] == '<')
        {

            //int j = i;
            while (text[i] != '>')
            {
                charToTranslate.append(text[i]);
                i++;
            }
            charToTranslate.append(text[i]);
            charToTranslate.append(' ');    // our table has spaces for this codes
        }
        else
        {
            charToTranslate = QString(text[i]).toLower();
            i++;
        }

        for (int iTable=0; iTable<numDecodeTableEntries; iTable++)
        {
            if (charToTranslate == QString(decodeTable[iTable].c))
            {
                sequence.append(QString(decodeTable[iTable].code));
                sequence.append(QString("  "));
                break;
            }
        }
    }

    return sequence;
}


void Decoder::newInput(char c)
{
    switch (c)
    {
    case '-':
    case '.':
        m_dashdotSequence.append(c);
        break;
    case ',':
        decodeChar(m_dashdotSequence);
        m_dashdotSequence.clear();
        m_dashdotSequence.append(c);
        // fall through
    case ' ':
        decodeChar(m_dashdotSequence);
        m_dashdotSequence.clear();
        break;
    }
}
