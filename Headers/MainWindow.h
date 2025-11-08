#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGridLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QColorDialog>
#include <QShortcut>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include "ButtonPanel.h"
#include "Canvas.h"
#include "Invoker.h"

class MainWindow : public QWidget {
	Q_OBJECT

	QVBoxLayout* mainLayout {nullptr};
	ButtonPanel* buttonPanel {nullptr};
	Canvas* canvas;
	Invoker* invoker;
	
	QShortcut* undoShortcut;
	QShortcut* redoShortcut;
	
	void setupMenuBar();
	void connectSignals();
	void setupShortcuts();

private slots:
	void onDrawingFinished(const QImage& beforeImage, const QImage& afterImage);
	void onUndo();
	void onRedo();
	void onSave();
	void onOpen();
	void onClear();

public:
	MainWindow(QWidget* parent=nullptr);	
	~MainWindow();
};


#endif
