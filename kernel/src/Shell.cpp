#include <Shell.h>
#include <SmartOS.h>
const char* handleShell(const char* input)
{
    if (memcmp(input, "echo ", 5) == 0)
    {
        return input + 5;
    }
    return NULL;
}