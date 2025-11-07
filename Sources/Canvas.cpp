#include "Canvas.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

Canvas::Canvas(QWidget* parent) : QWidget(parent), 
    currentColor(Qt::black), 
    brushSize(3),
    drawingMode(DrawingMode::Brush),
    isDrawing(false)
{
    setMinimumSize(400, 300);
    setStyleSheet("background-color: white;");
    
    // Создаем начальное изображение фиксированного разумного размера
    image = QImage(1024, 768, QImage::Format_ARGB32);
    image.fill(Qt::white);
}

Canvas::~Canvas()
{ }

void Canvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Рисуем изображение, масштабируя его под размер виджета
    if (!image.isNull()) {
        painter.drawImage(rect(), image);
    }
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isDrawing = true;
        lastPoint = event->pos();
        startPoint = event->pos();
        
        // Сохраняем состояние только один раз при начале рисования
        if (!imageBeforeDrawing.isNull()) {
            imageBeforeDrawing = QImage(); // Освобождаем предыдущую копию
        }
        imageBeforeDrawing = image.copy();
        
        if (drawingMode == DrawingMode::Brush) {
            drawPoint(event->pos());
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (isDrawing && event->buttons() & Qt::LeftButton) {
        if (drawingMode == DrawingMode::Brush) {
            drawLine(lastPoint, event->pos());
            lastPoint = event->pos();
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isDrawing) {
        if (drawingMode == DrawingMode::Rectangle) {
            drawRectangle(startPoint, event->pos());
        } else if (drawingMode == DrawingMode::Circle) {
            drawCircle(startPoint, event->pos());
        } else if (drawingMode == DrawingMode::Line) {
            drawLine(startPoint, event->pos());
        }
        isDrawing = false;
        
        // Отправляем сигнал о завершении рисования только если было изменение
        if (!imageBeforeDrawing.isNull()) {
            emit drawingFinished(imageBeforeDrawing, image);
            imageBeforeDrawing = QImage(); // Освобождаем память
        }
    }
}

QPoint Canvas::widgetToImage(const QPoint& widgetPoint) const
{
    if (image.isNull() || width() == 0 || height() == 0) {
        return widgetPoint;
    }
    
    // Преобразуем координаты виджета в координаты изображения
    qreal scaleX = static_cast<qreal>(image.width()) / width();
    qreal scaleY = static_cast<qreal>(image.height()) / height();
    
    return QPoint(static_cast<int>(widgetPoint.x() * scaleX),
                  static_cast<int>(widgetPoint.y() * scaleY));
}

void Canvas::drawPoint(const QPoint& point)
{
    QPoint imagePoint = widgetToImage(point);
    QPainter painter(&image);
    painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPoint(imagePoint);
    update();
}

void Canvas::drawLine(const QPoint& from, const QPoint& to)
{
    QPoint imageFrom = widgetToImage(from);
    QPoint imageTo = widgetToImage(to);
    QPainter painter(&image);
    painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(imageFrom, imageTo);
    update();
}

void Canvas::drawRectangle(const QPoint& start, const QPoint& end)
{
    QPoint imageStart = widgetToImage(start);
    QPoint imageEnd = widgetToImage(end);
    QPainter painter(&image);
    painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    QRect rect = QRect(imageStart, imageEnd).normalized();
    painter.drawRect(rect);
    update();
}

void Canvas::drawCircle(const QPoint& start, const QPoint& end)
{
    QPoint imageStart = widgetToImage(start);
    QPoint imageEnd = widgetToImage(end);
    QPainter painter(&image);
    painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    QRect rect = QRect(imageStart, imageEnd).normalized();
    painter.drawEllipse(rect);
    update();
}

void Canvas::setColor(const QColor& color)
{
    currentColor = color;
}

void Canvas::setBrushSize(int size)
{
    brushSize = size;
}

void Canvas::setDrawingMode(DrawingMode mode)
{
    drawingMode = mode;
}

void Canvas::clear()
{
    // Просто очищаем без создания команды - это быстрая операция
    // Если нужен undo, пользователь может использовать Ctrl+Z на последнем действии
    image.fill(Qt::white);
    update();
}

bool Canvas::saveImage(const QString& fileName)
{
    return image.save(fileName);
}

bool Canvas::loadImage(const QString& fileName)
{
    QImage loadedImage(fileName);
    if (!loadedImage.isNull()) {
        // Ограничиваем размер загружаемого изображения для экономии памяти
        const int MAX_SIZE = 2048;
        if (loadedImage.width() > MAX_SIZE || loadedImage.height() > MAX_SIZE) {
            loadedImage = loadedImage.scaled(MAX_SIZE, MAX_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        
        // Масштабируем изображение под текущий размер виджета
        int canvasWidth = qMax(width(), 400);  // Минимальный размер
        int canvasHeight = qMax(height(), 300);
        
        // Масштабируем загруженное изображение под размер canvas, сохраняя пропорции
        QSize targetSize = loadedImage.size().scaled(canvasWidth, canvasHeight, Qt::KeepAspectRatio);
        loadedImage = loadedImage.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // Создаем новое изображение нужного размера
        image = QImage(canvasWidth, canvasHeight, QImage::Format_ARGB32);
        image.fill(Qt::white);
        
        // Копируем загруженное изображение в центр
        QPainter painter(&image);
        int x = (canvasWidth - loadedImage.width()) / 2;
        int y = (canvasHeight - loadedImage.height()) / 2;
        painter.drawImage(x, y, loadedImage);
        
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
    update();
}

void Canvas::resizeEvent(QResizeEvent* event)
{
    // Ограничиваем максимальный размер изображения для экономии памяти
    const int MAX_WIDTH = 2048;
    const int MAX_HEIGHT = 2048;
    
    // Увеличиваем изображение только если окно стало больше, но не больше максимума
    int newWidth = qMin(qMax(width(), image.width()), MAX_WIDTH);
    int newHeight = qMin(qMax(height(), image.height()), MAX_HEIGHT);
    
    if (newWidth > image.width() || newHeight > image.height()) {
        resizeImage(&image, QSize(newWidth, newHeight));
    }
    QWidget::resizeEvent(event);
}

void Canvas::resizeImage(QImage* image, const QSize& newSize)
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(Qt::white);
    
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

