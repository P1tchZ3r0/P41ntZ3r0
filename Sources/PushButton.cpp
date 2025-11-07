#include "PushButton.h"

PushButton::PushButton(QWidget* parent, QColor _color, const char _text[]) : QPushButton(parent), color{_color}
{
	QPalette pal {this->palette()};
	pal.setColor(QPalette::Button, _color);
	this->setPalette(pal);
	this->setText(_text);
}

PushButton::~PushButton()
{ }
