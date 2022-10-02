#include <iostream>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class Qobject
{
};

template <class... ArgType>
class Signal
{
    struct Slot_metadata
    {
        Qobject *target;
        std::function<void(ArgType... arg)> method;

        Slot_metadata(Qobject *target, std::function<void(ArgType... arg)> method)
            : target(target), method(method)
        {
        }
    };

public:
    std::vector<Slot_metadata*> slots;

public:
    // Signal() {
    //     std::cout << "Create signal" << std::endl;
    // };

    void add_slot(Qobject *target, std::function<void(ArgType... arg)> method)
    {
        slots.push_back(new Slot_metadata(target, method));
    }

    void _emit(ArgType... arg)
    {
        for (auto &slot : slots)
        {
            slot->method(arg...);
        }
    }

    void operator()(ArgType... arg)
    {
        _emit(arg...);
    }
};

template <class... ArgType, class _Receive_t>
void connect(Signal<ArgType...> &signal, Qobject *receiver, Signal<ArgType...> &signal_next)
{
    std::function<void(ArgType... arg)> method = [signal_next](ArgType... arg)
    { signal_next.emit(arg...); };

    signal.add_slot(receiver, method);
}

template <class ArgType, class _Receive_t>
void connect(Signal<ArgType> &signal, _Receive_t *reciever, void (_Receive_t::*handler)(ArgType))
{
    signal.add_slot(reciever, std::bind(handler, reciever, std::placeholders::_1));
}

template <class ArgType1, class ArgType2, class _Receive_t>
void connect(Signal<ArgType1, ArgType2> &signal, _Receive_t *reciever, void (_Receive_t::*handler)(ArgType1, ArgType2))
{
    signal.add_slot(reciever, std::bind(handler, reciever, std::placeholders::_1, std::placeholders::_2));
}

template <class ArgType1, class ArgType2, class ArgType3, class _Receive_t>
void connect(Signal<ArgType1, ArgType2, ArgType3> &signal,
             _Receive_t *reciever,
             void (_Receive_t::*handler)(ArgType1, ArgType2, ArgType3))
{
    signal.add_slot(reciever, std::bind(handler,
                              reciever,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3));
}

template <class ArgType1, class ArgType2, class ArgType3, class ArgType4, class _Receive_t>
void connect(Signal<ArgType1, ArgType2, ArgType3, ArgType4> &signal,
             _Receive_t *reciever,
             void (_Receive_t::*handler)(ArgType1, ArgType2, ArgType3, ArgType4))
{
    signal.add_slot(reciever, std::bind(handler,
                              reciever,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4));
}

template <class ArgType1, class ArgType2, class ArgType3, class ArgType4, class ArgType5, class _Receive_t>
void connect(Signal<ArgType1, ArgType2, ArgType3, ArgType4, ArgType5> &signal,
             _Receive_t *reciever,
             void (_Receive_t::*handler)(ArgType1, ArgType2, ArgType3, ArgType4, ArgType5))
{
    signal.add_slot(reciever, std::bind(handler,
                              reciever,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4,
                              std::placeholders::_5));
}

#define slots public
#define signals public
#define emit

class Publisher : public Qobject
{
signals:
    Signal<int &, int> send;

public:
    Publisher() = default;
};

class Subcriber : public Qobject
{
private:
    int data;

public:
    Subcriber(int data)
    {
        this->data = data;
    }

    slots:
    void On_Receive(int &arg, int arg2)
    {
        std::cout << "Sub: " << data << " " << arg << " " << arg2 << std::endl;
        arg++;
    }
};

int main(int argc, char const *argv[])
{
    Publisher p;
    Subcriber sa(0), sb(1);

    int data = 0;
    int &xx = data;

    connect(p.send, &sa, Subcriber::On_Receive);
    connect(p.send, &sb, Subcriber::On_Receive);

    emit p.send(xx, 2);

    return 0;
}
