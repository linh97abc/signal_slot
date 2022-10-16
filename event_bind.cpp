#include <functional>
#include <vector>
#include <iostream>

namespace myLib
{
    class Slot;
    class Dispatcher;

    class Signal
    {

        friend class Dispatcher;

    private:
        /* data */
        std::vector<std::function<void()>> onSigRecieveds;

        void Trigger()
        {
            for (auto &onSigRecieved : onSigRecieveds)
            {
                onSigRecieved();
            }
        }

    public:
        void Emit();

        void connect(std::function<void()> onSigRecieved)
        {
            this->onSigRecieveds.push_back(onSigRecieved);
        }
    };

    class Dispatcher
    {
    private:
        static std::vector<Signal *> signals;

    public:
        static void Push(Signal *signal)
        {
            // Dispatcher::signals.push_back(signal);
            signal->Trigger();
        }

        static void Trigger()
        {
            while (Dispatcher::signals.size() > 0)
            {
                auto signal = Dispatcher::signals.at(0);
                signal->Trigger();
                Dispatcher::signals.pop_back();
            }
        }
    };

    class State
    {
    private:
        /* data */
        bool isActive;
        std::function<bool()> converter;

        const char *name;

        void setIsActive(bool value)
        {
            isActive = value;

            if (value)
            {
                std::cout << name << " setIsActive" << std::endl;
            }
        }

        void converterWrap(std::function<bool()> converter)
        {
            setIsActive(converter());
        }

    public:
        State(bool isActive, const char *name = nullptr) : name(name)
        {
            setIsActive(isActive);
        }

        bool IsActive()
        {
            return isActive;
        }

        void Binding(Signal &trigger, std::function<bool()> converter)
        {
            trigger.connect(std::bind(&converterWrap, this, converter));
        }
    };

    class ComboState
    {
    private:
        /* data */
    public:
        ComboState(/* args */);
        
        void Add(State &state)
        {
            
        }
    };

    

} // namespace myLib

std::vector<myLib::Signal *> myLib::Dispatcher::signals;

void myLib::Signal::Emit()
{
    myLib::Dispatcher::Push(this);
}

using namespace myLib;

class View1
{
private:
    State s1, s2;

public:
    Signal sig;

    int prop1, prop2;

    View1(/* args */) : s1(true, "s1"), s2(false, "s2")
    {
        prop1 = 0;

        s1.Binding(sig, [&]()
                   { return (prop1 == 1) || (s2.IsActive()); });

        s2.Binding(sig, [&]()
                   { return (prop1 == 2) || (s1.IsActive()); });
    }

    ~View1(){};
};

int main()
{
    View1 v1;
    std::cout << " ---" << std::endl;

    v1.sig.Emit();

    std::cout << " ---" << std::endl;
    v1.prop1 = 2;
    v1.sig.Emit();

    std::cout << " ---" << std::endl;
    v1.sig.Emit();

    std::cout << " ---" << std::endl;
    v1.prop1 = 1;
    v1.sig.Emit();

    std::cout << " ---" << std::endl;
    return 0;
}