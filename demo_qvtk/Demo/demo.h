#ifndef Demo1QT_H
#define Demo1QT_H

#include <QMainWindow>
#include <QObject>

#include "ui_Demo1QT.h"

class vtkEventQtSlotConnect;

class RenderWindowsQt : public QMainWindow, private Ui::RenderWindowsQt
{
  Q_OBJECT
public:

  // Constructor/Destructor
  RenderWindowsQt(); 
  ~RenderWindowsQt() {}
};

#endif
