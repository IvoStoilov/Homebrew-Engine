#pragma once
#ifdef POP_DEBUG
#include <system/singleton/singleton.h>

class GfxInfoQueue
{
    POP_DECLARE_SINGLETON(GfxInfoQueue);
    GfxInfoQueue();
public:
    void Set();
    String GetGfxMessage() const;

private:
    u64 m_NextMessageIndex = 0u;
    ComPtr<IDXGIInfoQueue> m_InfoQueue;
};

#define popGfxInfoQueue GfxInfoQueue::GetInstance()
#endif //POP_DEBUG

#ifdef POP_DEBUG
    #define popGfxVerify(hrCall) \
    do                           \
    {                            \
        popGfxInfoQueue.Set();   \
        HRESULT hr = (hrCall);   \
        if (FAILED(hr))          \
        {                        \
            popAssert(false, "Line: {} {}\n{}", __LINE__, __FILE__, popGfxInfoQueue.GetGfxMessage()); \
        }                        \
    } while(false)

#else
    #define popGfxVerify(hrCall) (hrCall)
#endif