#ifndef XMLSERIALIZATION_H
#define XMLSERIALIZATION_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMutex>
#include <QDebug>

#include "exercise.h"

struct UserSettings {
    QString username;

    // Exercise (id) und its speed;
    QMap<QString, quint16> exerciseSpeedMap;
};

class XMLSerialization {
  public:
    static XMLSerialization* getInstance();
    static void drop();

    QXmlStreamWriter* writeStartDocument(QString pFilename);
    void writeEndDocument(QXmlStreamWriter* pStreamWriter);

    QXmlStreamReader* readStartDocument(QString pFilename);
    void readEndDocument(QXmlStreamReader* pStreamReader);

    void writeExercise(Exercise& pExercise, QXmlStreamWriter* pXmlOut) const;
    Exercise readExercise(QXmlStreamReader* pXmlIn);

    QXmlStreamWriter* writeStartSettings();
    void writeEndSettings(QXmlStreamWriter* pStreamWriter);

    void writeUserSettings(QList<UserSettings> pUserSettings);
    QList<UserSettings> readUserSettings();

  private:
    XMLSerialization() {}
    XMLSerialization(const XMLSerialization&);
    XMLSerialization& operator=(const XMLSerialization&);

    static XMLSerialization* instance;

    QFile* currentFile;
};

#endif // XMLSERIALIZATION_H
