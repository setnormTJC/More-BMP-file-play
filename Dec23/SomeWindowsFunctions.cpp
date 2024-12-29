#include "SomeWindowsFunctions.h"
#include <fstream>
#include <queue>
#include <mutex>

HHOOK mouseHook;
HWND targetWindow;
POINT lastClickCoordinates;
std::atomic<bool> running(true);
std::queue<POINT> clickQueue;
std::mutex queueMutex;
std::condition_variable queueCV;

void workerThread()
{
    while (running)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCV.wait(lock, [] { return !clickQueue.empty() || !running; });

        while (!clickQueue.empty())
        {
            POINT pt = clickQueue.front();
            clickQueue.pop();
            lock.unlock();

            // Write coordinates to file
            writeCoordinatesToFile(pt, "click_coordinates.txt");

            lock.lock();
        }
    }
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_LBUTTONDOWN)
    {
        MSLLHOOKSTRUCT* mouseStruct = (MSLLHOOKSTRUCT*)lParam;

        if (mouseStruct != nullptr)
        {
            POINT pt = mouseStruct->pt;

            RECT rect;
            GetWindowRect(targetWindow, &rect);
            if (PtInRect(&rect, pt))
            {
                ScreenToClient(targetWindow, &pt); // updates pt values
                // ex: if window top left is at screen's {100, 100} coordinate

                lastClickCoordinates = pt;

                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    clickQueue.push(pt);
                }
                queueCV.notify_one();

                cout << "Mouse click at (" << pt.x << " , " << pt.y << ")\n";
            }
            else
            {
                cout << "Point not in window \n";
            }
        }
        else
        {
            cout << "Mouse struct is nullptr\n";
        }
    }
    else
    {
        cout << "MouseProc not processing LBUTTONDOWN\n";
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

void setMouseHook(HWND hwnd)
{
    targetWindow = hwnd;

    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, nullptr, 0);

    if (mouseHook == nullptr)
    {
        cout << "Failed to set mouse hook\n";
    }
    else
    {
        cout << "Mouse hook set \n";
        std::thread(workerThread).detach();
    }
}

void removeMouseHook()
{
    if (mouseHook != nullptr)
    {
        UnhookWindowsHookEx(mouseHook);
        mouseHook = nullptr;
        cout << "Mouse hook removed\n";
    }

    running = false;
    queueCV.notify_all();
}

POINT getLastClickCoordinates()
{
    return lastClickCoordinates;
}

void writeCoordinatesToFile(const POINT& pt, const std::string& filename)
{
    std::ofstream outFile;
    outFile.open(filename, std::ios_base::app); // Append mode
    if (outFile.is_open())
    {
        outFile << "Mouse click at (" << pt.x << ", " << pt.y << ")\n";
        outFile.close();
    }
    else
    {
        cout << "Failed to open file for writing\n";
    }
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    vector<HWND>& windows = *reinterpret_cast<vector<HWND>*>(lParam);

    windows.push_back(hwnd);

    return TRUE;
}

vector<HWND> getAllOpenWindows()
{
    vector<HWND> windows;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));

    return windows;
}

string getWindowTitle(HWND hwnd)
{
    char title[256];

    GetWindowTextA(hwnd, title, sizeof(title));

    return string(title);
}

HWND getWindowByTitle(const string& desiredTitle)
{
    vector<HWND> windows = getAllOpenWindows();

    for (auto currentWindow : windows)
    {
        if (getWindowTitle(currentWindow) == desiredTitle)
        {
            return currentWindow;
        }
    }

    return nullptr;
}