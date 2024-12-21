#include "raylib.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

// Function to get a random code snippet and its correct input
struct CodeChallenge {
    std::string question;
    std::string answer;
};

// Define categories of questions
std::vector<CodeChallenge> basicArithmetic = {
    {"What is 5 + 3?", "8"},
    {"What is 12 - 4?", "8"},
    {"What is 7 * 2?", "14"},
    {"What is 9 / 3?", "3"},
    {"What is 4 + 6?", "10"},
    {"What is 15 - 7?", "8"},
    {"What is 3 * 5?", "15"},
    {"What is 18 / 6?", "3"},
    {"What is 20 - 12?", "8"},
    {"What is 6 * 6?", "36"}
};

std::vector<CodeChallenge> incrementDecrement = {
	{"x = 5; x++; x?", "6"},
	{"y = 7; y--; y?", "6"},
	{"z = 3; ++z; z?", "4"},
	{"a = 9; --a; a?", "8"},
	{"b = 10; b += 5; b?", "15"},
	{"c = 4; c -= 2; c?", "2"},
	{"d = 2; d *= 3; d?", "6"},
	{"e = 8; e /= 4; e?", "2"},
	{"f = 1; f++; f++; f?", "3"},
	{"g = 10; --g; --g; g?", "8"}
};

std::vector<CodeChallenge> squareRoot = {
    {"What is sqrt(4)?", "2"},
    {"What is sqrt(9)?", "3"},
    {"What is sqrt(16)?", "4"},
    {"What is sqrt(25)?", "5"},
    {"What is sqrt(36)?", "6"},
    {"What is sqrt(49)?", "7"},
    {"What is sqrt(64)?", "8"},
    {"What is sqrt(81)?", "9"},
    {"What is sqrt(100)?", "10"},
    {"What is sqrt(144)?", "12"}
};

std::vector<CodeChallenge> asciiValues = {
	{"ASCII of 'A'?", "65"},
	{"ASCII of 'B'?", "66"},
	{"ASCII of 'Z'?", "90"},
	{"ASCII of 'a'?", "97"},
	{"ASCII of 'b'?", "98"},
	{"ASCII of 'z'?", "122"},
	{"ASCII of '0'?", "48"},
	{"ASCII of '1'?", "49"},
	{"ASCII of '9'?", "57"},
	{"ASCII of '+'?", "43"},
	{"ASCII of 'C'?", "67"},
	{"ASCII of 'D'?", "68"},
	{"ASCII of 'E'?", "69"},
	{"ASCII of 'f'?", "102"},
	{"ASCII of 'g'?", "103"},
	{"ASCII of 'H'?", "72"},
	{"ASCII of '!'?", "33"},
	{"ASCII of '@'?", "64"},
	{"ASCII of '#'?", "35"},
	{"ASCII of '$'?", "36"}
};

// Function to get a random code challenge from a given category
CodeChallenge GetRandomCodeChallenge(const std::vector<CodeChallenge>& categoryChallenges) {
    return categoryChallenges[rand() % categoryChallenges.size()];
}

// Function to spawn pipes
void SpawnObstacles(std::vector<Rectangle>& obstacles, int screenWidth, int screenHeight, float gapHeight, float pipeWidth, float pipeHeight, float pipeScale) {
    int scaledPipeHeight = (int)(pipeHeight * pipeScale);
    int topPipeHeight = GetRandomValue(50, screenHeight - gapHeight - 200);
    int bottomPipeHeight = screenHeight - topPipeHeight - gapHeight;

    obstacles.push_back(Rectangle{ (float)screenWidth, 0, pipeWidth * pipeScale, (float)topPipeHeight });
    obstacles.push_back(Rectangle{ (float)screenWidth, (float)(screenHeight - bottomPipeHeight), pipeWidth * pipeScale, (float)scaledPipeHeight });
}

int main() {
    // Initialize Raylib
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Code Input Flappy Bird Game");

    InitAudioDevice();
    Music backgroundMusic = LoadMusicStream("game.mp3");
    Sound jumpSound = LoadSound("jump.mp3");
    Sound gameOverSound = LoadSound("over.mp3");
    Sound startSound = LoadSound("start.mp3");

    Texture2D topPillar = LoadTexture("top.png");
    Texture2D bottomPillar = LoadTexture("bottom.png");
    Texture2D birdTexture1 = LoadTexture("Bird_01.png");
    Texture2D birdTexture2 = LoadTexture("Bird_02.png");
    Texture2D birdTexture3 = LoadTexture("Bird_03.png");
    Texture2D birdTextures[3] = { birdTexture1, birdTexture2, birdTexture3 };
    Texture2D background = LoadTexture("bg.png");

    PlayMusicStream(backgroundMusic);

    Rectangle character = { 100, screenHeight / 2 - 75, 50, 50 };
    std::vector<Rectangle> obstacles;

    int score = 0;
    bool isJumping = false;
    float gravity = 0.15f;
    float velocityY = 0;
    char userInput[20] = { '\0' };
    int inputIndex = 0;
    bool gameOver = false;
    int highestScore = 0;

    float spawnTimer = 0.0f;
    float spawnInterval = 2.5f;

    float pipeWidth = 26;
    float pipeHeight = 160;
    float pipeScale = 1.5f; // Scale factor for pipes

    int frameCounter = 0;
    int currentFrame = 0;
    const int maxFrames = 3;

    float backgroundScale = (float)screenHeight / background.height;
    int numBackgrounds = (int)ceil((float)screenWidth / (background.width * backgroundScale));
    float backgroundX = 0.0f;

    if (birdTexture1.id == 0 || birdTexture2.id == 0 || birdTexture3.id == 0 || background.id == 0) {
        printf("Error loading textures\n");
        return -1;
    }

    // Menu to select question category
    int selectedCategory = -1;
    while (selectedCategory == -1 && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Select a Question Category:", screenWidth / 2 - 150, 100, 20, DARKGRAY);
        DrawText("1. Basic Arithmetic", screenWidth / 2 - 150, 150, 20, DARKGRAY);
        DrawText("2. Increment/Decrement", screenWidth / 2 - 150, 180, 20, DARKGRAY);
        DrawText("3. Square Roots", screenWidth / 2 - 150, 210, 20, DARKGRAY);
        DrawText("4. ASCII Values", screenWidth / 2 - 150, 240, 20, DARKGRAY);
        DrawText("Press the corresponding number key to choose.", screenWidth / 2 - 200, 300, 20, DARKGRAY);

        if (IsKeyPressed(KEY_ONE)) selectedCategory = 1;
        if (IsKeyPressed(KEY_TWO)) selectedCategory = 2;
        if (IsKeyPressed(KEY_THREE)) selectedCategory = 3;
        if (IsKeyPressed(KEY_FOUR)) selectedCategory = 4;

        EndDrawing();
    }

    std::vector<CodeChallenge>* chosenCategory = nullptr;
    switch (selectedCategory) {
        case 1: chosenCategory = &basicArithmetic; break;
        case 2: chosenCategory = &incrementDecrement; break;
        case 3: chosenCategory = &squareRoot; break;
        case 4: chosenCategory = &asciiValues; break;
        default: break;
    }

    if (!chosenCategory) return 0;

    CodeChallenge currentChallenge = GetRandomCodeChallenge(*chosenCategory);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        UpdateMusicStream(backgroundMusic);

        if (!gameOver) {
            spawnTimer += GetFrameTime();
            if (spawnTimer >= spawnInterval) {
                float gapHeight = GetRandomValue(150, 300);
                SpawnObstacles(obstacles, screenWidth, screenHeight, gapHeight, pipeWidth, pipeHeight, pipeScale);
                spawnTimer = 0.0f;
            }

            for (size_t i = 0; i < obstacles.size(); ++i) {
                obstacles[i].x -= 2;
            }

            if (!obstacles.empty() && obstacles[0].x < -pipeWidth * pipeScale) {
                obstacles.erase(obstacles.begin(), obstacles.begin() + 2);
                score++;
            }

            for (size_t i = 0; i < obstacles.size(); i++) {
                if (CheckCollisionRecs(character, obstacles[i])) {
                    gameOver = true;
                    PlaySound(gameOverSound);
                }
            }

            velocityY += gravity;
            character.y += velocityY;

            if (character.y >= screenHeight - 125) {
                character.y = screenHeight - 125;
            }

            if (isJumping) {
                velocityY = -8.0f;
                isJumping = false;
                PlaySound(jumpSound);
            }

            if (IsKeyPressed(KEY_BACKSPACE) && inputIndex > 0) {
                userInput[--inputIndex] = '\0';
            } else if (IsKeyPressed(KEY_ENTER)) {
                if (std::string(userInput) == currentChallenge.answer) {
                    isJumping = true;
                    currentChallenge = GetRandomCodeChallenge(*chosenCategory);
                }
                inputIndex = 0;
                userInput[0] = '\0';
            } else if (inputIndex < 19) {
                int key = GetKeyPressed();
                if (key >= 32 && key <= 126) {
                    userInput[inputIndex++] = (char)key;
                    userInput[inputIndex] = '\0';
                }
            }

            frameCounter++;
            if (frameCounter >= 10) {
                frameCounter = 0;
                currentFrame = (currentFrame + 1) % maxFrames;
            }

            backgroundX -= 0.5f;
            if (backgroundX <= -screenWidth) {
                backgroundX = 0.0f;
            }
        } else {
            if (score > highestScore) {
                highestScore = score;
            }

            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                score = 0;
                obstacles.clear();
                currentChallenge = GetRandomCodeChallenge(*chosenCategory);
                character.y = screenHeight / 2 - 75;
                velocityY = 0;
                userInput[0] = '\0';
                inputIndex = 0;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < numBackgrounds + 1; i++) {
            Vector2 backgroundPosition = { backgroundX + (i * background.width * backgroundScale), 0 };
            DrawTextureEx(background, backgroundPosition, 0.0f, backgroundScale, WHITE);
        }

        if (!gameOver) {
            float birdScale = 4.0f;
            DrawTextureEx(birdTextures[currentFrame], (Vector2){ character.x, character.y }, 0.0f, birdScale, WHITE);

            for (size_t i = 0; i < obstacles.size(); ++i) {
                DrawTexturePro(i % 2 == 0 ? topPillar : bottomPillar,
                               Rectangle{ 0, 0, pipeWidth, pipeHeight },
                               obstacles[i],
                               Vector2{ 0, 0 },
                               0.0f, WHITE);
            }

            DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
            DrawText("Code Challenge:", 500, 50, 20, DARKGRAY);
            DrawText(currentChallenge.question.c_str(), 500, 80, 20, DARKGRAY);
            DrawText("Your Answer:", 500, 120, 20, DARKGRAY);
            DrawText(userInput, 500, 150, 20, DARKGRAY);
        } else {
            DrawText("Game Over!", screenWidth / 2 - 80, screenHeight / 2 - 20, 40, RED);
            DrawText(TextFormat("Final Score: %d", score), screenWidth / 2 - 80, screenHeight / 2 + 30, 20, DARKGRAY);
            DrawText(TextFormat("Highest Score: %d", highestScore), screenWidth / 2 - 100, screenHeight / 2 + 60, 20, DARKGRAY);
            DrawText("Press R to Restart", screenWidth / 2 - 100, screenHeight / 2 + 90, 20, DARKGRAY);
        }

        EndDrawing();
    }

    UnloadMusicStream(backgroundMusic);
    UnloadTexture(birdTexture1);
    UnloadTexture(birdTexture2);
    UnloadTexture(birdTexture3);
    UnloadTexture(background);
    CloseWindow();

    return 0;
}

