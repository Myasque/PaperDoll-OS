#include "button.h"

Button::Button(QWidget* parent, QString btn_text)
{
	//设计button的样式
	this->setParent(parent);
	this->setText(btn_text);
	this->setStyleSheet("QPushButton{border: 0px; border-radius: 10px;}\
                        QPushButton{background-color:#FFC1C1;color:black;}\
                        QPushButton:hover{background-color:#EEDFCC;}");
	this->resize(150, 50);
	//设置字体
	QFont font;
	font.setFamily("幼圆");
	font.setPointSize(10);
	this->setFont(font);

	//设置光标
	QCursor cursor;
	cursor.setShape(Qt::PointingHandCursor);

	this->setCursor(cursor);

}
