#include "Commands.h"
#include "Canvas.h"

Command::Command()
{ }
Command::~Command()
{ }

DrawCommand::DrawCommand(Canvas* _canvas, const QImage& before, const QImage& after)
	: canvas(_canvas), beforeImage(before), afterImage(after)
{ }

void DrawCommand::execute()
{
	if (canvas) {
		canvas->setImage(afterImage);
	}
}

void DrawCommand::undo()
{
	if (canvas) {
		canvas->setImage(beforeImage);
}
}

DrawCommand::~DrawCommand()
{ }
