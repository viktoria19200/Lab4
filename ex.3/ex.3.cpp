#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

const int NUM_FILES = 3;
const size_t BUFFER_SIZE = 1024 * 1024; // 1 МБ

struct FileContext {
    HANDLE hFile;
    OVERLAPPED overlapped;
    std::vector<char> buffer;
    std::wstring filename;
    DWORD bytesRead;
};

class AsyncFileProcessor {
private:
    std::vector<FileContext> files;
    std::vector<HANDLE> events;

public:
    AsyncFileProcessor() {
        // Ініціалізація файлів
        std::vector<std::wstring> filePaths = {
            L"file1.txt", L"file2.txt", L"file3.txt"
        };

        for (const auto& path : filePaths) {
            FileContext context;
            context.filename = path;
            context.hFile = CreateFile(
                path.c_str(),
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                NULL
            );

            if (context.hFile == INVALID_HANDLE_VALUE) {
                std::wcerr << L"Помилка відкриття файлу: " << path << std::endl;
                continue;
            }

            context.buffer.resize(BUFFER_SIZE);
            context.overlapped = {};
            context.overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            events.push_back(context.overlapped.hEvent);

            files.push_back(context);
        }
    }

    void ProcessFiles() {
        // Ініціювання асинхронного читання
        for (auto& file : files) {
            ReadFile(
                file.hFile,
                file.buffer.data(),
                BUFFER_SIZE,
                &file.bytesRead,
                &file.overlapped
            );
        }

        // Очікування та обробка завершення
        while (!events.empty()) {
            DWORD waitResult = WaitForMultipleObjects(
                events.size(),
                events.data(),
                FALSE,
                INFINITE
            );

            int index = waitResult - WAIT_OBJECT_0;
            auto& file = files[index];

            if (GetOverlappedResult(file.hFile, &file.overlapped, &file.bytesRead, FALSE)) {
                std::wcout << L"Прочитано " << file.bytesRead
                    << L" байт з файлу " << file.filename << std::endl;
            }

            // Закриття та видалення опрацьованого файлу
            CloseHandle(file.overlapped.hEvent);
            CloseHandle(file.hFile);
            files.erase(files.begin() + index);
            events.erase(events.begin() + index);
        }
    }

    ~AsyncFileProcessor() {
        for (auto& file : files) {
            CloseHandle(file.hFile);
            CloseHandle(file.overlapped.hEvent);
        }
    }
};

int main()
{
    try {
        AsyncFileProcessor processor;
        processor.ProcessFiles();
    }
    catch (const std::exception& e) {
        std::cerr << "Помилка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

