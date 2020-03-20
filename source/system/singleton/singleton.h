#pragma once

#define POP_DECLARE_SINGLETON(Type)                                                       \
public:                                                                                   \
    static inline Type& GetInstance()                                                     \
    {                                                                                     \
        popAssert(ms_Instance != nullptr, "{}::CreateInstance() not called", #Type);      \
        return *ms_Instance;                                                              \
    }                                                                                     \
    static void CreateInstance()                                                          \
    {                                                                                     \
        popAssert(!ms_Instance, "Calling {}::CreateInstance for the second time", #Type); \
        ms_Instance = new Type();                                                         \
    }                                                                                     \
    static void CleanInstance()                                                           \
    {                                                                                     \
        popAssert(ms_Instance, "Calling {}::CleanInstance for the second time", #Type);   \
        delete ms_Instance;                                                               \
        ms_Instance = nullptr;                                                            \
    }                                                                                     \
    Type(const Type&) = delete;                                                           \
    Type& operator= (const Type&) = delete;                                               \
private:                                                                                  \
    inline static Type* ms_Instance = nullptr;