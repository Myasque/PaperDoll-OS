#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PaperDoll.h"

class PaperDoll : public QMainWindow
{
	Q_OBJECT

public:
	PaperDoll(QWidget *parent = Q_NULLPTR);

private:
	Ui::PaperDollClass ui;
};
