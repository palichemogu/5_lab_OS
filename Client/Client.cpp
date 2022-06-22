#include <iostream>
#include <fstream>
#include <sstream>
#include <conio.h>
#include "windows.h"
#include "constants.h"
#include "errors.h"
#include "employee.h"
using namespace std;

HANDLE hReadyEvent;
HANDLE hStartAllEvent;
HANDLE hPipe;

bool prepareWin32(int processID)
{
    hStartAllEvent = OpenEvent(SYNCHRONIZE, FALSE, startAllEventName.c_str());

    ostringstream handleName;
    handleName << creatorName << "_ready_" << processID;
    hReadyEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, &handleName.str()[0]);

    if (hStartAllEvent == NULL || hReadyEvent == NULL)
    {
        logErrorWin32("OpenEvent failed!");
        return false;
    }
    return true;
}

bool connect()
{
    while (true)
    {
        hPipe = CreateFile(
                pipeName.c_str(),
                GENERIC_READ |
                GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            logErrorWin32("Could not open pipe.");
            return false;
        }

        if ( ! WaitNamedPipe(pipeName.c_str(), 20000))
        {
            printf("Could not open pipe: 20 second wait timed out.");
            return false;
        }
    }
    return true;
}

void processingFunc()
{
    bool readSuccess = false;
    bool sendSuccess = false;
    Employee emp;

    while (true)
    {
        DWORD cbRead;
        DWORD cbWritten;

        int selection;
        bool selectionOK = false;
        do
        {
            cout << "What to do?" << endl;
            cout << "1. Read entry" << endl;
            cout << "2. Modify entry" << endl;
            cout << "3. Exit" << endl;
            cout << "Your selection : ";
            cin >> selection;
            if (selection < 1 || selection > 3) cout << "Wrong selection! Try again." << endl;
            else selectionOK = true;
        } while(!selectionOK);
        if (selection == 3)
        {
            consoleMessage("Bye! o/");
            break;
        }

        ostringstream message;
        int id;

        cout << "Enter id : ";
        cin >> id;

        if (selection == 1) message << "r " << id;
        else message << "w " << id;

        char command[MESSAGE_MAX_SIZE];
        strcpy(command, &message.str()[0]);

        sendSuccess = WriteFile(hPipe, command, MESSAGE_MAX_SIZE, &cbRead, NULL);
        if (!sendSuccess)
        {
            logErrorWin32("Failed to send the command!");
            break;
        }
        readSuccess = ReadFile(hPipe, &emp, sizeof(Employee), &cbRead, NULL);
        if (!readSuccess)
        {
            logErrorWin32("Failed to read server's answer");
            break;
        }

        if (emp.id == -1)
        {
            consoleMessage("The entry doesn't exist or is blocked.");
            continue;
        }

        emp.print(cout);

        if (selection == 1)
        {
            consoleMessage("Press any key to send cancel message to the server.");
            system("pause");
            message.str("");
            message.clear();
            message << "c " << id;
            strcpy(command, &message.str()[0]);
            sendSuccess = WriteFile(hPipe, command, MESSAGE_MAX_SIZE, &cbWritten, NULL);
            if (!sendSuccess)
            {
                logErrorWin32("Failed to send the command!");
                break;
            }
        }
        if (selection == 2)
        {
            consoleMessage("Enter new info : ");
            cout << "ID : ";
            cin >> emp.id;
            cout << "Name : ";
            cin >> emp.name;
            cout << "Working hours : ";
            cin >> emp.hours;
            sendSuccess = WriteFile(hPipe, &emp, sizeof(Employee), &cbWritten, NULL);
            if (!sendSuccess)
            {
                logErrorWin32("Failed to send the message!");
                break;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int id = atoi(argv[0]);
    bool prepareWin32OK = prepareWin32(id);
    if (!prepareWin32OK)
    {
        consoleMessage("Something went wrong while preparing Win32 stuff!");
        return -1;
    }

    consoleMessage("I'm ready!");
    SetEvent(hReadyEvent);
    WaitForSingleObject(hStartAllEvent, INFINITE);
    consoleMessage("Received event to start.");
    consoleMessage("Trying to connect...");
    bool connectOK = connect();
    if (!connectOK)
    {
        consoleMessage("Failed to connect :(");
        return -1;
    }
    else consoleMessage("I am connected now.");

    processingFunc();
    return 0;
}