#ifndef DIALOG_H
#define DIALOG_H

#include <QThread>
#include <QDialog>
#include <QDebug>
#include <QPushButton>
#include <QGridLayout>
#include <QListWidget>
#include <QDir>

#include <exerciselistwidget.h>
#include <exerciselistwidgetitem.h>
#include <exerciseeditor.h>
#include <exercise.h>
#include <midi.h>
#include <xmlserialization.h>
#include <RtMidi.h>

class Dialog : public QDialog {
    Q_OBJECT

  private slots:
    void start();
    void addExercise();
    void deleteExercise();
    void editExercise();
    void nextExercise();

  public:
    Dialog(QWidget* parent = 0);
    ~Dialog();

  private:
    RtMidiOut* midiOut;
    Midi* midi;

    QPushButton* pbStart;
    QPushButton* pbConfig;
    QPushButton* pbQuit;

    QPushButton* pbAddExercise;
    QPushButton* pbDeleteExercise;
    QPushButton* pbEditExercise;

    ExerciseListWidget* lwExerciseList;

    int currentExercise;

    QList<UserSettings> userSettingsList;
};

#endif // DIALOG_H
