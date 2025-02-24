#include <mellohi/assets/asset_registry.hpp>
#include <mellohi/assets/asset_cache.hpp>
#include <mellohi/core/engine.hpp>
#include <mellohi/core/logger.hpp>
#include <mellohi/events/event_dispatcher.hpp>

using namespace mellohi;

int main()
{
    MH_INFO("Hello, world!");
    
    auto &engine = Engine::get();
    auto &asset_registry = engine.asset_registry();
    auto &asset_cache = engine.asset_cache();
    
    const auto id0 = asset_registry.asset_id_from_path(AssetPath{"@test:test:id0"});
    const auto id1 = asset_registry.asset_id_from_path(AssetPath{"@test:test:id1"});
    
    const auto asset0 = asset_cache.fetch_or_load<TextAsset>(id0);
    const auto asset1 = asset_cache.fetch_or_load<TextAsset>(id1);
    
    asset_cache.load<TextAsset>(id0);
    
    asset_cache.add_dependency(id0, id1);
    asset_cache.add_dependency(id1, id0);
    MH_TRACE("Added dependency");
    
    asset_cache.load<TextAsset>(id1);
    
    return 0;
}
