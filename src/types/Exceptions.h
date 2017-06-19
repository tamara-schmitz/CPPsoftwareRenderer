#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>

// contains project specific exceptions

class FileNotFound_error : public std::runtime_error
{
    std::string what_message;
    public:
        const char* what() override
        {
            return what_message.c_str();
        }
};

#endif // EXCEPTIONS_H
