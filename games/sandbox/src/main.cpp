#include <mellohi/core/engine.hpp>
#include <mellohi/core/logger.hpp>

using namespace mellohi;

int main()
{
    MH_INFO("Hello, world!");
    
    auto &engine = Engine::get();
    engine.initialize();
    
    engine.run();
    
    return 0;
}
