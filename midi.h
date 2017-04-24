#ifndef MIDITHREAD_H
#define MIDITHREAD_H

#include <QThread>
#include <QtDebug>
#include <QList>

#include "RtMidi.h"
#include "midiDefines.h"

class Midi : public QThread {
    Q_OBJECT

  public:
    bool initialize();
    void deinitialize();

    Midi(QThread* parent = 0);
    ~Midi();

    QStringList getPorts();

  public slots:
    void play();
    void play(quint16 pSpeed, uint pBeats, uint pAccBeat, uint pLength);
    void pause();
    void stop();

    void silence();

    void setVolume(int pVolume);

  protected:
    void run();

  signals:
    void stopped();
    void paused();

  private:
    RtMidiOut* midiout;

    unsigned int volume;
    unsigned int currentStep;
    quint16 currentSpeed;
    uint beats;
    uint accBeat;
    uint length;

    bool playing;

    std::vector<unsigned char> getModeMessage(unsigned int MSG_MODE);

    QStringList ports;
};

#endif // MIDITHREAD_H
