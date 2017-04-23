#include "exercise.h"

Exercise::Exercise(const QString &pId, const QString &pName, const QString &pAuthor, const QString &pDescription, QList<Track> &pTracks)
{
	id = pId;
	name = pName;
	author = pAuthor;
	description = pDescription;
	tracks = pTracks;
}

void Exercise::generateClickTrack(int pUpbeatSteps, int pDownbeatSteps, int pSourceTrackIndex)
{
	// Alle pUpbeatSteps wird ein Upbeat-Akzent gesetzt, alle pDownbeatSteps ein Downbeat-Step
	int length = tracks[pSourceTrackIndex].getStepCount();

	// Percussion-Instrument erzeugen

	Instrument instrument("Percussion");

	QList<quint8> stringTunings;
	stringTunings.append(0);

	// Eine Stimme sollte erstmal reichen
	instrument.setStringCount(1);
	instrument.setStringTunings(stringTunings);
	instrument.setPercussion(true);

	// Standard-Kit setzen
	instrument.setMidiProgram(0);

	Track track(instrument, 1, length);

	Note noteUpbeat(56, 127);
	Note noteDownbeat(37, 127);

	int s = 0;

	while(s < length)
	{
		track.setNote(noteDownbeat, 0, s);
		s += pDownbeatSteps;
	}

	s = 0;

	while(s < length)
	{
		track.setNote(noteUpbeat, 0, s);
		s += pUpbeatSteps;
	}

	tracks.append(track);
}

QDataStream &operator <<(QDataStream &out, const Exercise &exercise)
{
	out << exercise.getId()
		<< exercise.getName()
		<< exercise.getAuthor()
		<< exercise.getDescription();

	out << exercise.tracks.size();

	for(int t = 0; t < exercise.tracks.size(); t++)
	{
		out	<< exercise.tracks.at(t);
	}

	return out;
}

QDataStream &operator >>(QDataStream &in, Exercise &exercise)
{
	QString id;
	QString name;
	QString author;
	QString description;
	QList<Track> tracks;

	in	>> id
		>> name
		>> author
		>> description;

	int trackCount;

	in >> trackCount;

	for(int t = 0; t < trackCount; t++)
	{
		Track track;

		in	>> track;

		tracks.append(track);
	}

	exercise.setId(id);
	exercise.setName(name);
	exercise.setAuthor(author);
	exercise.setDescription(description);
	exercise.setTracks(tracks);

	return in;
}
