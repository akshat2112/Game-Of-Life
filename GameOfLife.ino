#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define WIDTH 8
#define HEIGHT 8
#define BRIGHTNESS 5
#define DELAYTIME 500
#define LIFE_COLOR white

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN,
                                              NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
                                              NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
uint32_t red = matrix.Color(255, 0, 0);
uint32_t green = matrix.Color(0, 255, 0);
uint32_t blue = matrix.Color(0, 0, 255);
uint32_t magenta = matrix.Color(255, 0, 255);
uint32_t yellow = matrix.Color(255, 255, 0);
uint32_t orange = matrix.Color(255, 128, 0);
uint32_t purple = matrix.Color(128, 0, 255);
uint32_t white = matrix.Color(255, 255, 255);

bool game_map_1[WIDTH][HEIGHT];
bool game_map_2[WIDTH][HEIGHT];

int generation = 0;

void setup()
{
  Serial.begin(9600);
  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
  matrix.fillScreen(red);
  matrix.show();
  Serial.println("Blank screen");
  delay(DELAYTIME);
  randomSeed(analogRead(0)); // use an analog pin as a seed for randomness

  for (int x = 0; x < WIDTH; x++)
  {
    for (int y = 0; y < HEIGHT; y++)
    {
      game_map_1[x][y] = random(2); // assign either 0 or 1 randomly
      Serial.print(game_map_1[x][y]);
    }
    Serial.println();
  }
  update_matrix();
  delay(DELAYTIME);
  Serial.println("Setup done");
}

void loop()
{
  calculate_next_map();

  copy_map();

  generation++;

  update_matrix();
  delay(DELAYTIME);

  if (is_zero() || generation == 50)
  {
    restart_game();
    generation = 0;
  }
}

void calculate_next_map()
{
  for (int x = 0; x < WIDTH; ++x)
  {
    for (int y = 0; y < HEIGHT; ++y)
    {
      int neighbors = count_neighbors(x, y);

      if (game_map_1[x][y] && (neighbors < 2 || neighbors > 3))
      {
        game_map_2[x][y] = 0;
      }
      else if (!game_map_1[x][y] && neighbors == 3)
      {
        game_map_2[x][y] = 1;
      }
      else
      {
        game_map_2[x][y] = game_map_1[x][y];
      }
    }
  }
}

int count_neighbors(int x, int y)
{
  int count = 0;

  for (int i = -1; i <= 1; ++i)
  {
    for (int j = -1; j <= 1; ++j)
    {
      if (i == 0 && j == 0)
      {
        continue;
      }

      if (game_map_1[(x + i + WIDTH) % WIDTH][(y + j + HEIGHT) % HEIGHT])
      {
        count++;
      }
    }
  }
  return count;
}

void copy_map()
{
  for (int x = 0; x < WIDTH; ++x)
  {
    for (int y = 0; y < HEIGHT; ++y)
    {
      game_map_1[x][y] = game_map_2[x][y];
    }
  }
}

void update_matrix()
{
  for (int x = 0; x < WIDTH; ++x)
  {
    for (int y = 0; y < HEIGHT; ++y)
    {
      if(game_map_1[x][y])
      {
        matrix.drawPixel(x, y, LIFE_COLOR);
      }
      else
      {
        matrix.drawPixel(x, y, 0);
      }
    }
  }
  matrix.show();
}

void restart_game()
{
  if (generation == 50)
  {
    matrix.fillScreen(green);
  }
  else
  {
    matrix.fillScreen(red);
  }
  matrix.show();
  delay(DELAYTIME);

  randomSeed(analogRead(0)); // use an analog pin as a seed for randomness

  for (int x = 0; x < WIDTH; x++)
  {
    for (int y = 0; y < HEIGHT; y++)
    {
      game_map_1[x][y] = random(2); // assign either 0 or 1 randomly
    }
  }
  update_matrix();
  delay(DELAYTIME);
}

bool is_zero()
{
  for (int x = 0; x < WIDTH; ++x)
  {
    for (int y = 0; y < HEIGHT; ++y)
    {
      if (game_map_1[x][y])
      {
        return false;
      }
    }
  }
  return true;
}