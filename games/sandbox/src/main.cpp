#include <mellohi/assets/asset_registry.hpp>
#include <mellohi/assets/asset_cache.hpp>
#include <mellohi/core/config_assets.hpp>
#include <mellohi/core/engine.hpp>
#include <mellohi/core/logger.hpp>
#include <mellohi/events/event_dispatcher.hpp>

using namespace mellohi;

int main()
{
    MH_INFO("Hello, world!");
    
    auto &engine = Engine::get();
    engine.initialize();
    
    return 0;
}
