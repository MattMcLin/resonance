#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSoundEffect>
#include <QTimer>
#include <QAudioOutput>
#include <QFile>
#include "decoder.h"
#include "pulsesampler.h"
#include "pulseplayer.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

signals:
    void keyStateChanged(int state);
    void playSequence(const QString& sequence, int wpm);

public slots:
    void onPlayback();
    void onListen();
    void onKeydown();
    void onKeyup();
    void onDecodedMark(char mark);
    void onDecodedText(QString& sDecodedText);
    void onStoppedTimerExpired();
    void onAudioNotify();
    
private:
    Ui::Dialog *ui;
    bool m_listening;

    PulseSampler m_sampler;
    PulsePlayer* m_player;
    Decoder m_decoder;
    QByteArray m_sampleBuffer;
    QIODevice* m_audioIoDevice;
    QAudioOutput* m_audio;
    QTimer m_stoppedTimer;
    bool m_playSine;
    int m_phase;
};

#endif // DIALOG_H
