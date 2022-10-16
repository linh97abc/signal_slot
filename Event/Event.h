#ifndef __EVENT_H__
#define __EVENT_H__

#include <iostream>
#include <vector>

class Object
{
protected:
    const char *objectName;

public:
    Object(const char *objectName = nullptr) : objectName(objectName)
    {
    }
    const char *ToString()
    {
        return this->objectName;
    }
};

namespace EventImpl
{
    template <class... ArgType>
    class EventMetaDataBase
    {
    public:
        EventMetaDataBase *next;
        virtual void Invoke(ArgType... arg) = 0;
    };

    template <class ParentType, class... ArgType>
    class EventMetadata : public EventMetaDataBase<ArgType...>
    {
        using EventHandler_t = void (ParentType::*)(void);

        ParentType *parent;
        EventHandler_t handler;

    public:
        EventMetadata(ParentType *parent, EventHandler_t handler, EventMetaDataBase<ArgType...> *next)
        {
            this->parent = parent;
            this->handler = handler;
            this->next = next;
        }

        void Invoke(ArgType... arg)
        {

            (this->parent->*handler)(arg...);
        }
    };

};

template <class... ArgType>
class EventBase : public Object
{
    EventBase(EventBase &) = delete;

protected:
    EventImpl::EventMetaDataBase<ArgType...> *eventData;

public:
    EventBase(const char *name = nullptr) : Object(name), eventData(nullptr)
    {
    }

    template <class ParentType>
    void Connect(ParentType *parent, void (ParentType::*handler)(ArgType...))
    {
        if ((parent != nullptr) && (handler != nullptr))
        {

            EventImpl::EventMetaDataBase<ArgType...> *newEventDataItem =
                new EventImpl::EventMetadata<ParentType, ArgType...>(parent, handler, this->eventData);

            this->eventData = newEventDataItem;
        }
    }

    virtual void Raise(ArgType... arg) = 0;
};

class EventAsync : public ::EventBase<>
{
private:
    /* data */
public:
    class Dispatcher
    {
        using EventAsyncData_t = EventImpl::EventMetaDataBase<>;
        friend class EventAsync;

    private:
        std::vector<EventAsyncData_t *> eventMetadataList;
        Dispatcher(/* args */) = default;

        void Push(EventAsyncData_t *pEventData)
        {
            eventMetadataList.push_back(pEventData);
        }

    public:
        static Dispatcher *GetInst();

        void InvokeAll();
    };

    EventAsync(const char *name = nullptr) : ::EventBase<>(name) {}

    void Raise();
};

template <class... ArgType>
class Event : public ::EventBase<ArgType...>
{
public:
    Event(const char *name = nullptr) : ::EventBase<ArgType...>(name) {}

    void Raise(ArgType... arg)
    {
        EventImpl::EventMetaDataBase<ArgType...> *iter = this->eventData;

        std::cout << this->ToString() << " raise" << std::endl;

        while (iter != nullptr)
        {
            iter->Invoke(arg...);
            iter = iter->next;
        }
    }
};

#endif
