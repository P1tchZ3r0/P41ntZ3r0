#include "ButtonPanel.h"
#include <QColorDialog>
#include <QLabel>

ButtonPanel::ButtonPanel(QWidget* parent) : QWidget(parent)
{
	// Горизонтальный layout для тулбара сверху
	QHBoxLayout* h_layout = new QHBoxLayout(this);
	h_layout->setContentsMargins(5, 5, 5, 5);
	h_layout->setSpacing(5);
	
	this->setFixedHeight(50);
	this->setStyleSheet("background-color: #f0f0f0; border-bottom: 1px solid #ccc;");
	
	// Колорпикер
	colorPickerBtn = new PushButton(this, Qt::white, "Color");
	h_layout->addWidget(colorPickerBtn);
	connect(colorPickerBtn, &QPushButton::clicked, this, &ButtonPanel::onColorPickerClicked);
	
	// Разделитель
	QLabel* separator1 = new QLabel("|", this);
	separator1->setStyleSheet("color: #999;");
	h_layout->addWidget(separator1);
	
	// Инструменты рисования
	brushBtn = new PushButton(this, Qt::lightGray, "Brush");
	h_layout->addWidget(brushBtn);
	connect(brushBtn, &QPushButton::clicked, this, &ButtonPanel::onBrushClicked);
	
	rectangleBtn = new PushButton(this, Qt::lightGray, "Rect");
	h_layout->addWidget(rectangleBtn);
	connect(rectangleBtn, &QPushButton::clicked, this, &ButtonPanel::onRectangleClicked);
	
	circleBtn = new PushButton(this, Qt::lightGray, "Circle");
	h_layout->addWidget(circleBtn);
	connect(circleBtn, &QPushButton::clicked, this, &ButtonPanel::onCircleClicked);
	
	lineBtn = new PushButton(this, Qt::lightGray, "Line");
	h_layout->addWidget(lineBtn);
	connect(lineBtn, &QPushButton::clicked, this, &ButtonPanel::onLineClicked);
	
	// Разделитель
	QLabel* separator2 = new QLabel("|", this);
	separator2->setStyleSheet("color: #999;");
	h_layout->addWidget(separator2);
	
	// Действия
	clearBtn = new PushButton(this, Qt::lightGray, "Clear");
	h_layout->addWidget(clearBtn);
	connect(clearBtn, &QPushButton::clicked, this, &ButtonPanel::onClearClicked);
	
	saveBtn = new PushButton(this, Qt::lightGray, "Save");
	h_layout->addWidget(saveBtn);
	connect(saveBtn, &QPushButton::clicked, this, &ButtonPanel::onSaveClicked);
	
	openBtn = new PushButton(this, Qt::lightGray, "Open");
	h_layout->addWidget(openBtn);
	connect(openBtn, &QPushButton::clicked, this, &ButtonPanel::onOpenClicked);
	
	// Растягивающийся спейсер
	h_layout->addStretch();
}

void ButtonPanel::onColorPickerClicked()
{
	emit colorPickerRequested();
}

void ButtonPanel::onBrushClicked()
{
	emit brushModeSelected();
}

void ButtonPanel::onRectangleClicked()
{
	emit rectangleModeSelected();
}

void ButtonPanel::onCircleClicked()
{
	emit circleModeSelected();
}

void ButtonPanel::onLineClicked()
{
	emit lineModeSelected();
}

void ButtonPanel::onClearClicked()
{
	emit clearRequested();
}

void ButtonPanel::onSaveClicked()
{
	emit saveRequested();
}

void ButtonPanel::onOpenClicked()
{
	emit openRequested();
}

ButtonPanel::~ButtonPanel()
{ }
