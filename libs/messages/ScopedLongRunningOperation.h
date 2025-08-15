#pragma once

#include "iradiant.h"
#include "LongRunningOperationMessage.h"
#include "util/Noncopyable.h"

namespace radiant
{

/**
 * Scoped helper class dispatching a LongRunningOperationMessage
 * to WorldEdit's message bus. The constructor will send 
 * the Started event, the destructor will do the Finished part
 */
class ScopedLongRunningOperation : 
	public util::Noncopyable
{
public:
	ScopedLongRunningOperation(const std::string& message)
	{
		LongRunningOperationMessage started(OperationEvent::Started, message);
		GlobalRadiantCore().getMessageBus().sendMessage(started);
	}

	~ScopedLongRunningOperation()
	{
		LongRunningOperationMessage finished(OperationEvent::Finished);
		GlobalRadiantCore().getMessageBus().sendMessage(finished);
	}
};

}
