#include "asio_sample.h"

int main()
{
    void (*pArray[]) () = {
        // usingTimerSynchronously, 
        // usingTimerAsynchronously, 
        // bindingArgsToCompletionHandler,
        // usingMemberFunctionAsCompletionHandler,
        synchronisingCompletionHandlersInMultithreadedPrograms
    };
    for (auto p: pArray)
        p();
    return 0;
}