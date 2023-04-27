#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6               // Data pin that NeoPixel is connected to
#define WIDTH 8             // Width of the matrix
#define HEIGHT 8            // Height of the matrix
#define BRIGHTNESS 5        // Max brightness of each pixel. Important to select based on total pixel count and available power supply
#define DELAYTIME 500       // Time between each generation
#define LIFE_COLOR white    // Colow of alive pixels
#define MAX_GENERATIONS 50  // Max number of generations to run for
#define SHOW_STATES 1       // Show intermediate states
#define DEBUG_PRINTS 0      // Enable debug prints

/* Create matrix object. The 4th param defines the first pixel and the orientation of
   the pixels on the matrix. Refer to Adafruit documentation for details.
*/
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN,
                                              NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
                                              NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);

// Some common colors to use as needed
uint32_t red = matrix.Color(255, 0, 0);
uint32_t green = matrix.Color(0, 255, 0);
uint32_t blue = matrix.Color(0, 0, 255);
uint32_t magenta = matrix.Color(255, 0, 255);
uint32_t yellow = matrix.Color(255, 255, 0);
uint32_t orange = matrix.Color(255, 128, 0);
uint32_t purple = matrix.Color(128, 0, 255);
uint32_t white = matrix.Color(255, 255, 255);

// 2 matrices required as next generation is calculated on a new one and copied to older matrix
bool game_map_1[WIDTH][HEIGHT];
bool game_map_2[WIDTH][HEIGHT];

#if SHOW_STATES
// Array to store colors of next generation of cells
uint32_t pixel_colors[WIDTH][HEIGHT];
#endif

// To track elapsed generations for resetting after a fixed time
int generation = 0;
bool restart = false;

// Setup code. Runs once on power up
void setup()
{
#if DEBUG_PRINTS
  // Serial logs for debugging
  Serial.begin(9600);
#endif
  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
  matrix.fillScreen(red);
  matrix.show();
#if DEBUG_PRINTS
  Serial.println("Blank screen");
#endif
  delay(DELAYTIME);
  randomSeed(analogRead(0)); // Use an analog pin as a seed for randomness

  for (int x = 0; x < WIDTH; x++)
  {
    for (int y = 0; y < HEIGHT; y++)
    {
      game_map_1[x][y] = random(2); // Assign either 0 or 1 randomly
    #if DEBUG_PRINTS
      Serial.print(game_map_1[x][y]); // Print the initial matrix to serial
    #endif
    }
  #if DEBUG_PRINTS
    Serial.println();
  #endif
  }
  update_matrix();
  delay(DELAYTIME);
#if DEBUG_PRINTS
  Serial.println("Setup done");
#endif
}

// Loop code. Runs endlessly
void loop()
{
  // Calculate next generation
  calculate_next_map();

  // Copy new generation to old matrix
  copy_map();

  generation++;

  // Send the array to the matrix
  update_matrix();

  delay(DELAYTIME);

  // If all cells are dead or max generations have elapsed, reset
  if (is_zero() || generation == MAX_GENERATIONS)
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
      // Count neighbors of the current cell
      int neighbors = count_neighbors(x, y);

      // If cell was alive and has less than 2 or more than 3 neighbors, it dies
      if (game_map_1[x][y] && (neighbors < 2 || neighbors > 3))
      {
        game_map_2[x][y] = 0;

      #if SHOW_STATES
        pixel_colors[x][y] = red; // Mark newly dead pixels
      #endif
      }
      // If cell was dead and has exactly 3 neighbors, it comes alive
      else if (!game_map_1[x][y] && neighbors == 3)
      {
        game_map_2[x][y] = 1;

      #if SHOW_STATES
        pixel_colors[x][y] = green; // Mark newly alive pixels
      #endif
      }
      // All other cells remain the same
      else
      {
        game_map_2[x][y] = game_map_1[x][y];

      #if SHOW_STATES
        pixel_colors[x][y] = white; // Mark continuing pixels
      #endif
      }
    }
  }
}

/*
  Count alive neighbors of a cell in a 3x3 grid.
  Each cell has 8 neighbors. i & j iterate over -1 to 1, with (0,0) being the cell itself
  (i,j) --> (-1,-1) (0,-1) (1,-1)
            (-1, 0) (0, 0) (1, 0)
            (-1, 1) (0, 1) (1, 1)
*/
int count_neighbors(int x, int y)
{
  int count = 0;

  for (int i = -1; i <= 1; ++i)
  {
    for (int j = -1; j <= 1; ++j)
    {
      // Avoid counting the cell itself
      if (i == 0 && j == 0)
      {
        continue;
      }

      // Loop over the edges while counting
      if (game_map_1[(x + i + WIDTH) % WIDTH][(y + j + HEIGHT) % HEIGHT])
      {
        count++;
      }
    }
  }
  return count;
}

// Copy new array to old array
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

// Send the new state to the NeoPixels and show them
void update_matrix()
{
  // If it's a restart, avoid showing intermediate state
  if (!restart)
  {
#if SHOW_STATES
  for (int x = 0; x < WIDTH; ++x)
  {
    for (int y = 0; y < HEIGHT; ++y)
    {
      if(game_map_1[x][y])
      {
        matrix.drawPixel(x,y,pixel_colors[x][y]);
      }
      else if (pixel_colors[x][y] == red)
      {
        matrix.drawPixel(x,y,pixel_colors[x][y]);
      }
      else
      {
        matrix.drawPixel(x, y, 0);
      }
    }
  }
  matrix.show();
  delay(DELAYTIME);
#endif
  }

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

// Reset the states randomly and restart
void restart_game()
{
  restart = true;
  if (generation == MAX_GENERATIONS)
  {
    matrix.fillScreen(green);
  }
  else
  {
    matrix.fillScreen(red);
  }
  matrix.show();
  delay(DELAYTIME);

  for (int x = 0; x < WIDTH; x++)
  {
    for (int y = 0; y < HEIGHT; y++)
    {
      game_map_1[x][y] = random(2); // Assign either 0 or 1 randomly
    }
  }
  update_matrix();
  delay(DELAYTIME);
  restart = false;
}

// Check if all cells are dead
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