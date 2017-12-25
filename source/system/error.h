#pragma once

#include <string>

#define POP_ASSERT_ENABLE 

#ifdef POP_ASSERT_ENABLE
    #define popAssert(condition, conditionString)   \
        if(!(condition))                            \
        {                                           \
            BreakWithMessageBox(conditionString);   \
        }                                        
#else
    #define popAssert(condition, conditionString)   \
        condition
#endif //POP_ASSERT_ENABLE

void BreakWithMessageBox(const std::string& conditionString);
