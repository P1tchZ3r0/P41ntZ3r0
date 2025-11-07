#ifndef COMMANDS_H
#define COMMANDS_H

#include <QImage>

class Canvas;

class Command {
public:
	Command();
	virtual void execute() = 0;
	virtual void undo() = 0;
	virtual ~Command();
};

class DrawCommand : public Command {
private:
	Canvas* canvas;
	QImage beforeImage;
	QImage afterImage;
	
public:
	DrawCommand(Canvas* canvas, const QImage& before, const QImage& after);
	virtual void execute() override;
	virtual void undo() override;
	virtual ~DrawCommand();
};

#endif
