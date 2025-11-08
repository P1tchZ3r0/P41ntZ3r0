#include "MainWindow.h"
#include "Command/Commands.h"
#include <QLineEdit>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent)
{
	invoker = new Invoker();
	
	mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	
	// Тулбар сверху горизонтально
	buttonPanel = new ButtonPanel(this);
	canvas = new Canvas(this);

	buttonPanel->setBrushSizeValue(canvas->getBrushSize());
	buttonPanel->setCurrentColor(canvas->getCurrentColor());

	mainLayout->addWidget(buttonPanel);
	
	// Canvas занимает оставшееся пространство
	mainLayout->addWidget(canvas, 1);
	canvas->setFocus();
	
	// Окно масштабируемое
	this->setMinimumSize(800, 600);
	this->resize(1024, 768);
	
	setupMenuBar();
	connectSignals();
	setupShortcuts();
}

void MainWindow::setupMenuBar()
{
	// Меню интегрировано в тулбар
}

void MainWindow::setupShortcuts()
{
	// Горячие клавиши для undo/redo
	undoShortcut = new QShortcut(QKeySequence::Undo, this);
	connect(undoShortcut, &QShortcut::activated, this, &MainWindow::onUndo);
	
	redoShortcut = new QShortcut(QKeySequence::Redo, this);
	connect(redoShortcut, &QShortcut::activated, this, &MainWindow::onRedo);
}

void MainWindow::connectSignals()
{
	// Подключаем сигналы Canvas к слотам
	connect(canvas, &Canvas::drawingFinished, this, &MainWindow::onDrawingFinished);
	
	// Подключаем кнопки из ButtonPanel
	connect(buttonPanel, &ButtonPanel::colorPickerRequested, this, [this]() {
		QColor color = QColorDialog::getColor(canvas->getCurrentColor(), this, "Выберите цвет");
		if (color.isValid()) {
			canvas->setColor(color);
			buttonPanel->setCurrentColor(color);
		}
	});
	connect(buttonPanel, &ButtonPanel::brushModeSelected, canvas, [this]() {
		canvas->setDrawingMode(DrawingMode::Brush);
	});
	connect(buttonPanel, &ButtonPanel::eraserModeSelected, canvas, [this]() {
		canvas->setDrawingMode(DrawingMode::Eraser);
	});
	connect(buttonPanel, &ButtonPanel::brushSizeChanged, canvas, &Canvas::setBrushSize);
	connect(buttonPanel, &ButtonPanel::rectangleModeSelected, canvas, [this]() {
		canvas->setDrawingMode(DrawingMode::Rectangle);
	});
	connect(buttonPanel, &ButtonPanel::circleModeSelected, canvas, [this]() {
		canvas->setDrawingMode(DrawingMode::Circle);
	});
	connect(buttonPanel, &ButtonPanel::lineModeSelected, canvas, [this]() {
		canvas->setDrawingMode(DrawingMode::Line);
	});
	connect(buttonPanel, &ButtonPanel::textModeSelected, canvas, [this]() {
		canvas->setDrawingMode(DrawingMode::Text);
	});
connect(buttonPanel, &ButtonPanel::fillModeSelected, canvas, [this]() {
	canvas->setDrawingMode(DrawingMode::Fill);
});
	connect(buttonPanel, &ButtonPanel::clearRequested, this, &MainWindow::onClear);
	connect(buttonPanel, &ButtonPanel::saveRequested, this, &MainWindow::onSave);
	connect(buttonPanel, &ButtonPanel::openRequested, this, &MainWindow::onOpen);

	connect(canvas, &Canvas::textEntryRequested, this, [this](const QPoint& imagePos) {
		bool ok {false};
		QString text = QInputDialog::getText(this, tr("Вставка текста"), tr("Введите текст:"), QLineEdit::Normal, QString(), &ok);
		if (ok && !text.trimmed().isEmpty()) {
			canvas->addText(imagePos, text);
		} else {
			canvas->cancelPendingOperation();
		}
	});
}

void MainWindow::onDrawingFinished(const QImage& beforeImage, const QImage& afterImage)
{
	Command* command = new DrawCommand(canvas, beforeImage, afterImage);
	invoker->execute(command);
}

void MainWindow::onUndo()
{
	invoker->undo();
}

void MainWindow::onRedo()
{
	invoker->redo();
}

void MainWindow::onSave()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Сохранить изображение", 
		"", "PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;BMP Files (*.bmp);;All Files (*)");
	if (!fileName.isEmpty()) {
		QFileInfo fileInfo(fileName);
		QString suffix = fileInfo.suffix().toLower();
		if (suffix.isEmpty()) {
			suffix = "png";
			fileName += ".png";
		}
		
		QByteArray format = suffix.toUpper().toUtf8();
		if (!canvas->saveImage(fileName, format)) {
			QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
		}
	}
}

void MainWindow::onOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Открыть изображение", 
		"", "Image Files (*.png *.jpg *.jpeg *.bmp);;All Files (*)");
	if (!fileName.isEmpty()) {
		if (!canvas->loadImage(fileName)) {
			QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
		}
	}
}

void MainWindow::onClear()
{
	canvas->clear();
}

MainWindow::~MainWindow()
{
	delete invoker;
}
