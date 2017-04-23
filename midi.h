#ifndef MIDITHREAD_H
#define MIDITHREAD_H

#include <QThread>
#include <QtDebug>
#include <QList>

#include "RtMidi.h"
#include "exercise.h"
#include "track.h"
#include "midiDefines.h"

class Midi : public QThread
{
	Q_OBJECT

public:
	bool initialize();
	void deinitialize();

	Midi(QThread *parent = 0);
	~Midi();

    QStringList getPorts();

public slots:
	void play(Exercise pExercise, quint16 pSpeed);
	void pause();
	void stop();

	void silence();

	void setVolume(int pVolume);

protected:
	void run();

signals:
	void newSpeedStep(unsigned int pNewSpeedStep, unsigned int pNewSpeedStepBPM);
	void newStep(unsigned int pNewStep);
	void stopped();
	void paused();

private:
	RtMidiOut *midiout;

	unsigned int volume;
	unsigned int currentStep;
	quint16 currentSpeed;

	bool playing;

	Exercise currentExercise;

	std::vector<unsigned char> getModeMessage(unsigned int MSG_MODE);

    QStringList ports;
};

#endif // MIDITHREAD_H
