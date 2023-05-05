#include "asio_sample.h"

int main()
{
    void (*pArray[]) () = {
        // usingTimerSynchronously, 
        // usingTimerAsynchronously, 
        // bindingArgsToCompletionHandler,
        usingMemberFunctionAsCompletionHandler
    };
    for (auto p: pArray)
        p();
    return 0;
}