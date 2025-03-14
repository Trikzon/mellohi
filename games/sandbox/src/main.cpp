#include <mellohi/core/engine.hpp>

using namespace mellohi;

int main()
{
    auto &engine = Engine::get();
    engine.initialize();
    engine.run();

    return 0;
}

