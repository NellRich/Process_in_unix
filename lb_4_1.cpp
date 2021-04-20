#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <ctime>

constexpr unsigned int NUMBER = 20;
constexpr unsigned int MIN = -10;
constexpr unsigned int MAX = 10;
constexpr int MIN_Y = -7;

const char *STATUS[] = {"не упал", "упал"};

class Ball {
public:
    Ball(unsigned int number, unsigned int time) {
        num = number;  // Записываем порядковый номер объекта
        seed = time;  // Генерируем зерно рандома для каждого объекта отдельно

        switch (process = fork()) {  // Пытаемся создать процесс
            case -1:
                std::cout << "Не удалось создать процесс\n";
                break;
            case 0:
                std::cout << "Создан процесс " << getpid() << std::endl;
                while (move()) {  // Если мяч упал - прерываем цикл
                    sleep(1);
                }
                exit(1);  // Будет этот код, если мячик успел упасть
        }
    }

    ~Ball() {
        if (process != -1) {
            wait(&process);  // Подождать завершения процесса
            std::cout << "Процесс #" << num << " был завершён с кодом: " << STATUS[WIFEXITED(process)] << std::endl;
        }
    }
    pid_t getPID() const { return process; }  // Получить PID процесса

private:
    int x = 0;
    int y = 0;
    pid_t process = -1;
    unsigned int num = 0;
    unsigned int seed = 0;

    bool move() {  // Перемещение в случайную сторону
        x = randomInt();
        y = randomInt();
        std::cout << x << ":" << y << std::endl; // Вывод координат броска
        return y > MIN_Y;  // Возвращает true для продолжения цикла, если есть необходимость его продолжать
    }

    int randomInt() {  // Генерация случайного значения с использованием зерна этого объекта
        srand(seed);
        return rand() % MAX + MIN;
    }
};

class BallsVector : public std::vector<Ball *> {  // Специальный вектор-контейнер для мячиков
public:
    explicit BallsVector(unsigned int size = 0) { this->reserve(size); };

    ~BallsVector() {
        for (int i = 0; i < this->size(); i++) {
            Ball *b = this->operator[](i);
            kill(this->operator[](i)->getPID(), SIGKILL);  // Убиваем процесс перед очищением памяти
            delete this->operator[](i);
        }
    }
};

int main() {
    BallsVector balls(NUMBER);
    for (unsigned int i = 0; i < NUMBER; ++i) {  // Заполняем вектор шарками
        balls.emplace_back(new Ball(i, i + time(0)));
    }
    sleep(3);  // Даём время на выполнение команды
    return 0;
}
