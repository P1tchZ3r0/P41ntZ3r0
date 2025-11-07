#include "Invoker.h"

Invoker::Invoker()
{ }

void Invoker::clearRedoStack()
{
	while (!redoStack.empty()) {
		delete redoStack.back();
		redoStack.pop_back();
	}
}

void Invoker::execute(Command* command)
{
	clearRedoStack();
	command->execute();
	undoStack.push_back(command);
	
	if (undoStack.size() > MAX_HIST_LEN) {
		delete undoStack.front();
		undoStack.pop_front();
	}
}

void Invoker::undo()
{
	if (undoStack.empty()) return;
	
	Command* command = undoStack.back();
	undoStack.pop_back();
	command->undo();
	redoStack.push_back(command);
}

void Invoker::redo()
{
	if (redoStack.empty()) return;
	
	Command* command = redoStack.back();
	redoStack.pop_back();
	command->execute();
	undoStack.push_back(command);
}

bool Invoker::canUndo() const
{
	return !undoStack.empty();
}

bool Invoker::canRedo() const
{
	return !redoStack.empty();
}

Invoker::~Invoker()
{
	while (!undoStack.empty()) {
		delete undoStack.back();
		undoStack.pop_back();
	}
	while (!redoStack.empty()) {
		delete redoStack.back();
		redoStack.pop_back();
	}
}

