#include "UndoSystem.h"

#include "itextstream.h"

#include <iostream>

#include "Operation.h"
#include "StackFiller.h"

namespace undo 
{

UndoSystem::UndoSystem() :
	_activeUndoStack(nullptr),
	_undoLevels(RKEY_UNDO_QUEUE_SIZE)
{}

UndoSystem::~UndoSystem()
{
	clear();
}

IUndoStateSaver* UndoSystem::getStateSaver(IUndoable& undoable)
{
	auto result = _undoables.try_emplace(&undoable, *this, undoable);

	// If we're in the middle of an active undo operation, assign this to the tracker (#4861)
	if (_activeUndoStack != nullptr)
	{
		result.first->second.setStack(_activeUndoStack);
	}

	return &(result.first->second);
}

void UndoSystem::releaseStateSaver(IUndoable& undoable)
{
	_undoables.erase(&undoable);
}

void UndoSystem::start()
{
	_redoStack.clear();
	if (_undoStack.size() == _undoLevels.get())
	{
		_undoStack.pop_front();
	}
	startUndo();
}

bool UndoSystem::operationStarted() const
{
	return _activeUndoStack != nullptr;
}

void UndoSystem::cancel()
{
	if (_activeUndoStack != nullptr)
	{
		_activeUndoStack->cancel();
		setActiveUndoStack(nullptr);
	}
}

void UndoSystem::finish(const std::string& command)
{
	if (finishUndo(command))
	{
		rMessage() << command << std::endl;
		_eventSignal.emit(EventType::OperationRecorded, command);
	}
}

void UndoSystem::undo()
{
	if (_undoStack.empty())
	{
		rMessage() << "Undo: no undo available" << std::endl;
		return;
	}

	if (operationStarted())
	{
		rWarning() << "Undo not available while an operation is still in progress" << std::endl;
		return;
	}
		
	const auto& operation = _undoStack.back();
	auto operationName = operation->getName(); // copy this name, we need it after op destruction
	rMessage() << "Undo: " << operationName << std::endl;

	startRedo();
	operation->restoreSnapshot();
	finishRedo(operationName);
	_undoStack.pop_back();
	_eventSignal.emit(EventType::OperationUndone, operationName);
}

void UndoSystem::redo()
{
	if (_redoStack.empty())
	{
		rMessage() << "Redo: no redo available" << std::endl;
		return;
	}

	if (operationStarted())
	{
		rWarning() << "Redo not available while an operation is still in progress" << std::endl;
		return;
	}
		
	const auto& operation = _redoStack.back();
	auto operationName = operation->getName(); // copy this name, we need it after op destruction
	rMessage() << "Redo: " << operationName << std::endl;

	startUndo();
	operation->restoreSnapshot();
	finishUndo(operationName);
	_redoStack.pop_back();
	_eventSignal.emit(EventType::OperationRedone, operationName);
}

void UndoSystem::clear()
{
	setActiveUndoStack(nullptr);
	_undoStack.clear();
	_redoStack.clear();
	_eventSignal.emit(EventType::AllOperationsCleared, std::string());

	// greebo: This is called on map shutdown, so don't clear the observers,
	// there are some "persistent" observers like EntityInspector and ShaderClipboard
}

sigc::signal<void(IUndoSystem::EventType, const std::string&)>& UndoSystem::signal_undoEvent()
{
	return _eventSignal;
}

void UndoSystem::startUndo()
{
	_undoStack.start("unnamedCommand");
	setActiveUndoStack(&_undoStack);
}

bool UndoSystem::finishUndo(const std::string& command)
{
	bool changed = _undoStack.finish(command);
	setActiveUndoStack(nullptr);
	return changed;
}

void UndoSystem::startRedo()
{
	_redoStack.start("unnamedCommand");
	setActiveUndoStack(&_redoStack);
}

bool UndoSystem::finishRedo(const std::string& command)
{
	bool changed = _redoStack.finish(command);
	setActiveUndoStack(nullptr);
	return changed;
}

// Assigns the given stack to all of the Undoables listed in the map
void UndoSystem::setActiveUndoStack(UndoStack* stack)
{
	_activeUndoStack = stack;

	for (auto& pair : _undoables)
	{
		pair.second.setStack(_activeUndoStack);
	}
}

} // namespace undo
