#include "mlib/mlib.hpp"
#include <csignal>

#include "engine.hpp"
Engine engine;

using namespace mlib;

void signalHandler(int signum)
{
    Out = "Interrupt signal (0x" + int2hex(signum) + ") received.\n";
    engine.term();
    exit(signum);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGTERM, signalHandler);
    return engine.run();
}
