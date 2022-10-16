#include "Event.h"
#include <iostream>
#include <vector>



void EventAsync::Raise()
{
    std::cout << this->ToString() << " raise" << std::endl;
    EventAsync::Dispatcher::GetInst()->Push(this->eventData);
}

EventAsync::Dispatcher *EventAsync::Dispatcher::GetInst()
{
    static Dispatcher inst;

    return &inst;
}

void EventAsync::Dispatcher::InvokeAll()
{
    for (auto i : this->eventMetadataList)
    {
        EventImpl::EventMetaDataBase<> *iter = i;

        while (iter != nullptr)
        {
            iter->Invoke();
            iter = iter->next;
        }
    }

    this->eventMetadataList.clear();
}
