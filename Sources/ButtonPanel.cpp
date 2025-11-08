#include "ButtonPanel.h"
#include <QColorDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QSignalBlocker>

ButtonPanel::ButtonPanel(QWidget* parent)
    : QWidget(parent), currentColor(0x5ec8ff)
{
    setFixedHeight(74);
    setStyleSheet(
        "background-color: #151823;"
        "border-bottom: 1px solid rgba(255, 255, 255, 0.05);"
    );

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(18, 12, 18, 12);
    layout->setSpacing(14);

    colorPickerBtn = new PushButton(this, Qt::white, "Color");
    layout->addWidget(colorPickerBtn);
    connect(colorPickerBtn, &QPushButton::clicked, this, &ButtonPanel::onColorPickerClicked);

    colorPreview = new QFrame(this);
    colorPreview->setFixedSize(34, 34);
    colorPreview->setStyleSheet("border-radius: 12px; border: 2px solid rgba(255,255,255,0.18);");
    layout->addWidget(colorPreview);

    toolGroup = new QButtonGroup(this);
    toolGroup->setExclusive(true);

    auto makeTool = [this, layout](const QString& label, void (ButtonPanel::*slot)()) {
        PushButton* btn = new PushButton(this, Qt::lightGray, label.toUtf8().constData());
        btn->setCheckable(true);
        toolGroup->addButton(btn);
        layout->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, slot);
        return btn;
    };

    brushBtn = makeTool("Brush", &ButtonPanel::onBrushClicked);
    eraserBtn = makeTool("Eraser", &ButtonPanel::onEraserClicked);
    rectangleBtn = makeTool("Rect", &ButtonPanel::onRectangleClicked);
    circleBtn = makeTool("Circle", &ButtonPanel::onCircleClicked);
    lineBtn = makeTool("Line", &ButtonPanel::onLineClicked);
    textBtn = makeTool("Text", &ButtonPanel::onTextClicked);
    fillBtn = makeTool("Fill", &ButtonPanel::onFillClicked);
    brushBtn->setChecked(true);

    brushSizeBtn = new PushButton(this, Qt::lightGray, "Brush: 5px");
    brushSizeBtn->setCheckable(false);
    layout->addWidget(brushSizeBtn);

    brushSizeMenu = new QMenu(this);
    brushSizeMenu->setStyleSheet(
        "QMenu {"
        "  background-color: #202436;"
        "  border: 1px solid rgba(255, 255, 255, 0.08);"
        "  padding: 6px;"
        "}"
        "QMenu::item {"
        "  padding: 0;"
        "  margin: 0;"
        "}"
    );

    QWidget* sliderContainer = new QWidget(this);
    QVBoxLayout* sliderLayout = new QVBoxLayout(sliderContainer);
    sliderLayout->setContentsMargins(18, 12, 18, 12);
    sliderLayout->setSpacing(10);

    QLabel* sliderLabel = new QLabel("Brush thickness", sliderContainer);
    sliderLabel->setStyleSheet("color: #d7e4ff; font: 600 13px 'Segoe UI'; letter-spacing: 0.4px;");
    sliderLayout->addWidget(sliderLabel);

    brushSizeSlider = new QSlider(Qt::Horizontal, sliderContainer);
    brushSizeSlider->setRange(1, 60);
    brushSizeSlider->setValue(5);
    brushSizeSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "  background: rgba(255, 255, 255, 0.08);"
        "  height: 6px;"
        "  border-radius: 3px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: rgba(94, 200, 255, 0.65);"
        "  border-radius: 3px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: #5ec8ff;"
        "  width: 18px;"
        "  height: 18px;"
        "  margin: -7px 0;"
        "  border-radius: 9px;"
        "  border: 2px solid rgba(255, 255, 255, 0.7);"
        "}"
        "QSlider::handle:horizontal:hover {"
        "  background: #7ad3ff;"
        "}"
    );
    sliderLayout->addWidget(brushSizeSlider);

    brushSizeAction = new QWidgetAction(this);
    brushSizeAction->setDefaultWidget(sliderContainer);
    brushSizeMenu->addAction(brushSizeAction);

    connect(brushSizeSlider, &QSlider::valueChanged, this, &ButtonPanel::onBrushSizeSliderChanged);
    connect(brushSizeBtn, &QPushButton::clicked, this, [this]() {
        const QPoint globalPos = brushSizeBtn->mapToGlobal(QPoint(brushSizeBtn->width() / 2, brushSizeBtn->height()));
        QSize menuSize = brushSizeMenu->sizeHint();
        brushSizeMenu->popup(globalPos - QPoint(menuSize.width() / 2, 0));
    });

    layout->addStretch();

    clearBtn = new PushButton(this, Qt::lightGray, "Clear");
    layout->addWidget(clearBtn);
    connect(clearBtn, &QPushButton::clicked, this, &ButtonPanel::onClearClicked);

    saveBtn = new PushButton(this, Qt::lightGray, "Save");
    layout->addWidget(saveBtn);
    connect(saveBtn, &QPushButton::clicked, this, &ButtonPanel::onSaveClicked);

    openBtn = new PushButton(this, Qt::lightGray, "Open");
    layout->addWidget(openBtn);
    connect(openBtn, &QPushButton::clicked, this, &ButtonPanel::onOpenClicked);

    setCurrentColor(currentColor);
    setBrushSizeValue(5);
}

ButtonPanel::~ButtonPanel() = default;

void ButtonPanel::onColorPickerClicked()
{
    emit colorPickerRequested();
}

void ButtonPanel::onBrushClicked()
{
    emit brushModeSelected();
}

void ButtonPanel::onEraserClicked()
{
    emit eraserModeSelected();
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

void ButtonPanel::onTextClicked()
{
    emit textModeSelected();
}

void ButtonPanel::onFillClicked()
{
    emit fillModeSelected();
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

void ButtonPanel::onBrushSizeSliderChanged(int value)
{
    setBrushSizeValue(value);
    emit brushSizeChanged(value);
}

void ButtonPanel::setBrushSizeValue(int value)
{
    brushSizeBtn->setText(QStringLiteral("Brush: %1px").arg(value));
    if (brushSizeSlider && brushSizeSlider->value() != value) {
        QSignalBlocker blocker(brushSizeSlider);
        brushSizeSlider->setValue(value);
    }
}

void ButtonPanel::setCurrentColor(const QColor& color)
{
    currentColor = color;
    colorPreview->setStyleSheet(QStringLiteral(
        "border-radius: 12px; border: 2px solid rgba(255,255,255,0.35); background-color: %1;"
    ).arg(color.name()));
}
