#ifndef BUTTONPANEL_H
#define BUTTONPANEL_H

#include <iostream>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "PushButton.h"

class ButtonPanel : public QWidget {
	Q_OBJECT

	PushButton* colorPickerBtn;
	PushButton* brushBtn;
	PushButton* rectangleBtn;
	PushButton* circleBtn;
	PushButton* lineBtn;
	PushButton* clearBtn;
	PushButton* saveBtn;
	PushButton* openBtn;
	
private slots:
	void onColorPickerClicked();
	void onBrushClicked();
	void onRectangleClicked();
	void onCircleClicked();
	void onLineClicked();
	void onClearClicked();
	void onSaveClicked();
	void onOpenClicked();
	
public:
	ButtonPanel(QWidget* parent=nullptr);	
	~ButtonPanel();

signals:
	void colorPickerRequested();
	void brushModeSelected();
	void rectangleModeSelected();
	void circleModeSelected();
	void lineModeSelected();
	void clearRequested();
	void saveRequested();
	void openRequested();
};


#endif
