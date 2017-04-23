#ifndef EXERCISE_H
#define EXERCISE_H

#include <QString>
#include <QVector>
#include <QDataStream>
#include <QMap>

#include "track.h"

class Exercise
{
public:
	Exercise() {}
	Exercise(const QString &pId, const QString &pName, const QString &pAuthor, const QString &pDescription, QList<Track> &pTracks);

	const QString getId() const { return id; }
	void setId(const QString pId) { id = pId; }

	const QString getName() const { return name; }
	void setName(const QString pName) { name = pName; }

	const QString getAuthor() const { return author; }
	void setAuthor(const QString pAuthor) { author = pAuthor; }

	const QString getDescription() const { return description; }
	void setDescription(const QString pDescription) { description = pDescription; }

	QList<Track> getTracks() const { return tracks; }
	void setTracks(QList<Track> &pTracks) { tracks = pTracks; }

	// Erzeugt einen Clicktrack, der während der Übung abgespielt werden sollen. Der Track wird automatisch an die Trackliste angehängt
	void generateClickTrack(int pUpbeatSteps, int pDownbeatSteps, int pSourceTrackIndex);

private:
	QString id;
	QString name;
	QString author;
	QString description;
	QList<Track> tracks;

	friend QDataStream &operator <<(QDataStream &out, const Exercise &exercise);
	friend QDataStream &operator >>(QDataStream &in, Exercise &exercise);
};

#endif // EXERCISE_H
