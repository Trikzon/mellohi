#pragma once

#include "mellohi/core/types.hpp"

#include <memory>

namespace mellohi
{
    class Window : public std::enable_shared_from_this<Window>
    {
    public:
        ~Window();
        
        bool should_close() const;
    
    private:
        friend class Platform;
        
        struct ImplData;
        
        std::unique_ptr<ImplData> m_data;
        
        Window(uvec2 size, bool resizable, const std::string &title);
    };
}
