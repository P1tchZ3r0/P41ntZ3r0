#include "Canvas.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <cmath>
#include <algorithm>

Canvas::Canvas(QWidget* parent) : QWidget(parent),
    currentColor(Qt::black),
    currentFont(QFont("Sans Serif", 24, QFont::Bold)),
    brushSize(5),
    drawingMode(DrawingMode::Brush),
    isDrawing(false),
    zoomFactor(1.0),
    panOffset(0.0, 0.0)
{
    setMinimumSize(400, 300);
    setStyleSheet("background-color: white;");
    setFocusPolicy(Qt::StrongFocus);

    const QSize defaultSize(1024, 768);
    image = QImage(defaultSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);
}

Canvas::~Canvas()
{ }

void Canvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillRect(rect(), QColor(21, 24, 35));
    if (image.isNull()) {
        return;
    }

    QSizeF scaledSize = QSizeF(image.width(), image.height()) * zoomFactor;
    QPointF baseTopLeft((width() - scaledSize.width()) / 2.0,
                        (height() - scaledSize.height()) / 2.0);
    QPointF topLeft = baseTopLeft - panOffset * zoomFactor;
    QRectF targetRect(topLeft, scaledSize);

    painter.drawImage(targetRect, image);

    QPen borderPen(QColor(94, 200, 255, 140));
    borderPen.setWidthF(2.0);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(targetRect.adjusted(0.5, 0.5, -0.5, -0.5));
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint imagePos = widgetToImage(event->pos());
        if (!isValidPoint(imagePos.x(), imagePos.y())) {
            return;
        }

        if (drawingMode == DrawingMode::Text) {
            imageBeforeDrawing = image.copy();
            emit textEntryRequested(imagePos);
            return;
        }

        if (drawingMode == DrawingMode::Fill) {
            imageBeforeDrawing = image.copy();
            QRgb targetColor = image.pixel(imagePos);
            QRgb replacementColor = qPremultiply(currentColor.rgba());
            if (targetColor != replacementColor) {
                floodFill(imagePos, targetColor, replacementColor);
                update();
                emit drawingFinished(imageBeforeDrawing, image);
            }
            imageBeforeDrawing = QImage();
            return;
        }

        isDrawing = true;
        lastPoint = event->pos();
        startPoint = event->pos();

        imageBeforeDrawing = image.copy();

        if (drawingMode == DrawingMode::Brush || drawingMode == DrawingMode::Eraser) {
            drawPoint(event->pos());
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (isDrawing && event->buttons() & Qt::LeftButton) {
        if (drawingMode == DrawingMode::Brush || drawingMode == DrawingMode::Eraser) {
            drawLine(lastPoint, event->pos());
            lastPoint = event->pos();
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (drawingMode == DrawingMode::Text) {
        return;
    }

    if (event->button() == Qt::LeftButton && isDrawing) {
        if (drawingMode == DrawingMode::Rectangle) {
            drawRectangle(startPoint, event->pos());
        } else if (drawingMode == DrawingMode::Circle) {
            drawCircle(startPoint, event->pos());
        } else if (drawingMode == DrawingMode::Line) {
            drawLine(startPoint, event->pos());
        }
        isDrawing = false;

        if (!imageBeforeDrawing.isNull()) {
            emit drawingFinished(imageBeforeDrawing, image);
            imageBeforeDrawing = QImage();
        }
    }
}

QPoint Canvas::widgetToImage(const QPoint& widgetPoint) const
{
    if (image.isNull() || width() == 0 || height() == 0) {
        return QPoint(-1, -1);
    }

    QSizeF scaledSize = QSizeF(image.width(), image.height()) * zoomFactor;
    QPointF baseTopLeft((width() - scaledSize.width()) / 2.0,
                        (height() - scaledSize.height()) / 2.0);
    QPointF topLeft = baseTopLeft - panOffset * zoomFactor;
    QPointF relative = QPointF(widgetPoint) - topLeft;

    if (relative.x() < 0.0 || relative.y() < 0.0 ||
        relative.x() >= scaledSize.width() || relative.y() >= scaledSize.height()) {
        return QPoint(-1, -1);
    }

    qreal x = relative.x() / zoomFactor;
    qreal y = relative.y() / zoomFactor;
    return QPoint(static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y)));
}

void Canvas::drawPoint(const QPoint& point)
{
    QPoint imagePoint = widgetToImage(point);
    if (!isValidPoint(imagePoint.x(), imagePoint.y())) {
        return;
    }

    QPainter painter;
    if (!painter.begin(&image)) {
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);
    if (drawingMode == DrawingMode::Eraser) {
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setPen(QPen(Qt::white, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    } else {
        painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }
    painter.drawPoint(imagePoint);
    painter.end();
    update();
}

void Canvas::drawLine(const QPoint& from, const QPoint& to)
{
    QPoint imageFrom = widgetToImage(from);
    QPoint imageTo = widgetToImage(to);
    if (!isValidPoint(imageFrom.x(), imageFrom.y()) || !isValidPoint(imageTo.x(), imageTo.y())) {
        return;
    }

    QPainter painter;
    if (!painter.begin(&image)) {
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);
    if (drawingMode == DrawingMode::Eraser) {
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setPen(QPen(Qt::white, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    } else {
        painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }
    painter.drawLine(imageFrom, imageTo);
    painter.end();
    update();
}

void Canvas::drawRectangle(const QPoint& start, const QPoint& end)
{
    QPoint imageStart = widgetToImage(start);
    QPoint imageEnd = widgetToImage(end);
    if (!isValidPoint(imageStart.x(), imageStart.y()) || !isValidPoint(imageEnd.x(), imageEnd.y())) {
        return;
    }

    QPainter painter;
    if (!painter.begin(&image)) {
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    QRect rect = QRect(imageStart, imageEnd).normalized();
    painter.drawRect(rect);
    painter.end();
    update();
}

void Canvas::drawCircle(const QPoint& start, const QPoint& end)
{
    QPoint imageStart = widgetToImage(start);
    QPoint imageEnd = widgetToImage(end);
    if (!isValidPoint(imageStart.x(), imageStart.y()) || !isValidPoint(imageEnd.x(), imageEnd.y())) {
        return;
    }

    QPainter painter;
    if (!painter.begin(&image)) {
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    QRect rect = QRect(imageStart, imageEnd).normalized();
    painter.drawEllipse(rect);
    painter.end();
    update();
}

void Canvas::setColor(const QColor& color)
{
    currentColor = color;
}

void Canvas::setBrushSize(int size)
{
    if (size < 1) {
        size = 1;
    } else if (size > 100) {
        size = 100;
    }
    brushSize = size;
}

void Canvas::setDrawingMode(DrawingMode mode)
{
    drawingMode = mode;
}

void Canvas::setFont(const QFont& font)
{
    currentFont = font;
}

void Canvas::addText(const QPoint& imagePos, const QString& text)
{
    if (text.isEmpty()) {
        return;
    }

    if (image.isNull()) {
        image = QImage(1024, 768, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::white);
    }

    if (imageBeforeDrawing.isNull()) {
        imageBeforeDrawing = image.copy();
    }

    QPainter painter;
    if (!painter.begin(&image)) {
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(currentColor);
    painter.setFont(currentFont);

    QFontMetrics metrics(currentFont);
    QPoint drawPos = imagePos;
    drawPos.setY(drawPos.y() + metrics.ascent());
    painter.drawText(drawPos, text);
    painter.end();

    update();

    emit drawingFinished(imageBeforeDrawing, image);
    imageBeforeDrawing = QImage();
}

void Canvas::cancelPendingOperation()
{
    imageBeforeDrawing = QImage();
}

void Canvas::clear()
{
    image.fill(Qt::white);
    zoomFactor = 1.0;
    panOffset = QPointF(0.0, 0.0);
    update();
}

bool Canvas::saveImage(const QString& fileName, const QByteArray& format)
{
    if (image.isNull()) {
        return false;
    }

    if (format.isEmpty()) {
        return image.save(fileName);
    }
    return image.save(fileName, format.constData());
}

bool Canvas::loadImage(const QString& fileName)
{
    QImage loadedImage(fileName);
    if (!loadedImage.isNull()) {
        const int MAX_SIZE = 2048;
        if (loadedImage.width() > MAX_SIZE || loadedImage.height() > MAX_SIZE) {
            loadedImage = loadedImage.scaled(MAX_SIZE, MAX_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        int canvasWidth = qMax(width(), 400);
        int canvasHeight = qMax(height(), 300);

        QSize targetSize = loadedImage.size().scaled(canvasWidth, canvasHeight, Qt::KeepAspectRatio);
        loadedImage = loadedImage.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        image = QImage(canvasWidth, canvasHeight, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::white);
        QPainter painter(&image);
        int x = (canvasWidth - loadedImage.width()) / 2;
        int y = (canvasHeight - loadedImage.height()) / 2;
        painter.drawImage(x, y, loadedImage);

        zoomFactor = 1.0;
        panOffset = QPointF(0.0, 0.0);

        update();
        return true;
    }
    return false;
}

QImage Canvas::getImage() const
{
    return image;
}

void Canvas::setImage(const QImage& newImage)
{
    image = newImage;
    zoomFactor = 1.0;
    panOffset = QPointF(0.0, 0.0);
    update();
}

void Canvas::wheelEvent(QWheelEvent* event)
{
    if (!(event->modifiers() & Qt::ControlModifier)) {
        QWidget::wheelEvent(event);
        return;
    }

    if (image.isNull()) {
        return;
    }

    QPointF cursorPos = event->position();

    QSizeF scaledSize = QSizeF(image.width(), image.height()) * zoomFactor;
    QPointF baseTopLeft((width() - scaledSize.width()) / 2.0,
                        (height() - scaledSize.height()) / 2.0);
    QPointF topLeft = baseTopLeft - panOffset * zoomFactor;
    QPointF relative = cursorPos - topLeft;
    QPointF imagePoint(relative.x() / zoomFactor, relative.y() / zoomFactor);

    const qreal steps = event->angleDelta().y() / 120.0;
    if (!qFuzzyIsNull(steps)) {
        const qreal factor = std::pow(1.15, steps);
        qreal newZoom = std::clamp(zoomFactor * factor, 0.25, 8.0);
        if (!qFuzzyCompare(newZoom, zoomFactor)) {
            zoomFactor = newZoom;

            QSizeF newScaledSize = QSizeF(image.width(), image.height()) * zoomFactor;
            QPointF newBaseTopLeft((width() - newScaledSize.width()) / 2.0,
                                   (height() - newScaledSize.height()) / 2.0);

            QPointF newTopLeft = cursorPos - imagePoint * zoomFactor;
            QPointF panPixels = newBaseTopLeft - newTopLeft;
            panOffset = panPixels / zoomFactor;
        }
    }

    update();
    event->accept();
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
    const qreal step = 40.0 / zoomFactor;
    bool handled = false;

    switch (event->key()) {
        case Qt::Key_Left:
            panOffset.rx() -= step;
            handled = true;
            break;
        case Qt::Key_Right:
            panOffset.rx() += step;
            handled = true;
            break;
        case Qt::Key_Up:
            panOffset.ry() -= step;
            handled = true;
            break;
        case Qt::Key_Down:
            panOffset.ry() += step;
            handled = true;
            break;
        default:
            break;
    }

    if (handled) {
        update();
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void Canvas::resizeEvent(QResizeEvent* event)
{
    const int MAX_WIDTH = 2048;
    const int MAX_HEIGHT = 2048;

    int newWidth = qMin(qMax(width(), image.width()), MAX_WIDTH);
    int newHeight = qMin(qMax(height(), image.height()), MAX_HEIGHT);

    QSize newSize(newWidth, newHeight);
    resizeImageIfNeeded(newSize);
    QWidget::resizeEvent(event);
}

void Canvas::resizeImageIfNeeded(const QSize& newSize)
{
    if (image.size() == newSize) {
        return;
    }

    QImage newImage(newSize, QImage::Format_ARGB32_Premultiplied);
    newImage.fill(Qt::white);

    QPainter painter;
    if (painter.begin(&newImage)) {
        painter.drawImage(QPoint(0, 0), image);
        painter.end();
    }
    image = newImage;
}

bool Canvas::isValidPoint(int x, int y) const
{
    return x >= 0 && y >= 0 && x < image.width() && y < image.height();
}

void Canvas::floodFill(const QPoint& startPos, QRgb targetColor, QRgb replacementColor)
{
    if (targetColor == replacementColor) {
        return;
    }

    QQueue<QPoint> queue;
    queue.enqueue(startPos);

    while (!queue.isEmpty()) {
        QPoint p = queue.dequeue();
        int x = p.x();
        int y = p.y();

        if (!isValidPoint(x, y)) {
            continue;
        }

        if (image.pixel(x, y) != targetColor) {
            continue;
        }

        image.setPixel(x, y, replacementColor);

        queue.enqueue(QPoint(x + 1, y));
        queue.enqueue(QPoint(x - 1, y));
        queue.enqueue(QPoint(x, y + 1));
        queue.enqueue(QPoint(x, y - 1));
    }
}

