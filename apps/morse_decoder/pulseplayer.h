#ifndef PULSEPLAYER_H
#define PULSEPLAYER_H

#include <QObject>
#include <QIODevice>
#include <QThread>

class PulsePlayer : public QThread
{
    Q_OBJECT
public:
    explicit PulsePlayer(QIODevice* audioDevice);

signals:
    void keyOn();
    void keyOff();

public slots:
    void slot_playSequence(const QString& sequence, int wpm);

private:
    void run() Q_DECL_OVERRIDE;
    void playSequence(const QString& sequence, int wpm);

    QIODevice* m_audioDevice;
    QThread m_thread;

    QString m_sequence;
    int m_wpm;
};

#endif // PULSEPLAYER_H
