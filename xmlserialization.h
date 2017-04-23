#ifndef XMLSERIALIZATION_H
#define XMLSERIALIZATION_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMutex>
#include <QDebug>

#include "instrument.h"
#include "note.h"
#include "track.h"
#include "exercise.h"
#include "collection.h"

struct UserSettings
{
	QString name;
	// Exercise-ID und zugehörige speedPoints;
	QMap<QString, SpeedPoints> speedPointsMapping;
};

class XMLSerialization
{
public:
	static XMLSerialization *getInstance();
	static void drop();

	QXmlStreamWriter *writeStartDocument(QString pFilename);
	void writeEndDocument(QXmlStreamWriter *pStreamWriter);

	QXmlStreamReader *readStartDocument(QString pFilename);
	void readEndDocument(QXmlStreamReader *pStreamReader);

	Instrument readInstrument(QXmlStreamReader *pXmlIn);
	void writeInstrument(Instrument &pInstrument, QXmlStreamWriter *pXmlOut) const;

	Note readNote(QXmlStreamReader *pXmlIn);
	void writeNote(Note &pNote, QXmlStreamWriter *pXmlOut) const;

	void writeTrack(Track &pTrack, QXmlStreamWriter *pXmlOut) const;
	Track readTrack(QXmlStreamReader *pXmlIn);

	void writeExercise(Exercise &pExercise, QXmlStreamWriter *pXmlOut) const;
	Exercise readExercise(QXmlStreamReader *pXmlIn);

	void writeCollection(Collection &pCollection, QXmlStreamWriter *pXmlOut) const;
	Collection readCollection(QXmlStreamReader *pXmlIn);

	bool findNextStartElement(QString pName, QXmlStreamReader *pXmlIn);
	bool findNextStartElement(QString pName, QString pStopElement, QXmlStreamReader *pXmlIn);

	QXmlStreamWriter *writeStartSettings();
	void writeEndSettings(QXmlStreamWriter *pStreamWriter);

	void writeUserSettings(UserSettings pUserSettings, QXmlStreamWriter *pXmlOut);
	UserSettings readUserSettings(QXmlStreamReader *pXmlIn);

private:
	XMLSerialization() {}
	XMLSerialization(const XMLSerialization &);
	XMLSerialization &operator=(const XMLSerialization &);

	static XMLSerialization *instance;

	QFile *currentFile;
};

#endif // XMLSERIALIZATION_H
