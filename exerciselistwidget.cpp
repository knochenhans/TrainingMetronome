#include "exerciselistwidget.h"

ExerciseListWidget::ExerciseListWidget(QWidget* parent) : QListWidget(parent)
{
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this,
            SLOT(itemClicked(QListWidgetItem*)));
}

ExerciseListWidget::~ExerciseListWidget()
{

}

void ExerciseListWidget::itemClicked(QListWidgetItem* item)
{
    //    QVariant data = item->data(Qt::UserRole);
    //    Exercise exercise = data.value<Exercise>();
    //    QMessageBox::information(this, "Test", exercise.getName());
}
