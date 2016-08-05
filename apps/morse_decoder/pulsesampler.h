#ifndef PULSESAMPLER_H
#define PULSESAMPLER_H

#include <QObject>
#include <QElapsedTimer>

/**
 * @brief The PulseSampler class listens to on/off state changes,
 * and decodes these into morse code "marks" (dashes, dots, and spaces).
 *
 * Types of marks:
 *  '-' = dash
 *  '.' = dot
 *  ' ' = intra-character space
 *  ',' = intra-word space
 */

class PulseSampler : public QObject
{
    Q_OBJECT
public:
    explicit PulseSampler(QObject *parent = 0);
    void reset();
    int currentWPM();
    
signals:
    void newMarkAvailable(char mark);
    
public slots:
    void pulseStateChanged(int state);
    
private:
    QElapsedTimer m_timer;
    int m_lastState;
    qreal m_minSpaceTimeMeasured;  // in milliseconds
    int m_numSpacesMeasured;
};

#endif // PULSESAMPLER_H
