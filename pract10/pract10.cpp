// pract10.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>

const int count = 3;
bool loadRunning = false;

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
    }
    return 0;
}

DWORD WINAPI ThreadFib(LPVOID p) {
    ThreadTemplate* threadTemplate = (ThreadTemplate*)p;
    threadTemplate->running = true;
    threadTemplate->iterations = 0;

    SetThreadPriority(GetCurrentThread(), threadTemplate->priority);

    while (threadTemplate->running) {
        long long counter = 0;
        for (int i = 0; i < 1000; i++) {
            fib(i % 40);
        }
        threadTemplate->iterations += 1000;
    }
    return 0;
}

DWORD WINAPI ThreadFact(LPVOID p) {
    ThreadTemplate* threadTemplate = (ThreadTemplate*)p;
    threadTemplate->running = true;
    threadTemplate->iterations = 0;

    SetThreadPriority(GetCurrentThread(), threadTemplate->priority);

    while (threadTemplate->running) {
        long long counter = 0;
        for (int i = 0; i < 1000; i++) {
            fact(i % 20);

        }
        threadTemplate->iterations += 1000;
    }
    return 0;
}

DWORD WINAPI LoadThread(LPVOID p) {
    loadRunning = true;
    loadThreadIterations = 0;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    std::cout << "Поток нагрузчик запущен с максимальным приоритетом";

    DWORD start = GetTickCount64();
    while (GetTickCount64() - start < 3000) {
        for (int i = 0; i < 100000; i++) {
            fib(30);
            fact(15);
        }
        loadThreadIterations += 100000;
    }

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    std::cout << "Поток нагрузчик перешел в нормальный приоритет";

    while (loadRunning) {
        for (int i = 0; i < 10000; i++) {
            fib(25);
            fact(12);
        }
        loadThreadIterations += 10000;
    }
    return 0;
}


int main()
{
    
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
