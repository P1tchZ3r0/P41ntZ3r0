#include "PushButton.h"
#include <QSizePolicy>

PushButton::PushButton(QWidget* parent, QColor _color, const char _text[])
    : QPushButton(QString::fromUtf8(_text), parent),
      color{_color}
{
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setMinimumHeight(40);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(255, 255, 255, 0.05);"
        "  border: 1px solid rgba(255, 255, 255, 0.08);"
        "  color: #ebf1ff;"
        "  padding: 8px 16px;"
        "  border-radius: 12px;"
        "  font-weight: 600;"
        "  letter-spacing: 0.3px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(94, 200, 255, 0.22);"
        "  border: 1px solid rgba(94, 200, 255, 0.55);"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgba(94, 200, 255, 0.32);"
        "}"
        "QPushButton:checked {"
        "  background-color: rgba(94, 200, 255, 0.35);"
        "  border: 1px solid rgba(94, 200, 255, 0.75);"
        "}"
        "QPushButton:disabled {"
        "  color: rgba(235, 241, 255, 0.35);"
        "  border-color: rgba(255, 255, 255, 0.05);"
        "  background-color: rgba(255, 255, 255, 0.02);"
        "}"
    );
}

PushButton::~PushButton() = default;
