#pragma once
#include <cstdint>

namespace engine {

using EntityID    = std::uint32_t;
using ComponentID = std::uint32_t;

static constexpr EntityID NULL_ENTITY = 0;

// Base class for all components (tag interface)
struct Component {
    virtual ~Component() = default;
};

// Helper to assign unique IDs to component types at runtime
namespace detail {
    inline ComponentID nextComponentID() {
        static ComponentID id = 0;
        return ++id;
    }
} // namespace detail

template <typename T>
ComponentID getComponentID() {
    static ComponentID id = detail::nextComponentID();
    return id;
}

} // namespace engine
