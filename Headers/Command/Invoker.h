#ifndef INVOKER_H
#define INVOKER_H

#include "Commands.h"
#include <deque>

using std::deque;

class Invoker {
	deque<Command*> undoStack;
	deque<Command*> redoStack;
	static const int MAX_HIST_LEN = 20; // Уменьшено для экономии памяти
	
	void clearRedoStack();
	
public:
	Invoker();
	~Invoker();
	void execute(Command* command);
	void undo();
	void redo();
	bool canUndo() const;
	bool canRedo() const;
};


#endif
