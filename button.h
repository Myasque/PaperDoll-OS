#ifndef BUTTON_H
#define BUTTON_H

//此类继承了QPushButton,用于定义自己的button
#include <QWidget>
#include <QPushButton>
#include <QString>

class Button : public QPushButton
{
	Q_OBJECT;
public:
	Button(QWidget* parent, QString btn_text);//设置按钮样式

signals:

};

#endif // BUTTON_H

