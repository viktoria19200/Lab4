#include <windows.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <cstdio>

const std::wstring LARGE_FILE_PATH = L"C:\\largefile.bin";
const size_t BUFFER_SIZE = 1024 * 1024; // 1 МБ

// Читання через стандартні функції C
double ReadFileCLibrary() {
    auto start = std::chrono::high_resolution_clock::now();

    FILE* file = _wfopen(LARGE_FILE_PATH.c_str(), L"rb");
    if (!file) {
        std::wcerr << L"Помилка відкриття файлу" << std::endl;
        return -1;
    }

    std::vector<char> buffer(BUFFER_SIZE);
    size_t bytesRead;
    while ((bytesRead = fread(buffer.data(), 1, BUFFER_SIZE, file)) > 0) {
        // Симуляція обробки даних
    }

    fclose(file);

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

// Читання через Windows API
double ReadFileWindowsAPI() {
    auto start = std::chrono::high_resolution_clock::now();

    HANDLE hFile = CreateFile(
        LARGE_FILE_PATH.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Помилка відкриття файлу" << std::endl;
        return -1;
    }

    std::vector<char> buffer(BUFFER_SIZE);
    DWORD bytesRead;
    while (ReadFile(
        hFile,
        buffer.data(),
        BUFFER_SIZE,
        &bytesRead,
        NULL
    ) && bytesRead > 0) {
        // Симуляція обробки даних
    }

    CloseHandle(hFile);

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}
int main()
{
    const int ITERATIONS = 5;
    double cLibTotal = 0, windowsAPITotal = 0;

    for (int i = 0; i < ITERATIONS; ++i) {
        cLibTotal += ReadFileCLibrary();
        windowsAPITotal += ReadFileWindowsAPI();
    }

    std::wcout << L"Середній час читання (C бібліотека): "
        << cLibTotal / ITERATIONS << L" сек" << std::endl;
    std::wcout << L"Середній час читання (Windows API): "
        << windowsAPITotal / ITERATIONS << L" сек" << std::endl;

    return 0;
}

