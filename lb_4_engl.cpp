#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <ctime>

constexpr unsigned int NUMBER = 30;
constexpr unsigned int MIN = -10;
constexpr unsigned int MAX = 10;
constexpr int MIN_Y = -7;

const char *STATUS[] = {"exists ", "disappear"};

class Ball {
public:
    Ball(unsigned int number, unsigned int time) {
        num = number;
        seed = time;

        switch (process = fork()) {
            case -1:
                std::cout << "Failed to create process\n";
                break;
            case 0:
                std::cout <<  "Created process " << getpid() << std::endl;
                while (move()) {
                    sleep(1);
                }
                exit(1);
        }
    }

    ~Ball() {
        if (process != -1) {
            wait(&process);
            std::cout << "Process #" << num << " was completed with code: " << STATUS[WIFEXITED(process)] << std::endl;
        }
    }
    pid_t getPID() const { return process; }

private:
    int x = 0;
    int y = 0;
    pid_t process = -1;
    unsigned int num = 0;
    unsigned int seed = 0;

    bool move() {
        x = randomInt();
        y = randomInt();
       // std::cout << x << ":" << y << std::endl;
        return y > MIN_Y;
    }

    int randomInt() {
        srand(seed);
        return rand() % MAX + MIN;
    }
};

class BallsVector : public std::vector<Ball *> {
public:
    explicit BallsVector(unsigned int size = 0) { this->reserve(size); };

    ~BallsVector() {
        for (int i = 0; i < this->size(); i++) {
            Ball *b = this->operator[](i);
            kill(this->operator[](i)->getPID(), SIGKILL);
            delete this->operator[](i);
        }
    }
};

int main() {
    BallsVector balls(NUMBER);
    for (unsigned int i = 0; i < NUMBER; ++i) {
        balls.emplace_back(new Ball(i, i + time(0)));
    }
    sleep(3);
    return 0;
}
