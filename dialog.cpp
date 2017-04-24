#include "dialog.h"

Dialog::Dialog(QWidget* parent) : QDialog(parent)
{
    currentExercise = 0;

    pbStart = new QPushButton("Start", this);
    pbQuit = new QPushButton("Ende", this);
    pbConfig = new QPushButton("Konfig.", this);

    pbAddExercise = new QPushButton("Hinzuf.", this);
    pbDeleteExercise = new QPushButton("Entf.", this);
    pbEditExercise = new QPushButton("Bearb.", this);

    QGridLayout* gridLayout = new QGridLayout(this);

    setLayout(gridLayout);

    lwExerciseList = new ExerciseListWidget(this);

    QVector<Exercise> exerciseList;

    // Load all exercise files

    Exercise exercise;

    QStringList filter("*.xex");
    QStringList files = QDir::current().entryList(filter);

    for (int i = 0; i < files.size(); i++) {
        QXmlStreamReader* xmlIn = XMLSerialization::getInstance()->readStartDocument(files[i]);
        exercise = XMLSerialization::getInstance()->readExercise(xmlIn);
        XMLSerialization::getInstance()->readEndDocument(xmlIn);

        exerciseList.append(exercise);
    }

    // Add exercises to listWidget, attaching their data as QVariants

    QString username = qgetenv("USER"); // get the user name in Linux

    if (username.isEmpty()) {
        username = qgetenv("USERNAME"); // get the name in Windows
    }

    userSettingsList = XMLSerialization::getInstance()->readUserSettings();

    for (int i = 0; i < userSettingsList.size(); i++) {
        UserSettings userSettings = userSettingsList[i];

        if (userSettings.username == username) {
            for (int i = 0; i < exerciseList.size(); i++) {
                exercise = exerciseList[i];

                QVariant v;
                v.setValue(exercise);

                QListWidgetItem* item = new QListWidgetItem();
                item->setText(exercise.getName() + " (" + QString::number(
                                  userSettings.exerciseSpeedMap[exercise.getId()]) + ")");
                item->setData(Qt::UserRole, v);
                lwExerciseList->addItem(item);
            }

            break;
        }
    }

    gridLayout->addWidget(lwExerciseList, 0, 0, 3, 3);
    gridLayout->addWidget(pbStart, 0, 3);
    gridLayout->addWidget(pbConfig, 1, 3);
    gridLayout->addWidget(pbQuit, 2, 3);

    gridLayout->addWidget(pbAddExercise, 3, 0);
    gridLayout->addWidget(pbDeleteExercise, 3, 1);
    gridLayout->addWidget(pbEditExercise, 3, 2);

    connect(pbQuit, SIGNAL(clicked()), this, SLOT(close()));
    connect(pbStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(pbEditExercise, SIGNAL(clicked()), this, SLOT(editExercise()));

    midi = new Midi();
    connect(midi, SIGNAL(finished()), this, SLOT(nextExercise()));
}

Dialog::~Dialog()
{
    XMLSerialization::getInstance()->writeUserSettings(userSettingsList);
}

void Dialog::start()
{
    if (midi->initialize()) {
        QListWidgetItem* item = lwExerciseList->item(currentExercise);

        item->setSelected(true);

        pbStart->setText("Pause");

        QVariant data = item->data(Qt::UserRole);
        Exercise exercise = data.value<Exercise>();

        if (!midi->isRunning()) {
            midi->play(100, exercise.getBeats(), exercise.getAccBeat(), exercise.getLength());
        }
    }

}

void Dialog::addExercise()
{
    ExerciseEditor exerciseEditor;
    exerciseEditor.exec();
}

void Dialog::deleteExercise()
{

}

void Dialog::editExercise()
{
    ExerciseEditor exerciseEditor;
    exerciseEditor.exec();
}

void Dialog::nextExercise()
{
    pbStart->setText("Start");

    if (currentExercise < lwExerciseList->count()) {
        currentExercise++;

        start();
    }
}
