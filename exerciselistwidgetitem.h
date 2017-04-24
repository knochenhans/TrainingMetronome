#ifndef EXERCISELISTWIDGETITEM_H
#define EXERCISELISTWIDGETITEM_H

#include <QListWidgetItem>
#include <exercise.h>

class ExerciseListWidgetItem : public QListWidgetItem {
  public:
    ExerciseListWidgetItem(Exercise* pExercise);

    Exercise* getExercise() const;
    void setExercise(Exercise* value);

  private:
    Exercise* exercise;
};

#endif // EXERCISELISTWIDGETITEM_H
