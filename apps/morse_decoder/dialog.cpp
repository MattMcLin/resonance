#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QAudioFormat>
#include <QtMath>
#include <QThread>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_listening(false),
    m_audio(nullptr),
    m_audioIoDevice(nullptr),
    m_playSine(false),
    m_phase(0)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButtonPlay, SIGNAL(released()), this, SLOT(onPlayback()));

    m_stoppedTimer.setInterval(1000);
    m_stoppedTimer.setSingleShot(true);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    m_audio = new QAudioOutput(format, this);

    m_audio->setBufferSize(8190);
    qDebug() << "buffer size:" << m_audio->bufferSize();
    m_audio->setNotifyInterval(20);
    qDebug() << "notify interval:" << m_audio->notifyInterval();

    connect(m_audio, &QAudioOutput::notify, this, &Dialog::onAudioNotify);

    m_audioIoDevice = m_audio->start();
    qDebug() << "periodSize:" << m_audio->periodSize();

    m_player = new PulsePlayer(m_audioIoDevice);

    connect(this, &Dialog::keyStateChanged, &m_sampler, &PulseSampler::pulseStateChanged);
    connect(&m_sampler, &PulseSampler::newMarkAvailable, this, &Dialog::onDecodedMark);
    connect(&m_sampler, &PulseSampler::newMarkAvailable, &m_decoder, &Decoder::newInput);
    connect(&m_decoder, &Decoder::stringUpdated, this, &Dialog::onDecodedText);
    connect(&m_stoppedTimer, &QTimer::timeout, this, &Dialog::onStoppedTimerExpired);
    connect(m_player, &PulsePlayer::keyOn, this, &Dialog::onKeydown);
    connect(m_player, &PulsePlayer::keyOff, this, &Dialog::onKeyup);
    connect(this, &Dialog::playSequence, m_player, &PulsePlayer::slot_playSequence, Qt::QueuedConnection);
    m_player->start();
}

Dialog::~Dialog()
{
    delete m_player;
    m_audio->stop();
    delete m_audio;
    delete ui;
}

void Dialog::onPlayback()
{
    QString sequence = m_decoder.encodePulseSequenceFromText(ui->textForPlayback->toPlainText());
    ui->codedText->setPlainText(sequence);
    //const int wpm = 15.0;
    const int wpm = m_sampler.currentWPM();
    emit playSequence(sequence, wpm);
}

void Dialog::onListen() // for now, treating this as a reset button
{
    /*
    m_listening = !m_listening;

    if (m_listening)
    {
        ui->pushButton->setText("Stop listening");
    }
    else
    {
        ui->pushButton->setText("Listen and decode");
    }
    */

    m_sampler.reset();
    m_decoder.reset();
    ui->morseText->setPlainText(QString());
    ui->decodedText->setPlainText(QString());
}

void Dialog::onKeydown()
{
    m_playSine = true;
    emit keyStateChanged(1);
    ui->pushButtonKey->setText("Key*");

    //onAudioNotify();
}

void Dialog::onKeyup()
{
    m_playSine = false;
    emit keyStateChanged(0);
    ui->pushButtonKey->setText("Key");
    m_stoppedTimer.start();
}

void Dialog::onDecodedMark(char mark)
{
    QString txtToAdd;
    // Can't use appendPlainText, because that inserts a newline each time.
    if (mark == ',')
    {
        // for display purposes, convert intra-word mark into a space
        txtToAdd.append(' ');
        txtToAdd.append(' ');
    }
    else
    {
        txtToAdd = QString(mark);
    }
    ui->morseText->setPlainText(ui->morseText->toPlainText() + txtToAdd);
    QString wpmStr("Detected WPM: ");
    wpmStr.append(QString::number(m_sampler.currentWPM()));
    ui->wpmLabel->setText(wpmStr);
}

void Dialog::onDecodedText(QString &sDecodedText)
{
    ui->decodedText->setPlainText(sDecodedText);
}

void Dialog::onStoppedTimerExpired()
{
    // Force a space mark to be emitted, so that the previos
    // morse character gets decoded.
    emit keyStateChanged(0);
}

void Dialog::onAudioNotify()
{
    //qDebug() << "audio state before:" << m_audio->state();
    int periodSize = m_audio->periodSize();
    int bytesToWrite = m_audio->bytesFree();

    while (bytesToWrite < periodSize)
    {
        QThread::msleep(10);
        bytesToWrite = m_audio->bytesFree();
    }

    //qDebug() << "audio bytes free before:" << bytesToWrite;
    if (bytesToWrite > 0)
    {
        const int SAMPLES_PER_SEC = 44100;
        const int samples = bytesToWrite / 2;
        qint16* buf = new qint16[samples];

        int i;
        for (i = 0; i<samples; ++i)
        {
            qreal freq = 440.0;
            qreal theta = ((freq * 2.0 * 3.1415926) / (qreal)SAMPLES_PER_SEC) * static_cast<qreal>(i + m_phase);
            qreal multiplier = m_playSine ? 32767.0 : 0.0;

            buf[i] = static_cast<qint16>((sin(theta)) * multiplier);
        }

        char* pBuf = reinterpret_cast<char*>(buf);
        i = 0;
        while (bytesToWrite > 0)
        {
            int bytesInChunk = qMin(bytesToWrite,periodSize);
            qint64 bytesWritten = m_audioIoDevice->write(&(pBuf[i]), bytesInChunk);
            if (bytesWritten > 0)
            {
                bytesToWrite -= bytesWritten;
                i += bytesWritten;
                //qDebug() << "audio bytes written:" << bytesWritten;
            }
            else
            {
                qDebug() << "error";
                bytesToWrite = 0;
            }
        }

        m_phase += samples;
        m_phase %= SAMPLES_PER_SEC;
        //qDebug() << "phase: " << m_phase;

        delete[] buf;
    }

    //qDebug() << "audio state after:" << m_audio->state();
}
