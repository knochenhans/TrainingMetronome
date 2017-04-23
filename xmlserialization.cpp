#include "xmlserialization.h"

XMLSerialization *XMLSerialization::getInstance()
{
	static QMutex mutex;
	if(!instance)
	{
		mutex.lock();

		if (!instance)
			instance = new XMLSerialization;

		mutex.unlock();
	}

	return instance;
}

void XMLSerialization::drop()
{
	static QMutex mutex;
	mutex.lock();
	delete instance;
	instance = 0;
	mutex.unlock();
}

QXmlStreamWriter *XMLSerialization::writeStartDocument(QString pFilename)
{
	if(!pFilename.isEmpty())
	{
		currentFile = new QFile(pFilename);

		if(currentFile->open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QXmlStreamWriter *xmlOut = new QXmlStreamWriter(currentFile);
			xmlOut->setAutoFormatting(true);

			xmlOut->writeStartDocument();
			return xmlOut;
		}
	}
	return 0;
}

void XMLSerialization::writeEndDocument(QXmlStreamWriter *pStreamWriter)
{
	pStreamWriter->writeEndDocument();
	delete pStreamWriter;

	currentFile->close();
}

QXmlStreamReader *XMLSerialization::readStartDocument(QString pFilename)
{
	if(!pFilename.isEmpty())
	{
		currentFile = new QFile(pFilename);

		if(currentFile->open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QXmlStreamReader *pXmlIn = new QXmlStreamReader(currentFile);

			while(!pXmlIn->atEnd() && !pXmlIn->hasError())
			{
				pXmlIn->readNext();
				if(pXmlIn->isStartDocument())
				{
					// Dokumentanfang gefunden, Rest der Arbeit macht die aufrufende Funktion
					return pXmlIn;
				}
			}

			// Kein Dokumentanfang gefunden: Fehler!
		}
	}
	return 0;
}

void XMLSerialization::readEndDocument(QXmlStreamReader *pXmlIn)
{
	pXmlIn->clear();
	delete pXmlIn;

	currentFile->close();
}

void XMLSerialization::writeInstrument(Instrument &pInstrument, QXmlStreamWriter *pXmlOut) const
{
	pXmlOut->writeStartElement("Instrument");
	pXmlOut->writeTextElement("Name", pInstrument.getName());
	pXmlOut->writeTextElement("StringCount", QString::number(pInstrument.getStringCount()));
	pXmlOut->writeTextElement("FretCount", QString::number(pInstrument.getFretCount()));
	pXmlOut->writeTextElement("MidiProgram", QString::number(pInstrument.getMidiProgram()));
	pXmlOut->writeTextElement("isPercussion", QString::number(pInstrument.isPercussion() ? 1 : 0));

	pXmlOut->writeStartElement("StringTuningList");
	foreach(quint8 i, pInstrument.getStringTunings())
	{
		pXmlOut->writeTextElement("StringTuning", QString::number(i));
	}
	pXmlOut->writeEndElement();

	pXmlOut->writeEndElement();
}

void XMLSerialization::writeNote(Note &pNote, QXmlStreamWriter *pXmlOut) const
{
	pXmlOut->writeStartElement("Note");
	pXmlOut->writeTextElement("Pitch", QString::number(pNote.getPitch()));
	pXmlOut->writeTextElement("Velocity", QString::number(pNote.getVelocity()));
	pXmlOut->writeEndElement();
}

void XMLSerialization::writeTrack(Track &pTrack, QXmlStreamWriter *pXmlOut) const
{
	pXmlOut->writeStartElement("Track");
	pXmlOut->writeStartElement("NoteMap");
	for(int v = 0; v < pTrack.getVoiceCount(); v++)
	{
		pXmlOut->writeStartElement("Voice");
		for(int s = 0; s < pTrack.getStepCount(); s++)
		{
			Note note(pTrack.getNote(v, s));
			XMLSerialization::getInstance()->writeNote(note, pXmlOut);
		}
		pXmlOut->writeEndElement();
	}

	pXmlOut->writeEndElement();

	Instrument instrument(pTrack.getInstrument());
	XMLSerialization::getInstance()->writeInstrument(instrument, pXmlOut);

	pXmlOut->writeEndElement();
}

void XMLSerialization::writeExercise(Exercise &pExercise, QXmlStreamWriter *pXmlOut) const
{
	pXmlOut->writeStartElement("Exercise");
	pXmlOut->writeTextElement("ID", pExercise.getId());
	pXmlOut->writeTextElement("Name", pExercise.getName());
	pXmlOut->writeTextElement("Author", pExercise.getAuthor());
	pXmlOut->writeTextElement("Description", pExercise.getDescription());

	Track track = pExercise.getTracks().at(0);
	XMLSerialization::getInstance()->writeTrack(track, pXmlOut);

	pXmlOut->writeEndElement();
}


Exercise XMLSerialization::readExercise(QXmlStreamReader *pXmlIn)
{
	Exercise exercise;

	while(!pXmlIn->atEnd() && !pXmlIn->hasError())
	{
		pXmlIn->readNext();

		if(pXmlIn->isStartElement())
		{
			if(pXmlIn->name() == "ID")
			{
				exercise.setId(pXmlIn->readElementText());
			}
			else if(pXmlIn->name() == "Name")
			{
				exercise.setName(pXmlIn->readElementText());
			}
			else if(pXmlIn->name() == "Author")
			{
				exercise.setAuthor(pXmlIn->readElementText());
			}
			else if(pXmlIn->name() == "Description")
			{
				exercise.setDescription(pXmlIn->readElementText());
			}
			else if(pXmlIn->name() == "Track")
			{
				QList<Track> tracks;
				tracks.append(readTrack(pXmlIn)); // noch nicht für mehrere Tracks angepasst
				exercise.setTracks(tracks);
			}
		}
		else if(pXmlIn->isEndElement())
		{
			if(pXmlIn->name() == "Exercise")
				break;
		}
	}

	return exercise;
}

void XMLSerialization::writeCollection(Collection &pCollection, QXmlStreamWriter *pXmlOut) const
{
	pXmlOut->writeStartElement("Collection");
	pXmlOut->writeTextElement("Name", pCollection.getName());
	pXmlOut->writeTextElement("Description", pCollection.getDescription());
	pXmlOut->writeTextElement("Author", pCollection.getAuthor());
	pXmlOut->writeStartElement("Exercises");

	foreach(QString item, pCollection.getFiles())
	{
		pXmlOut->writeTextElement("Exercise", item);
	}

	pXmlOut->writeEndElement();
	pXmlOut->writeEndElement();
}

Collection XMLSerialization::readCollection(QXmlStreamReader *pXmlIn)
{
	Collection collection;

	QStringList files;

	while(!pXmlIn->atEnd() && !pXmlIn->hasError())
	{
		pXmlIn->readNext();

		if(pXmlIn->isStartElement())
		{
			if(pXmlIn->name() == "Name")
			{
				collection.setName(pXmlIn->readElementText());
			}
			else if(pXmlIn->name() == "Description")
			{
				collection.setDescription(pXmlIn->readElementText());
			}
			else if(pXmlIn->name() == "Author")
			{
				collection.setAuthor(pXmlIn->readElementText());
			}
			else if(pXmlIn->name() == "Exercises")
			{
				while(!pXmlIn->atEnd() && !pXmlIn->hasError())
				{
					pXmlIn->readNext();

					if(pXmlIn->name() == "Exercise")
					{
						files.append(pXmlIn->readElementText());
					}
				}

				if(files.size() > 0)
				{
					collection.setFiles(files);
				}
			}
		}
		else if(pXmlIn->isEndElement())
		{
			if(pXmlIn->name() == "Collection")
				break;
		}
	}

	return collection;
}

Note XMLSerialization::readNote(QXmlStreamReader *pXmlIn)
{
	Note note;

	while(!pXmlIn->atEnd() && !pXmlIn->hasError())
	{
		pXmlIn->readNext();

		if(pXmlIn->isStartElement())
		{
			if(pXmlIn->name() == "Pitch")
			{
				note.setPitch(pXmlIn->readElementText().toInt());
			}
			else if(pXmlIn->name() == "Velocity")
			{
				note.setVelocity(pXmlIn->readElementText().toInt());
			}
		}
		else if(pXmlIn->isEndElement())
		{
			if(pXmlIn->name() == "Note")
				break;
		}
	}
	return note;
}

Instrument XMLSerialization::readInstrument(QXmlStreamReader *pXmlIn)
{
	Instrument instrument;

	while(!pXmlIn->atEnd() && !pXmlIn->hasError())
	{
		pXmlIn->readNext();

		if(pXmlIn->isStartElement())
		{
			if(pXmlIn->name() == "Name")
			{
				instrument.setName(pXmlIn->readElementText());
			}
			else if(pXmlIn->name() == "StringCount")
			{
				instrument.setStringCount(pXmlIn->readElementText().toInt());
			}
			else if(pXmlIn->name() == "FretCount")
			{
				instrument.setFretCount(pXmlIn->readElementText().toInt());
			}
			else if(pXmlIn->name() == "MidiProgram")
			{
				instrument.setMidiProgram(pXmlIn->readElementText().toInt());
			}
			else if(pXmlIn->name() == "isPercussion")
			{
				instrument.setPercussion(pXmlIn->readElementText().toInt() != 0);
			}
			else if(pXmlIn->name() == "StringTuningList")
			{
				QList<quint8> stringTunings;

				while(!pXmlIn->atEnd() && !pXmlIn->hasError())
				{
					pXmlIn->readNext();

					if(pXmlIn->isStartElement())
					{
						if(pXmlIn->name() == "StringTuning")
						{
							stringTunings.append(pXmlIn->readElementText().toInt());
						}
					}
					else if(pXmlIn->isEndElement())
					{
						if(pXmlIn->name() == "StringTuning")
							break;
					}
				}
				if(!stringTunings.isEmpty())
				{
					instrument.setStringTunings(stringTunings);
				}
			}
		}
		else if(pXmlIn->isEndElement())
		{
			if(pXmlIn->name() == "Instrument")
				break;
		}
	}
	return instrument;
}

void XMLSerialization::writeUserSettings(UserSettings pUserSettings, QXmlStreamWriter *pXmlOut)
{
	pXmlOut->writeStartElement("UserSettings");
	pXmlOut->writeTextElement("Name", pUserSettings.name);

	pXmlOut->writeStartElement("SpeedPointMappings");

	QMapIterator<QString, SpeedPoints> i(pUserSettings.speedPointsMapping);
	while (i.hasNext()) {
		i.next();
		pXmlOut->writeStartElement("SpeedPointMapping");
		pXmlOut->writeTextElement("ExerciseID", i.key());

		SpeedPoints p = i.value();

		pXmlOut->writeTextElement("StartSpeed", QString::number(p.startSpeed));
		pXmlOut->writeTextElement("MaxSpeed", QString::number(p.maxSpeed));
		pXmlOut->writeTextElement("EndSpeed", QString::number(p.endSpeed));
		pXmlOut->writeEndElement();
	}

	pXmlOut->writeEndElement();
	pXmlOut->writeEndElement();
}

UserSettings XMLSerialization::readUserSettings(QXmlStreamReader *pXmlIn)
{
	UserSettings userSettings;

	while(!pXmlIn->atEnd() && !pXmlIn->hasError())
	{
		pXmlIn->readNext();

		if(pXmlIn->isStartElement())
		{
			if(pXmlIn->name() == "Name")
			{
				userSettings.name = pXmlIn->readElementText();
			}
			else if(pXmlIn->name() == "SpeedPointMappings")
			{
				QMap<QString, SpeedPoints> speedPointsMapping;

				while(!pXmlIn->atEnd() && !pXmlIn->hasError())
				{
					pXmlIn->readNext();

					if(pXmlIn->isStartElement())
					{
						if(pXmlIn->name() == "SpeedPointMapping")
						{
							QString exerciseID;
							SpeedPoints speedPoints;

							while(!pXmlIn->atEnd() && !pXmlIn->hasError())
							{
								pXmlIn->readNext();

								if(pXmlIn->isStartElement())
								{
									if(pXmlIn->name() == "ExerciseID")
									{
										exerciseID = pXmlIn->readElementText();
									}
									else if(pXmlIn->name() == "StartSpeed")
									{
										speedPoints.startSpeed = pXmlIn->readElementText().toInt();
									}
									else if(pXmlIn->name() == "MaxSpeed")
									{
										speedPoints.maxSpeed = pXmlIn->readElementText().toInt();
									}
									else if(pXmlIn->name() == "EndSpeed")
									{
										speedPoints.endSpeed = pXmlIn->readElementText().toInt();
									}
								}
								else if(pXmlIn->isEndElement())
								{
									if(pXmlIn->name() == "SpeedPointMapping")
									{
										userSettings.speedPointsMapping.insert(exerciseID, speedPoints);
										break;
									}
								}
							}
						}
					}
					else if(pXmlIn->isEndElement())
					{
						if(pXmlIn->name() == "SpeedPointMappings")
							break;
					}
				}
			}
			else if(pXmlIn->isEndElement())
			{
				if(pXmlIn->name() == "UserSettings")
					break;
			}
		}
	}
	return userSettings;
}

Track XMLSerialization::readTrack(QXmlStreamReader *pXmlIn)
{
	Track track;

	if(findNextStartElement("NoteMap", "Track", pXmlIn))
	{
		QVector<QVector <Note> > noteMap;

		while(findNextStartElement("Voice", "NoteMap", pXmlIn))
		{
			QVector<Note> voice;

			while(findNextStartElement("Note", "Voice", pXmlIn))
			{
				Note note = readNote(pXmlIn);
				voice.append(note);
			}
			if(!voice.isEmpty())
			{
				noteMap.append(voice);
			}
		}
		if(!noteMap.isEmpty())
		{
			track.setNoteMap(noteMap);
		}
	}

	if(findNextStartElement("Instrument", pXmlIn))
	{
		Instrument instrument = readInstrument(pXmlIn);
		track.setInstrument(instrument);
	}
	return track;
}

// TODO: Den ganzen find-Kram nochmal überdenken und überarbeiten
bool XMLSerialization::findNextStartElement(QString pName, QXmlStreamReader *pXmlIn)
{
	return findNextStartElement(pName, "", pXmlIn);
}

bool XMLSerialization::findNextStartElement(QString pName, QString pStopElement, QXmlStreamReader *pXmlIn)
{
	while(!pXmlIn->atEnd() && !pXmlIn->hasError())
	{
		if(pXmlIn->isEndElement())
		{
			if(pXmlIn->name() == pStopElement)
			{
				return false;
			}
		}

		pXmlIn->readNext();

		if(pXmlIn->isStartElement())
		{
			if(pXmlIn->name() == pName)
			{
				//qDebug() << pXmlIn->lineNumber() << ": " << pName << " gefunden!";
				return true;
			}
		}
	}
	return false;
}

QXmlStreamWriter *XMLSerialization::writeStartSettings()
{
	currentFile = new QFile("settings.xml");

	if(currentFile->open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QXmlStreamWriter *xmlOut = new QXmlStreamWriter(currentFile);
		xmlOut->setAutoFormatting(true);

		xmlOut->writeStartDocument();
		return xmlOut;
	}
	return 0;
}

void XMLSerialization::writeEndSettings(QXmlStreamWriter *pStreamWriter)
{
	pStreamWriter->writeEndDocument();
	delete pStreamWriter;

	currentFile->close();
}


XMLSerialization* XMLSerialization::instance = 0;
