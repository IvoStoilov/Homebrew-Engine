#pragma once
#include <system/singleton/singleton.h>

struct ID3D11Device;
class GfxResourceManager
{
    POP_DECLARE_SINGLETON(ResourceManager);
public:
    template <class T, typename...Params>
    SharedPtr<T> Resolve(ComPtr<ID3D11Device>& device, Params&&... params);

    using ObjectID = String;
private:
    std::unordered_map<ObjectID, SharedPtr<GfxBindableResource>> m_Resources;
};

template <class T, typename...Params>
SharedPtr<T> GfxResourceManager::Resolve(ComPtr<ID3D11Device>& device, Params&&... params)
{
    static_assert(std::is_base_of<GfxBindableResource, T>::value, "Can only resolve classes derived from GfxBindableResource");
    const ObjectID key = T::GenerateObjectID(std::forward<Params>(p)...params);
    const auto valueIt = m_Resources.find(key);

    if (valueIt != m_Resources.end())
        return std::static_pointer_cast<T>(valueIt->second);

    SharedPtr<T> newResource = std::make_shared(device, std::forward<Params>(params)...);
    m_Resources[key] = newResource;
    return newResource;
}
