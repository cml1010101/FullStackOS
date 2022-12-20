#include <Shell.h>
#include <SmartOS.h>
#include <MMU.h>
#include <Scheduler.h>
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
    else if (fileSystems[0]->exists(strcat("/bin/", options[0])))
    {
        File* file = fileSystems[0]->open(strcat("/bin/", options[0]));
        size_t fileSize = file->getSize();
        uint8_t* filedata = (uint8_t*)kmalloc_a(sizeof(filedata));
        file->read(filedata, fileSize);
        Thread* thread = new Thread((void(*)())filedata, options[0], false);
        thread->dir->map((uint64_t)filedata, (uint64_t)filedata, fileSize,
            MMU_RW | MMU_PRESENT | MMU_USER);
        addThread(thread);
        join(thread->pid);
        return strcat(strcat("Executable: '", options[0]), "' executed successfully");
    }
    else
    {
        return strcat(strcat("Could not find executable '", options[0]), "'");
    }
    return NULL;
}