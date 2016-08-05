#include "pulsesampler.h"
#include <QDebug>

// WPM = 2.4 * dots per second, where dot = 2*dit_length
// PARIS 10 times a minute = 10 WPM

const qreal word_gap = 6.5; // 8 "dits"
const qreal char_gap = 3.5;  // 3.5 "dits"

PulseSampler::PulseSampler(QObject *parent) :
    QObject(parent),
    m_lastState(0),
    m_minSpaceTimeMeasured(1000.0),
    m_numSpacesMeasured(0)
{
    m_timer.restart();
}

void PulseSampler::reset()
{
    m_minSpaceTimeMeasured = 1000.0;
    m_timer.restart();
}

int PulseSampler::currentWPM()
{
    // WPM = 2.4 * dots per second, where dot = 2*dit_length
    qreal dots_per_second = 1000.0 / (m_minSpaceTimeMeasured * 2.0);
    int wpm = static_cast<int>(2.4 * dots_per_second + 0.5);
    return wpm;
}

void PulseSampler::pulseStateChanged(int state)
{
    if (state != m_lastState)
    {
        qreal msElapsed = static_cast<qreal>(m_timer.elapsed());
        if (state)  // now on, so just finished an off pulse
        {
            if (msElapsed < m_minSpaceTimeMeasured)
            {
                m_minSpaceTimeMeasured = msElapsed;

                // Assume maximum 20WPM, so don't allow minimum dit/space length to go below this.
                // 20WPM => 60ms dit length
                if (m_minSpaceTimeMeasured < 60.0)
                {
                    m_minSpaceTimeMeasured = 60.0;
                }
            }

            if (msElapsed > (word_gap * m_minSpaceTimeMeasured))
            {
                // For long pauses, emit a comma, so the decoder
                // knows this is the beginning of a new word.
                emit newMarkAvailable(',');
                qDebug() << "word gap in ms:" << msElapsed;
            }
            else if (msElapsed > (char_gap * m_minSpaceTimeMeasured))
            {
                // For short pauses, emit a space, so the decoder knows
                // this is a gap between characters.
                emit newMarkAvailable(' ');
            }
            else
            {
                // There will always be small spaces between the dashes & dots,
                // but we don't need to report those.  We only want to report
                // the spaces that separate characters and words.
            }
        }
        else    // now off, so just finished an on pulse
        {
            // if on pulse is roughly equal to minimum off pulse time,
            // then we have a dot, otherwise a dash.
            if (msElapsed > (2.5*m_minSpaceTimeMeasured))
            {
                emit newMarkAvailable('-');
            }
            else
            {
                emit newMarkAvailable('.');
            }
        }
        m_timer.restart();
    }
    else
    {
        if (state == 0)
        {
            // If we're being reminded that the key/pulse is still off,
            // then go ahead and emit a word separator.
            emit newMarkAvailable(',');
        }
    }
    m_lastState = state;
}
