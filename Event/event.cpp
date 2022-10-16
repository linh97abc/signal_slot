#include <iostream>
#include "Event.h"

class StateRegion;

class StateBase
{
    friend class StateRegion;

protected:
    StateBase *next;
    bool isActive;

public:
    bool IsActive()
    {
        return isActive;
    }
};

class StateRegion : public Object
{
private:
    StateBase *beginOfStateLists;

public:
    StateRegion(const char *name = nullptr) : beginOfStateLists(nullptr)
    {
    }

    void AddState(StateBase *state)
    {
        if (this->beginOfStateLists == nullptr)
        {
            state->isActive = true;
        }
        else
        {
            state->isActive = false;
        }

        state->next = this->beginOfStateLists;
        this->beginOfStateLists = state;

        // std::cout << this->beginOfStateLists << " " << this->beginOfStateLists->next << std::endl;
    }

    void SelectState(StateBase *state)
    {
        StateBase *iter = this->beginOfStateLists;

        while (iter != nullptr)
        {
            // std::cout << __func__ << " " << __LINE__ << std::endl;
            iter->isActive = (iter == state) ? true : false;

            iter = iter->next;
        }
    }

    StateBase *GetActiveState()
    {
        StateBase *iter = this->beginOfStateLists;

        while (iter != nullptr)
        {
            if (iter->isActive)
            {
                return iter;
            }

            iter = iter->next;
        }
    }
};

class State : public StateBase, public Object
{
private:
    StateRegion *parent;

public:
    State(StateRegion *parent, const char *name = nullptr) : Object(name)
    {
        if (parent != nullptr)
        {
            // std::cout << __func__ << " " << parent << " " << name << std::endl;
            this->parent = parent;

            parent->AddState(this);
        }
    }

    void SetActive(bool isActive)
    {
        // std::cout << this->ToString() << " " << __func__ << std::endl;
        this->parent->SelectState(this);
    }
};

#include <iostream>

class View1
{
public:
    void OnEventRecieved1()
    {
        std::cout << __func__ << std::endl;
    }
};

class View : public StateRegion
{

private:
    /* data */

    void OnEventRecieved()
    {
        std::cout << __func__ << " " << prop << std::endl;
    }

public:
    int prop;
    Event<> e;

    EventAsync eAsync;

    View1 v;

    State s1, s2, s3;

    View(/* args */) : e("Event1"), s1(this, "state1"), s2(this, "state2"), s3(this, "state3"), eAsync("event_async")
    {
        this->prop = 1;
        e.Connect(this, &OnEventRecieved);
        e.Connect(&v, &View1::OnEventRecieved1);

        eAsync.Connect(this, &OnEventRecieved);
    }
};

int main()
{
    View v;

    {
        View v2;
    }

    EventBase<> *pEvent;

    pEvent = &v.e;

    pEvent->Raise();

    // v.e.Raise();
    pEvent = &v.eAsync;
    pEvent->Raise();
    // v.eAsync.Raise();

    std::cout << v.s1.ToString() << " " << v.s1.IsActive() << std::endl;
    std::cout << v.s2.ToString() << " " << v.s2.IsActive() << std::endl;
    std::cout << v.s3.ToString() << " " << v.s3.IsActive() << std::endl;

    v.s2.SetActive(true);

    std::cout << v.s1.ToString() << " " << v.s1.IsActive() << std::endl;
    std::cout << v.s2.ToString() << " " << v.s2.IsActive() << std::endl;
    std::cout << v.s3.ToString() << " " << v.s3.IsActive() << std::endl;

    v.s3.SetActive(true);

    std::cout << v.s1.ToString() << " " << v.s1.IsActive() << std::endl;
    std::cout << v.s2.ToString() << " " << v.s2.IsActive() << std::endl;
    std::cout << v.s3.ToString() << " " << v.s3.IsActive() << std::endl;

    EventAsync::Dispatcher::GetInst()->InvokeAll();

    return 0;
}
