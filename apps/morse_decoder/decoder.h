#ifndef DECODER_H
#define DECODER_H

#include <QObject>
#include <QString>

struct DecodeTable
{
    char c[16];       // letter/number
    char code[16];   // dash/dot sequence for code
};

class Decoder : public QObject
{
    Q_OBJECT
public:
    explicit Decoder(QObject *parent = 0);

    void reset();

    void decodeString(const QString& sequence);
    void decodeChar(const QString& sequence);

    QString encodePulseSequenceFromText(const QString& text);
    
signals:
    // TODO: do I need this signal, or can I use some kind of built-in "on changed" signal?
    void stringUpdated(QString& sDecodedText);
    
public slots:
    void newInput(char c);

private:
    QString m_strOut;
    QString m_dashdotSequence;
    
};

#endif // DECODER_H
