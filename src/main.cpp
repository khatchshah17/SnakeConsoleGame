#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BLOCK_SIZE = 20;

struct Point {
    int x, y;
};

class SnakeGame {
public:
    SnakeGame() : window(nullptr), renderer(nullptr), running(true), direction({1, 0}) {
        srand(static_cast<unsigned>(time(0)));
    }

    ~SnakeGame() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
            return false;
        }

        resetGame();
        return true;
    }

    void run() {
        while (running) {
            handleEvents();
            update();
            render();
            SDL_Delay(100);
        }
    }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    std::vector<Point> snake;
    Point food;
    Point direction;

    void resetGame() {
        snake = {{5, 5}};
        spawnFood();
    }

    void spawnFood() {
        food.x = rand() % (SCREEN_WIDTH / BLOCK_SIZE);
        food.y = rand() % (SCREEN_HEIGHT / BLOCK_SIZE);
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: if (direction.y == 0) direction = {0, -1}; break;
                    case SDLK_DOWN: if (direction.y == 0) direction = {0, 1}; break;
                    case SDLK_LEFT: if (direction.x == 0) direction = {-1, 0}; break;
                    case SDLK_RIGHT: if (direction.x == 0) direction = {1, 0}; break;
                }
            }
        }
    }

    void update() {
        Point newHead = {snake[0].x + direction.x, snake[0].y + direction.y};

        if (newHead.x < 0 || newHead.y < 0 || newHead.x >= SCREEN_WIDTH / BLOCK_SIZE || newHead.y >= SCREEN_HEIGHT / BLOCK_SIZE) {
            running = false;
            return;
        }

        for (const auto& segment : snake) {
            if (newHead.x == segment.x && newHead.y == segment.y) {
                running = false;
                return;
            }
        }

        snake.insert(snake.begin(), newHead);

        if (newHead.x == food.x && newHead.y == food.y) {
            spawnFood();
        } else {
            snake.pop_back();
        }
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (const auto& segment : snake) {
            SDL_Rect rect = {segment.x * BLOCK_SIZE, segment.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect foodRect = {food.x * BLOCK_SIZE, food.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &foodRect);

        SDL_RenderPresent(renderer);
    }
};

int main(int argc, char* argv[]) {
    SnakeGame game;
    if (!game.init()) {
        return -1;
    }

    game.run();
    return 0;
}
