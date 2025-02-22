#include <mellohi/core/engine.hpp>
#include <mellohi/core/logger.hpp>

using namespace mellohi;

int main()
{
    MH_INFO("Hello, world!");
    
    auto &engine = Engine::get();
    
    auto &registry = engine.asset_registry();
    
    const AssetPath p{"@core:game:config.toml"};
    const AssetPath p2{"@core:engine:config.toml"};
    
    auto &cache = engine.asset_cache();
    
    // {
        const auto asset = cache.fetch<TextAsset>(registry.asset_id_from_path(p));
        MH_INFO("Text: {}", asset->text());
    // }
    
    const auto asset2 = cache.fetch<DependentAsset>(registry.asset_id_from_path(p2));
    MH_INFO("Text: {}", asset2->text());
}
