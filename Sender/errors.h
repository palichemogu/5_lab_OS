//
// Created by Ivan on 22.06.2022.
//

#include <iostream>
#include "windows.h"
#include "constants.h"

#ifndef CLIENT_ERRORS_H
#define CLIENT_ERRORS_H

#define ERROR_PREFIX "[ERROR]"

std::string GetLastErrorAsString()
{
    DWORD errorId = GetLastError();

    if (errorId == 0)
    {
        return std::string();
    }

    LPSTR msgBuf = NULL;
    int size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorId,
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            (LPSTR)&msgBuf,
            0,
            NULL
    );

    std::string msg(msgBuf, size);

    LocalFree(msgBuf);

    return msg;
}

void logError(const char* title, const char* reason)
{
    std::cout << ERROR_PREFIX << ' ' << title << '\n';
    std::cout << "   |_  " << ' ' << reason << '\n';
}

void logErrorWin32(const char* title)
{
    logError(title, GetLastErrorAsString().c_str());
}

void consoleMessage(const char* message){
    std::cout << CONSOLE_PREFIX << ' ' << message << '\n';
}

#endif //CLIENT_ERRORS_H
