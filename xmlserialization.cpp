#include "xmlserialization.h"

XMLSerialization* XMLSerialization::getInstance()
{
    static QMutex mutex;

    if (!instance) {
        mutex.lock();

        if (!instance) {
            instance = new XMLSerialization;
        }

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

QXmlStreamWriter* XMLSerialization::writeStartDocument(QString pFilename)
{
    if (!pFilename.isEmpty()) {
        currentFile = new QFile(pFilename);

        if (currentFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
            QXmlStreamWriter* xmlOut = new QXmlStreamWriter(currentFile);
            xmlOut->setAutoFormatting(true);

            xmlOut->writeStartDocument();
            return xmlOut;
        }
    }

    return 0;
}

void XMLSerialization::writeEndDocument(QXmlStreamWriter* pStreamWriter)
{
    pStreamWriter->writeEndDocument();
    delete pStreamWriter;

    currentFile->close();
}

QXmlStreamReader* XMLSerialization::readStartDocument(QString pFilename)
{
    if (!pFilename.isEmpty()) {
        currentFile = new QFile(pFilename);

        if (currentFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
            QXmlStreamReader* pXmlIn = new QXmlStreamReader(currentFile);

            while (!pXmlIn->atEnd() && !pXmlIn->hasError()) {
                pXmlIn->readNext();

                if (pXmlIn->isStartDocument()) {
                    // Document start found, leave the remaining work for the calling function

                    return pXmlIn;
                }
            }

            // No document start found: error
        }
    }

    return 0;
}

void XMLSerialization::readEndDocument(QXmlStreamReader* pXmlIn)
{
    pXmlIn->clear();
    delete pXmlIn;

    currentFile->close();
}

void XMLSerialization::writeExercise(Exercise& pExercise, QXmlStreamWriter* pXmlOut) const
{
    pXmlOut->writeStartElement("Exercise");
    pXmlOut->writeTextElement("ID", pExercise.getId());
    pXmlOut->writeTextElement("Name", pExercise.getName());
    pXmlOut->writeTextElement("Author", pExercise.getAuthor());
    pXmlOut->writeTextElement("Description", pExercise.getDescription());
    pXmlOut->writeTextElement("Beats", QString::number(pExercise.getBeats()));
    pXmlOut->writeTextElement("AccBeat", QString::number(pExercise.getAccBeat()));
    pXmlOut->writeTextElement("Length", QString::number(pExercise.getLength()));

    pXmlOut->writeEndElement();
}


Exercise XMLSerialization::readExercise(QXmlStreamReader* pXmlIn)
{
    Exercise exercise;

    while (!pXmlIn->atEnd() && !pXmlIn->hasError()) {
        pXmlIn->readNext();

        if (pXmlIn->isStartElement()) {
            if (pXmlIn->name() == "ID") {
                exercise.setId(pXmlIn->readElementText());
            } else if (pXmlIn->name() == "Name") {
                exercise.setName(pXmlIn->readElementText());
            } else if (pXmlIn->name() == "Author") {
                exercise.setAuthor(pXmlIn->readElementText());
            } else if (pXmlIn->name() == "Description") {
                exercise.setDescription(pXmlIn->readElementText());
            } else if (pXmlIn->name() == "Beats") {
                exercise.setBeats(pXmlIn->readElementText().toUInt());
            } else if (pXmlIn->name() == "AccBeat") {
                exercise.setAccBeat(pXmlIn->readElementText().toUInt());
            } else if (pXmlIn->name() == "Length") {
                exercise.setLength(pXmlIn->readElementText().toUInt());
            }
        } else if (pXmlIn->isEndElement()) {
            if (pXmlIn->name() == "Exercise") {
                break;
            }
        }
    }

    return exercise;
}

void XMLSerialization::writeUserSettings(QList<UserSettings> pUserSettings)
{
    QXmlStreamWriter* XmlOut = writeStartDocument("settings.xus");

    XmlOut->writeStartElement("UserSettings");

    for (int i = 0; i < pUserSettings.size(); i++) {
        UserSettings userSetting = pUserSettings[i];

        XmlOut->writeStartElement("UserSetting");

        XmlOut->writeTextElement("Username", userSetting.username);

        XmlOut->writeStartElement("SpeedMaps");

        QMapIterator<QString, quint16> j(userSetting.exerciseSpeedMap);

        while (j.hasNext()) {
            j.next();
            XmlOut->writeStartElement("SpeedMap");
            XmlOut->writeTextElement("ExerciseID", j.key());

            XmlOut->writeTextElement("StartSpeed", QString::number(j.value()));
            XmlOut->writeEndElement();
        }

        XmlOut->writeEndElement();
        XmlOut->writeEndElement();
    }

    XmlOut->writeEndElement();

    writeEndDocument(XmlOut);
}

QList<UserSettings> XMLSerialization::readUserSettings()
{
    UserSettings userSettings;

    QList<UserSettings> userSettingsList;

    QXmlStreamReader* xmlIn = readStartDocument("settings.xus");

    while (!xmlIn->atEnd() && !xmlIn->hasError()) {
        xmlIn->readNextStartElement();

        if (xmlIn->name() == "UserSettings") {

            while (!xmlIn->atEnd() && !xmlIn->hasError()) {
                xmlIn->readNextStartElement();

                if (xmlIn->name() == "UserSetting") {
                    xmlIn->readNextStartElement();

                    if (xmlIn->name() == "Username") {
                        userSettings.username = xmlIn->readElementText();

                        QMap<QString, quint16> speedMap;

                        xmlIn->readNextStartElement();

                        if (xmlIn->name() == "SpeedMaps") {
                            while (!xmlIn->atEnd() && !xmlIn->hasError()) {

                                xmlIn->readNextStartElement();

                                if (xmlIn->name() == "SpeedMap") {
                                    QString exerciseId;
                                    quint16 startSpeed;

                                    xmlIn->readNextStartElement();

                                    if (xmlIn->name() == "ExerciseID") {
                                        exerciseId = xmlIn->readElementText();
                                    }

                                    xmlIn->readNextStartElement();

                                    if (xmlIn->name() == "StartSpeed") {
                                        startSpeed = xmlIn->readElementText().toUInt();
                                    }

                                    speedMap[exerciseId] = startSpeed;

                                    xmlIn->readNextStartElement();
                                }
                            }
                        }

                        userSettings.exerciseSpeedMap = speedMap;
                    }

                    userSettingsList.append(userSettings);
                }
            }
        }
    }

    return userSettingsList;
}

QXmlStreamWriter* XMLSerialization::writeStartSettings()
{
    currentFile = new QFile("settings.xml");

    if (currentFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        QXmlStreamWriter* xmlOut = new QXmlStreamWriter(currentFile);
        xmlOut->setAutoFormatting(true);

        xmlOut->writeStartDocument();
        return xmlOut;
    }

    return 0;
}

void XMLSerialization::writeEndSettings(QXmlStreamWriter* pStreamWriter)
{
    pStreamWriter->writeEndDocument();
    delete pStreamWriter;

    currentFile->close();
}


XMLSerialization* XMLSerialization::instance = 0;
