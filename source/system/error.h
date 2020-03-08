#pragma once

#include <string>

#ifdef POP_ASSERT_ENABLED
    #define popAssert(condition, conditionString)   \
        if(!(condition))                            \
        {                                           \
            BreakWithMessageBox(conditionString);   \
        }                                        
#else
    #define popAssert(condition, conditionString)   \
        condition
#endif //POP_ASSERT_ENABLED

void BreakWithMessageBox(const std::string& conditionString);
