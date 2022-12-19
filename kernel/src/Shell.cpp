#include <Shell.h>
#include <SmartOS.h>
uint64_t getIndexOf(const char* input, char c)
{
    size_t i = 0;
    while (input[i])
    {
        if (input[i] == c)
        {
            return i;
        }
        i++;
    }
    return i;
}
Vector<const char*> getCommandLineOptions(const char* input)
{
    size_t i = 0, j = 0;
    bool inQuote = false;
    bool backslash = false;
    Vector<const char*> options = {};
    while (input[i])
    {
        if (input[i] == '\"')
        {
            if (!backslash) inQuote = !inQuote;
        }
        if (input[i] == '\\')
        {
            backslash = true;
        }
        else backslash = false;
        if (input[i] == ' ' && !inQuote)
        {
            if (input[i - 1] == '\"' && (i == 1 || input[i - 2] != '\\'))
            {
                char* buffer = new char[i - j - 1];
                memcpy(buffer, input + j + 1, i - j - 2);
                buffer[i - j - 2] = 0;
                options.push(buffer);
            }
            else
            {
                char* buffer = new char[i - j + 1];
                memcpy(buffer, input + j, i - j);
                buffer[i - j] = 0;
                options.push(buffer);
            }
            j = i + 1;
        }
        i++;
    }
    if (input[i - 1] == '\"' && (i == 1 || input[i - 2] != '\\'))
    {
        char* buffer = new char[i - j - 1];
        memcpy(buffer, input + j + 1, i - j - 2);
        buffer[i - j - 2] = 0;
        options.push(buffer);
    }
    else
    {
        char* buffer = new char[i - j + 1];
        memcpy(buffer, input + j, i - j);
        buffer[i - j] = 0;
        options.push(buffer);
    }
    return options;
}
const char* handleShell(const char* input)
{
    auto options = getCommandLineOptions(input);
    if (strcmp(options[0], "echo") == 0)
    {
        const char* str = "";
        for (size_t i = 1; i < options.size(); i++)
        {
            str = strcat(str, options[i]);
            if (i != options.size() - 1) str = strcat(str, " ");
        }
        return str;
    }
    return NULL;
}