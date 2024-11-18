#define ASIO_STANDALONE
#include <asio.hpp>
#include <iostream>
#include <functional>
#include <thread>

class Printer
{
public:
    Printer(asio::io_context& io)
    : _strand(asio::make_strand(io))
    , _timer1(io, asio::chrono::seconds(1))
    , _timer2(io, asio::chrono::seconds(1))
    {
        _timer1.async_wait(asio::bind_executor(_strand, std::bind(&Printer::print1, this)));
        _timer2.async_wait(asio::bind_executor(_strand, std::bind(&Printer::print2, this)));
    }

    void print1()
    {
        if (_counter < 10)
        {
            std::cout << "Timer 1: Count is  " << _counter << '\n';
            _timer1.expires_from_now(asio::chrono::seconds(1));
            _counter++;

            _timer1.async_wait(asio::bind_executor(_strand, std::bind(&Printer::print1, this)));
        }
    }

    void print2()
    {
        if (_counter < 10)
        {
            std::cout << "Timer 2: Count is  " << _counter << '\n';
            _timer2.expires_from_now(asio::chrono::seconds(1));
            _counter++;

            _timer2.async_wait(asio::bind_executor(_strand, std::bind(&Printer::print2, this)));
        }
    }

    int counter() const
    {
        return _counter;
    }

private:
    asio::strand<asio::io_context::executor_type> _strand;
    asio::steady_timer                            _timer1;
    asio::steady_timer                            _timer2;
    int                                           _counter = 0;
};

int main()
{
    asio::io_context io;
    Printer          printer(io);
    std::thread      t([&io]() { io.run(); });
    io.run();

    std::cout << "The final count is " << printer.counter() << '\n';
    t.join();
    return 0;
}
