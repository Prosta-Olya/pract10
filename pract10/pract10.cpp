// pract10.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>

const int count = 3;
bool loadRunning = false;
bool programRunning = true;

struct ThreadTemplate { //шаблон потока
    int id;
    HANDLE handle;
    DWORD threadId;
    bool running;
    long long iterations;
    int priority;
    const char* name;
};

ThreadTemplate threads[count];
HANDLE loadThread = NULL;
DWORD loadThreadId = 0;
long long loadThreadIterations = 0;

long long fact(int n) { //вычисление факториала
    if (n <= 1) return 1;
    long long res = 1;
    for (int i = 2; i <= n; i++) {
        res *= i;
    }
    return res;
}
long long fib(int n) { //вычисление числа Фибоначчи
    if (n <= 1) return n;
    long long a = 0;
    long long b = 1;
    long long c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

DWORD WINAPI ThreadIncrement(LPVOID p) {
    ThreadTemplate* threadTemplate = (ThreadTemplate*)p;
    threadTemplate->running = true;
    threadTemplate->iterations = 0;

    SetThreadPriority(GetCurrentThread(), threadTemplate->priority);

    while (threadTemplate->running) {
        long long counter = 0;
        for (int i = 0; i < 100000; i++) {
            counter++;
        }
        threadTemplate->iterations += 100000;
        Sleep(5);
    }
    return 0;
}

DWORD WINAPI ThreadFib(LPVOID p) {
    ThreadTemplate* threadTemplate = (ThreadTemplate*)p;
    threadTemplate->running = true;
    threadTemplate->iterations = 0;

    SetThreadPriority(GetCurrentThread(), threadTemplate->priority);

    while (threadTemplate->running) {
        for (int i = 0; i < 3000; i++) {
            fib(i % 40);
        }
        threadTemplate->iterations += 3000;
        Sleep(5);
    }
    return 0;
}

DWORD WINAPI ThreadFact(LPVOID p) {
    ThreadTemplate* threadTemplate = (ThreadTemplate*)p;
    threadTemplate->running = true;
    threadTemplate->iterations = 0;

    SetThreadPriority(GetCurrentThread(), threadTemplate->priority);

    while (threadTemplate->running) {
        for (int i = 0; i < 5000; i++) {
            fact(i % 20);
        }
        threadTemplate->iterations += 5000;
        Sleep(5);
    }
    return 0;
}

DWORD WINAPI LoadThread(LPVOID p) {
    loadRunning = true;
    loadThreadIterations = 0;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    std::cout << "Поток нагрузчик запущен с максимальным приоритетом\n";

    DWORD start = GetTickCount64();
    while (GetTickCount64() - start < 3000) {
        for (int i = 0; i < 100000; i++) {
            fib(30);
            fact(15);
        }
        loadThreadIterations += 100000;
    }

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    std::cout << "Поток нагрузчик перешел в нормальный приоритет\n";

    while (loadRunning) {
        for (int i = 0; i < 10000; i++) {
            fib(25);
            fact(12);
        }
        loadThreadIterations += 10000;
        Sleep(10);
    }
    return 0;
}

void ShowStatistics() {
    static DWORD lastShowTime = 0;
    DWORD currentTime = GetTickCount64();

    if (currentTime - lastShowTime < 1000) {
        return;
    }
    lastShowTime = currentTime;

    std::cout << "Статистика потоков\n";
    std::cout << "Время: " << currentTime / 1000 << "сек\n";

    for (int i = 0; i < count; i++) {
        std::cout << "Поток " << threads[i].id << " (" << threads[i].name << "): " << threads[i].iterations << " итераций, приоритет: ";

        switch (threads[i].priority) {
        case THREAD_PRIORITY_IDLE: std::cout << "IDLE"; break;
        case THREAD_PRIORITY_LOWEST: std::cout << "LOWEST"; break;
        case THREAD_PRIORITY_BELOW_NORMAL: std::cout << "BELOW_NORMAL"; break;
        case THREAD_PRIORITY_NORMAL: std::cout << "NORMAL"; break;
        case THREAD_PRIORITY_ABOVE_NORMAL: std::cout << "ABOVE_NORMAL"; break;
        case THREAD_PRIORITY_HIGHEST: std::cout << "HIGHEST"; break;
        case THREAD_PRIORITY_TIME_CRITICAL: std::cout << "TIME_CRITICAL"; break;
        default: std::cout << "UNKNOWN"; break;
        }
        std::cout << "\n";
    }

    if (loadRunning) {
        std::cout << "Нагрузчик: " << loadThreadIterations << " итераций\n";
    }

    std::cout << "\n";
}

DWORD WINAPI StatisticsThread(LPVOID p) {
    while (programRunning) {
        ShowStatistics();
        Sleep(10000);
    }
    return 0;
}

void ChangeThreadPriority(int threadId, int newPriority) {
    for (int i = 0; i < count; i++) {
        if (threads[i].id == threadId) {
            if (SetThreadPriority(threads[i].handle, newPriority)) {
                threads[i].priority = newPriority;
                std::cout << "Приоритет потока " << threadId << " изменен на: ";

                switch (newPriority) {
                case THREAD_PRIORITY_IDLE: std::cout << "IDLE"; break;
                case THREAD_PRIORITY_LOWEST: std::cout << "LOWEST"; break;
                case THREAD_PRIORITY_BELOW_NORMAL: std::cout << "BELOW_NORMAL"; break;
                case THREAD_PRIORITY_NORMAL: std::cout << "NORMAL"; break;
                case THREAD_PRIORITY_ABOVE_NORMAL: std::cout << "ABOVE_NORMAL"; break;
                case THREAD_PRIORITY_HIGHEST: std::cout << "HIGHEST"; break;
                case THREAD_PRIORITY_TIME_CRITICAL: std::cout << "TIME_CRITICAL"; break;
                default: std::cout << "UNKNOWN"; break;
                }
                std::cout << "\n";
            }
            else {
                std::cout << "Ошибка при изменении приоритета!\n";
            }
            return;
        }
    }
    std::cout << "Поток с ID " << threadId << " не найден!\n";
}

void StartWorkerThreads() {
    threads[0] = { 1, NULL, 0, false, 0, THREAD_PRIORITY_BELOW_NORMAL, "Инкремент" };
    threads[1] = { 2, NULL, 0, false, 0, THREAD_PRIORITY_NORMAL, "Фибоначчи" };
    threads[2] = { 3, NULL, 0, false, 0, THREAD_PRIORITY_ABOVE_NORMAL, "Факториал" };

    threads[0].handle = CreateThread(NULL, 0, ThreadIncrement, &threads[0], 0, &threads[0].threadId);
    threads[1].handle = CreateThread(NULL, 0, ThreadFib, &threads[1], 0, &threads[1].threadId);
    threads[2].handle = CreateThread(NULL, 0, ThreadFact, &threads[2], 0, &threads[2].threadId);

    std::cout << "Рабочие потоки запущены со стартовыми приоритетами:\n";
    std::cout << "Поток 1 (Инкремент): BELOW_NORMAL\n";
    std::cout << "Поток 2 (Фибоначчи): NORMAL\n";
    std::cout << "Поток 3 (Факториал): ABOVE_NORMAL\n";
}

void Cleanup() {
    programRunning = false;
    loadRunning = false;

    for (int i = 0; i < count; i++) {
        threads[i].running = false;
        if (threads[i].handle) {
            WaitForSingleObject(threads[i].handle, 1000);
            CloseHandle(threads[i].handle);
        }
    }

    if (loadThread) {
        WaitForSingleObject(loadThread, 1000);
        CloseHandle(loadThread);
    }
}

int main() {
    setlocale(0, "rus");

    std::cout << "Управление приоритетами потоков\n";

    StartWorkerThreads();

    HANDLE statsThread = CreateThread(NULL, 0, StatisticsThread, NULL, 0, NULL);
    while (programRunning) {
        std::cout << "Меню управления\n";
        std::cout << "1 - Изменить приоритет потока\n";
        std::cout << "2 - Запустить поток-нагрузчик\n";
        std::cout << "3 - Показать статистику сейчас\n";
        std::cout << "0 - Выход\n";

        int menu;
        std::cin >> menu;

        switch (menu) {
        case 1: {
            std::cout << "Введите ID потока (1-3): ";
            int threadId;
            std::cin >> threadId;

            std::cout << "Выберите приоритет:\n";
            std::cout << "1 - IDLE\n";
            std::cout << "2 - LOWEST\n";
            std::cout << "3 - BELOW_NORMAL\n";
            std::cout << "4 - NORMAL\n";
            std::cout << "5 - ABOVE_NORMAL\n";
            std::cout << "6 - HIGHEST\n";
            std::cout << "7 - TIME_CRITICAL\n";

            int priority;
            std::cin >> priority;

            int newPriority;
            switch (priority) {
            case 1: newPriority = THREAD_PRIORITY_IDLE; break;
            case 2: newPriority = THREAD_PRIORITY_LOWEST; break;
            case 3: newPriority = THREAD_PRIORITY_BELOW_NORMAL; break;
            case 4: newPriority = THREAD_PRIORITY_NORMAL; break;
            case 5: newPriority = THREAD_PRIORITY_ABOVE_NORMAL; break;
            case 6: newPriority = THREAD_PRIORITY_HIGHEST; break;
            case 7: newPriority = THREAD_PRIORITY_TIME_CRITICAL; break;
            default: newPriority = THREAD_PRIORITY_NORMAL; break;
            }

            ChangeThreadPriority(threadId, newPriority);
            break;
        }
        case 2:
            if (!loadRunning) {
                loadThread = CreateThread(NULL, 0, LoadThread, NULL, 0, &loadThreadId);
                std::cout << "Поток-нагрузчик запущен!\n";
            }
            else {
                std::cout << "Поток-нагрузчик уже работает!\n";
            }
            break;
        case 3:
            ShowStatistics();
            break;
        case 0:
            programRunning = false;
            break;
        default:
            std::cout << "Неверный выбор!\n";
            break;
        }
    }

    Cleanup();
    WaitForSingleObject(statsThread, 1000);
    CloseHandle(statsThread);

    std::cout << "Программа завершена\n";
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
