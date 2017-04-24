#include "exercise.h"

Exercise::Exercise(const QString& pName, const QString& pAuthor,
                   const QString& pDescription) : name(pName), author(pAuthor), description(pDescription)
{
    id = generateId();
}

Exercise::Exercise(const QString& pId, const QString& pName, const QString& pAuthor,
                   const QString& pDescription) : id(pId), name(pName), author(pAuthor), description(pDescription)
{
}


QString Exercise::generateId()
{
    const QString possibleCharacters("abcdef0123456789");
    const int randomStringLength = 16;

    QString randomString;

    for (int i = 0; i < randomStringLength; ++i) {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }

    return randomString;
}

QDataStream& operator <<(QDataStream& out, const Exercise& exercise)
{
    out << exercise.getId()
        << exercise.getName()
        << exercise.getAuthor()
        << exercise.getDescription()
        << exercise.getBeats()
        << exercise.getAccBeat()
        << exercise.getLength();

    return out;
}

QDataStream& operator >>(QDataStream& in, Exercise& exercise)
{
    QString id;
    QString name;
    QString author;
    QString description;

    quint16 beats;
    quint16 accBeat;
    quint16 length;

    in  >> id
        >> name
        >> author
        >> description
        >> beats
        >> accBeat
        >> length;

    exercise.setId(id);
    exercise.setName(name);
    exercise.setAuthor(author);
    exercise.setDescription(description);
    exercise.setBeats(beats);
    exercise.setAccBeat(accBeat);
    exercise.setLength(length);

    return in;
}
