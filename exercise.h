#ifndef EXERCISE_H
#define EXERCISE_H

#include <QString>
#include <QDataStream>
#include <QCryptographicHash>

class Exercise {
  public:
    Exercise() {}
    Exercise(const QString& pName, const QString& pAuthor,
             const QString& pDescription);
    Exercise(const QString& pId, const QString& pName, const QString& pAuthor,
             const QString& pDescription);

    const QString getId() const {
        return id;
    }
    void setId(const QString value) {
        id = value;
    }

    const QString getName() const {
        return name;
    }
    void setName(const QString value) {
        name = value;
    }

    const QString getAuthor() const {
        return author;
    }
    void setAuthor(const QString value) {
        author = value;
    }

    const QString getDescription() const {
        return description;
    }
    void setDescription(const QString value) {
        description = value;
    }

    const quint16 getBeats() const {
        return beats;
    }
    void setBeats(const quint16& value) {
        beats = value;
    }

    const quint16 getAccBeat() const {
        return accBeat;
    }
    void setAccBeat(const quint16& value) {
        accBeat = value;
    }

    const quint16 getLength() const {
        return length;
    }
    void setLength(const quint16& value) {
        length = value;
    }

  private:
    QString id;
    QString name;
    QString author;
    QString description;

    quint16 beats;
    quint16 accBeat;
    quint16 length;

    friend QDataStream& operator <<(QDataStream& out, const Exercise& exercise);
    friend QDataStream& operator >>(QDataStream& in, Exercise& exercise);

    QString generateId();
};

Q_DECLARE_METATYPE(Exercise)

#endif // EXERCISE_H
