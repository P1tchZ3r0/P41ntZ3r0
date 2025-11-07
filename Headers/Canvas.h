#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QColor>
#include <QResizeEvent>

enum class DrawingMode {
    Brush,
    Rectangle,
    Circle,
    Line
};

class Canvas : public QWidget {
	Q_OBJECT

private:
    QImage image;
    QColor currentColor;
    int brushSize;
    DrawingMode drawingMode;
    bool isDrawing;
    QPoint lastPoint;
    QPoint startPoint;
    QImage imageBeforeDrawing;

    QPoint widgetToImage(const QPoint& widgetPoint) const;
    void drawPoint(const QPoint& point);
    void drawLine(const QPoint& from, const QPoint& to);
    void drawRectangle(const QPoint& start, const QPoint& end);
    void drawCircle(const QPoint& start, const QPoint& end);
    void resizeImage(QImage* image, const QSize& newSize);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

public:
	Canvas(QWidget* parent=nullptr);
	~Canvas();

    void setColor(const QColor& color);
    QColor getCurrentColor() const { return currentColor; }
    void setBrushSize(int size);
    void setDrawingMode(DrawingMode mode);
    void clear();
    bool saveImage(const QString& fileName);
    bool loadImage(const QString& fileName);
    QImage getImage() const;
    void setImage(const QImage& newImage);

signals:
    void drawingFinished(const QImage& beforeImage, const QImage& afterImage);
};


#endif
