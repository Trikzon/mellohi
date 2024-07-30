#pragma once

#include "mellohi/core/types.hpp"

namespace mellohi
{
    class AssetId
    {
    public:
        explicit AssetId(str fully_qualified_id);
        AssetId(str namespace_, str id);

        auto operator==(const AssetId &other) const -> bool;
        auto operator!=(const AssetId &other) const -> bool;
        friend auto operator<<(std::ostream &os, const AssetId &asset_id) -> std::ostream &;

        auto get_fully_qualified_id() const -> string;
        auto get_namespace() const -> const string &;
        auto get_id() const -> const string &;

        auto file_exists() const -> bool;
        auto get_file_path() const -> path;
        auto read_file_to_string() const -> string;

    private:
        string m_namespace{}, m_path{};
    };
}
