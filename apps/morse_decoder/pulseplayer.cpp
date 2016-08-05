#include "pulseplayer.h"
#include <QThread>

PulsePlayer::PulsePlayer(QIODevice *audioDevice)
    : m_audioDevice(audioDevice)
{

}

void PulsePlayer::slot_playSequence(const QString &sequence, int wpm)
{
    m_sequence = sequence;
    m_wpm = wpm;
}

void PulsePlayer::playSequence(const QString &sequence, int wpm)
{
    // WPM = 2.4 * dots per second, where dot = 2*dit_length
    const qreal dots_per_second = static_cast<qreal>(wpm) / 2.4;
    const qreal spaceTime = 0.5 * (1000.0 / dots_per_second);
    const int delay = static_cast<int>(spaceTime);

    for (int i=0; i<sequence.length(); ++i)
    {
        int spaceCount = 0;

        char c = sequence[i].toLatin1();
        switch (c)
        {
        case '.':
            emit keyOn();
            QThread::msleep(delay);
            emit keyOff();
            QThread::msleep(delay);
            spaceCount = 0;
            break;
        case '-':
            emit keyOn();
            QThread::msleep(3*delay);
            emit keyOff();
            QThread::msleep(delay);
            spaceCount = 0;
            break;
        case ' ':
            if (spaceCount == 0)
            {
                // A space repesents break between letters, not words.
                // char space is 3 dots, but we already waited for one, so 2 left.
                QThread::msleep(2*delay);
            }
            else
            {
                // Break between words is 7 dots (1 + 6).
                QThread::msleep(6*delay);
            }
            spaceCount++;
            break;
        }
    }
}

void PulsePlayer::run()
{
    while (1)
    {
        if (m_sequence.length() > 0)
        {
            playSequence(m_sequence, m_wpm);
            m_sequence.clear();
        }
        QThread::sleep(10);
    }
}
