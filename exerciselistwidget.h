#ifndef EXERCISELISTWIDGET_H
#define EXERCISELISTWIDGET_H

#include <QListWidget>
#include <QMessageBox>
#include <exerciselistwidgetitem.h>

class ExerciseListWidget : public QListWidget {
    Q_OBJECT
  public:
    ExerciseListWidget(QWidget* parent);

    ~ExerciseListWidget();

  private slots:
    void itemClicked(QListWidgetItem* item);
};

#endif // EXERCISELISTWIDGET_H
