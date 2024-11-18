#include <asio.hpp>
#include <iostream>
#include <functional>

class Printer
{
public:
    Printer(asio::io_context& io)
    :_io(io),
     _timer(_io, asio::chrono::seconds(1))
    {
        _timer.async_wait(std::bind(&Printer::print, this));
    }

    void print()
    {
        if (_counter < 5)
        {
            std::cout << "Count is  " << _counter << '\n';
            _timer.expires_from_now(asio::chrono::seconds(1));
            _counter++;

            _timer.async_wait(std::bind(&Printer::print, this));
        }        
    }

    int counter() const
    {
        return _counter;
    }

private:
    asio::io_context& _io;
    asio::steady_timer _timer;
    int _counter = 0;
};

int main()
{
    asio::io_context io;
    Printer printer(io);
    io.run();

    std::cout << "The final count is " << printer.counter() << '\n';
    return 0;
}

