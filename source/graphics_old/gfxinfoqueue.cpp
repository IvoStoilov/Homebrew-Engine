#include <graphics/precompile.h>
#include <graphics/gfxinfoqueue.h>

#ifdef POP_DEBUG
GfxInfoQueue::GfxInfoQueue()
{
    typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
    HMODULE dxgiDebug = LoadLibrary("dxgidebug.dll");
    fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(dxgiDebug, "DXGIGetDebugInterface");

    HRESULT result = DXGIGetDebugInterface(__uuidof(IDXGIInfoQueue), &m_InfoQueue);
    popAssert(!FAILED(result), "");
}

void GfxInfoQueue::Set()
{
    m_NextMessageIndex = m_InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

String GfxInfoQueue::GetGfxMessage() const
{
    String result;
    const u64 lastMessageIndex = m_InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
    for (u64 i = m_NextMessageIndex; i < lastMessageIndex; ++i)
    {
        size_t messageLength = 0u;
        m_InfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);

        UniquePtr<char[]> buffer = std::make_unique<char[]>(messageLength);
        DXGI_INFO_QUEUE_MESSAGE* dxgiMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(buffer.get());
        m_InfoQueue->GetMessage(DXGI_DEBUG_ALL, i, dxgiMessage, &messageLength);

        result.append(dxgiMessage->pDescription, messageLength);
        result.append("\n");
    }

    return result;
}
#endif //POP_DEBUG