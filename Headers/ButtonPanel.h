#ifndef BUTTONPANEL_H
#define BUTTONPANEL_H

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QMenu>
#include <QButtonGroup>
#include <QWidgetAction>
#include <QFrame>
#include <QColor>
#include <QVector>
#include "PushButton.h"

class ButtonPanel : public QWidget {
    Q_OBJECT

    PushButton* colorPickerBtn;
    PushButton* brushBtn;
    PushButton* eraserBtn;
    PushButton* rectangleBtn;
    PushButton* circleBtn;
    PushButton* lineBtn;
    PushButton* textBtn;
    PushButton* fillBtn;
    PushButton* clearBtn;
    PushButton* saveBtn;
    PushButton* openBtn;
    PushButton* brushSizeBtn;
    QSlider* brushSizeSlider;
    QButtonGroup* toolGroup;
    QMenu* brushSizeMenu;
    QWidgetAction* brushSizeAction;
    QFrame* colorPreview;
    QColor currentColor;

private slots:
    void onColorPickerClicked();
    void onBrushClicked();
    void onEraserClicked();
    void onRectangleClicked();
    void onCircleClicked();
    void onLineClicked();
    void onTextClicked();
    void onFillClicked();
    void onClearClicked();
    void onSaveClicked();
    void onOpenClicked();
    void onBrushSizeSliderChanged(int value);

public:
    ButtonPanel(QWidget* parent=nullptr);
    ~ButtonPanel();

    void setBrushSizeValue(int value);
    void setCurrentColor(const QColor& color);

signals:
    void colorPickerRequested();
    void brushModeSelected();
    void eraserModeSelected();
    void rectangleModeSelected();
    void circleModeSelected();
    void lineModeSelected();
    void textModeSelected();
    void fillModeSelected();
    void clearRequested();
    void saveRequested();
    void openRequested();
    void brushSizeChanged(int value);
};

#endif
