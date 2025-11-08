#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QColor>
#include <QResizeEvent>
#include <QByteArray>
#include <QFont>
#include <QString>
#include <QQueue>
#include <QPoint>
#include <QPointF>

class QKeyEvent;


enum class DrawingMode {
    Brush,
    Eraser,
    Rectangle,
    Circle,
    Line,
    Text,
    Fill
};

class Canvas : public QWidget {
    Q_OBJECT

private:
    QImage image;
    QColor currentColor;
    QFont currentFont;
    int brushSize;
    DrawingMode drawingMode;
    bool isDrawing;
    qreal zoomFactor;
    QPointF panOffset;
    QPoint lastPoint;
    QPoint startPoint;
    QImage imageBeforeDrawing;

    QPoint widgetToImage(const QPoint& widgetPoint) const;
    void drawPoint(const QPoint& point);
    void drawLine(const QPoint& from, const QPoint& to);
    void drawRectangle(const QPoint& start, const QPoint& end);
    void drawCircle(const QPoint& start, const QPoint& end);
    void floodFill(const QPoint& startPos, QRgb targetColor, QRgb replacementColor);
    bool isValidPoint(int x, int y) const;
    void resizeImageIfNeeded(const QSize& newSize);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

public:
    Canvas(QWidget* parent=nullptr);
    ~Canvas();

    void setColor(const QColor& color);
    QColor getCurrentColor() const { return currentColor; }
    void setBrushSize(int size);
    int getBrushSize() const { return brushSize; }
    void setDrawingMode(DrawingMode mode);
    void setFont(const QFont& font);
    QFont getFont() const { return currentFont; }
    void addText(const QPoint& imagePos, const QString& text);
    void cancelPendingOperation();
    void clear();
    bool saveImage(const QString& fileName, const QByteArray& format = QByteArray());
    bool loadImage(const QString& fileName);
    QImage getImage() const;
    void setImage(const QImage& newImage);

signals:
    void drawingFinished(const QImage& beforeImage, const QImage& afterImage);
    void textEntryRequested(const QPoint& imagePos);
};

#endif
