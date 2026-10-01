#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <cstddef>
#include <cstdint>

#include <iostream>

#define PI 3.1415926535

#define MAP_WIDTH 16
#define MAP_HEIGTH 16

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define INITIAL_PLAYER_X MAP_WIDTH / 2
#define INITIAL_PLAYER_Y MAP_HEIGTH / 2

#define FOV 120.0

const char map[MAP_HEIGTH][MAP_WIDTH + 1] = {
    "################",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "#              #",
    "################"
};

constexpr double powerOf(double x, uint16_t power) {
    double product = 1;
    for (uint16_t i = 0; i < power; ++i) { product *= x; }
    return product;
}

constexpr double factorial(int32_t n) {
    double product = 1;
    if (n == 0) { return product; }
    if (n < 0) { return -1; }
    if (n >= 1) {
        while (n > 1) {
            product *= n * (n - 1);
            n -= 2;
        }
    }
    return product;
}

constexpr double cosine(double angle) {
    double radians = angle * (PI / 180);
    return 1
    - (powerOf(radians, 2) / factorial(2))
    + (powerOf(radians, 4) / factorial(4))
    - (powerOf(radians, 6) / factorial(6))
    + (powerOf(radians, 8) / factorial(8))
    - (powerOf(radians, 10) / factorial(10))
    + (powerOf(radians, 12) / factorial(12))
    - (powerOf(radians, 14) / factorial(14))
    + (powerOf(radians, 16) / factorial(16))
    - (powerOf(radians, 18) / factorial(18))
    + (powerOf(radians, 20) / factorial(20))
    - (powerOf(radians, 22) / factorial(22))
    + (powerOf(radians, 24) / factorial(24))
    - (powerOf(radians, 26) / factorial(26))
    + (powerOf(radians, 28) / factorial(28))
    - (powerOf(radians, 30) / factorial(30));
}

constexpr double sinus(double angle) {
    double radians = angle * (PI / 180);
    return radians
    - (powerOf(radians, 3) / factorial(3))
    + (powerOf(radians, 5) / factorial(5))
    - (powerOf(radians, 7) / factorial(7))
    + (powerOf(radians, 9) / factorial(9))
    - (powerOf(radians, 11) / factorial(11))
    + (powerOf(radians, 13) / factorial(13))
    - (powerOf(radians, 15) / factorial(15))
    + (powerOf(radians, 17) / factorial(17))
    - (powerOf(radians, 19) / factorial(19))
    + (powerOf(radians, 21) / factorial(21))
    - (powerOf(radians, 23) / factorial(23))
    + (powerOf(radians, 25) / factorial(25))
    - (powerOf(radians, 27) / factorial(27))
    + (powerOf(radians, 29) / factorial(29))
    - (powerOf(radians, 31) / factorial(31));
}

class GraphicalUserInterface {
    private:
        GLFWwindow* window;
        uint8_t playerX;
        uint8_t playerY;
        double playerAngle;
        bool leftWasPressed;
        bool rightWasPressed;
        bool wWasPressed;
        struct Coordinate2D {
            double xCoordinate;
            double yCoordinate;
            double distanceBetweenEndpointAndPlayer;
        };
        void initialize(void) {
            glfwInit();
        }
        void openWindow(const uint16_t width, const uint16_t height) {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            window = glfwCreateWindow(width, height, "TestWindow", NULL, NULL);
            glfwMakeContextCurrent(window);
            glfwSwapInterval(1);
            glViewport(0, 0, width, height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, width, 0, height, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        void movePlayer(int8_t x, int8_t y) {
            if (map[playerY + y][playerX + x] != '#') {
                playerX += x;
                playerY += y;
            }
        }
        Coordinate2D castRay(double rayAngle) {
            double rayX = playerX;
            double rayY = playerY;
            double directionX = cosine(rayAngle);
            double directionY = sinus(rayAngle);
            double distance = 0;
            Coordinate2D endpoint;
            while (true) {
                rayX += directionX;
                rayY += directionY;
                distance += 1;
                if (map[static_cast<int>(rayY)][static_cast<int>(rayX)] == '#') {
                    endpoint = { .xCoordinate = rayX, .yCoordinate = rayY, .distanceBetweenEndpointAndPlayer = distance };
                    return endpoint;
                }
            }
        }
        void castRays(void) {
            double rayAngleMin = playerAngle - (FOV / 2);
            double angleStep = FOV / SCREEN_WIDTH;
            glBegin(GL_LINES);
            for (uint16_t i = 0; i < SCREEN_WIDTH; ++i) {
                double rayAngle = rayAngleMin + (i * angleStep);
                Coordinate2D endpoint = castRay(rayAngle);
                double wallHeight = SCREEN_HEIGHT / endpoint.distanceBetweenEndpointAndPlayer;
                double wallBottom = (SCREEN_HEIGHT / 2.0) - (wallHeight / 2.0);
                double wallTop = (SCREEN_HEIGHT / 2.0) + (wallHeight / 2.0);
                glVertex2d(i, wallBottom);
                glVertex2d(i, wallTop);
            }
            glEnd();
        }
        void drawFloorDots(void) {
            glPointSize(3);
            glBegin(GL_POINTS);
            for (uint16_t y = 0; y < SCREEN_HEIGHT / 2; y += 20) {
                for (uint16_t x = 0; x < SCREEN_WIDTH; x += 20) {
                    glVertex2d(x, y);
                }
            }
            glEnd();
        }
    public:
        void run(void) {
            playerX = INITIAL_PLAYER_X;
            playerY = INITIAL_PLAYER_Y;
            playerAngle = 0;
            leftWasPressed = false;
            rightWasPressed = false;
            wWasPressed = false;
            initialize();
            openWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT);
                drawFloorDots();
                castRays();
                glfwSwapBuffers(window);
                bool leftPressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
                bool rightPressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
                bool wPressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
                else if (leftPressed && !leftWasPressed) { playerAngle -= 45; if (playerAngle < 0) { playerAngle += 360; } }
                else if (rightPressed && !rightWasPressed) { playerAngle += 45; if (playerAngle >= 360) { playerAngle -= 360; } }
                else if (wPressed && !wWasPressed) {
                    double directionX = cosine(playerAngle);
                    double directionY = sinus(playerAngle);
                    int8_t moveX = 0;
                    int8_t moveY = 0;
                    if (directionX > 0.5) { moveX = 1; }
                    else if (directionX < -0.5) { moveX = -1; }
                    if (directionY > 0.5) { moveY = 1; }
                    else if (directionY < -0.5) { moveY = -1; }
                    movePlayer(moveX, moveY);
                }
                leftWasPressed = leftPressed;
                rightWasPressed = rightPressed;
                wWasPressed = wPressed;
            }
        }
};

int main(void) {
    GraphicalUserInterface GUI{};
    GUI.run();
    return 0;
}
