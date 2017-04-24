#include "exerciselistwidgetitem.h"

ExerciseListWidgetItem::ExerciseListWidgetItem(Exercise* pExercise)
{
    exercise = pExercise;
}

Exercise* ExerciseListWidgetItem::getExercise() const
{
    return exercise;
}

void ExerciseListWidgetItem::setExercise(Exercise* value)
{
    exercise = value;
}
