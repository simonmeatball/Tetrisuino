#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define R_LED 3
#define G_LED 4
#define B_LED 5
#define BUTTON 6
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Joystick pins
const int VRX = A0;
const int VRY = A1;
const int SW = 2;

// Some variables for movement
const int MIDDLE = -1;
const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;
const int buttonPressed = 0;

// Piece & screen variables
const int16_t blockWidth = 5;
const int16_t blockHeight = blockWidth;
const int16_t white = SSD1306_WHITE;
const int16_t minX = 2;
const int16_t maxX = 10 * (blockWidth + 1) + 2;
const int16_t minY = 2;
const int16_t maxY = 20 * (blockHeight + 1) + 2;

const int16_t I = 0;
const int16_t J = 1;
const int16_t L = 2;
const int16_t O = 3;
const int16_t S = 4;
const int16_t T = 5;
const int16_t Z = 6;

const int16_t left = -1;
const int16_t right = 1;

// General game variables
int16_t originalTickCount = 20000;
int16_t level = 1;
int16_t tickCount = originalTickCount;
int score = 0;
int hiScore = 0;
int clearedLines = 0;
int combo = -1;
int blinkTimes = 0;
bool isOn = true;
int16_t heldPiece = -1;
bool heldPressed = false;

int16_t blockMap[20][10] = {
  // blockMap[column][row]
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0}
};

const unsigned char myBitmap [] PROGMEM = {
	// 'tetrislogo, 128x64px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x0f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x0f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x0f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x0f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x0f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x1f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3d, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x3d, 0xef, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x38, 0xef, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x38, 0x6f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x38, 0x0f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x38, 0x07, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x30, 0x07, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x20, 0x0f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x0f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x0f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x07, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x03, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x07, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x1f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x3f, 0xbe, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x3f, 0x0c, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xfe, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x38, 0x0c, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3c, 0x3e, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3c, 0x7f, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3c, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0xcf, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3f, 0x87, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x1e, 0x03, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x04, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*
Coordinate:
  0 1 2 3 4 5 6 7 8 9 (x)
19
18
17
16
...
3
2
1
0
(y)
*/

// BLOCK CLASS
class Block
{
public:
  Block(int16_t x, int16_t y) : m_x(x), m_y(y) {}
  bool canDraw(int16_t x, int16_t y);
  void draw(int16_t x, int16_t y);
  void moveDown();
  void move(int16_t x, int16_t y) { m_x = x; m_y = y; }
  int16_t getX() { return m_x; }
  int16_t getY() { return m_y; }
private:
  int16_t m_x;
  int16_t m_y;
};

// BLOCK CLASS IMPLEMENTATIONS
bool Block::canDraw(int16_t x, int16_t y)
{
  if (blockMap[y][x] == 1)
    return false;
  if (x >= 0 && x < 10 && y >= 0 && y < 20)
    return true;
  return false;
}

void Block::draw(int16_t x, int16_t y)
{
  // takes in x from 0 to 9, y from 0 to 19
  display.drawRect(y * (blockWidth + 1) + 2, x * (blockHeight + 1) + 2, blockWidth, blockHeight, SSD1306_WHITE);
}

void Block::moveDown()
{
  m_y--;
  draw(m_x, m_y);
}

// PIECE CLASS
class Piece
{
public:
  Piece(int16_t x, int16_t y, int16_t type);
  ~Piece() {
    for (int i = 0; i < 4; i++)
      delete pieceBlocks[i];
  }
  bool getState() { return m_state; }
  bool moveDir(int16_t dir);
  bool moveDown();
  bool moveLeft();
  bool rotate();
  void draw();
  int16_t getType() { return m_type; }
  Block** blocks() { return pieceBlocks; }
private:
  int16_t m_x;
  int16_t m_y;
  bool m_state;
  int16_t m_type;
  Block* pieceBlocks[4];
};

// PIECE CLASS IMPLEMENTATION
Piece::Piece(int16_t x, int16_t y, int16_t type) : m_x(x), m_y(y), m_type(type), m_state(true)
{
  for (int16_t i = 0; i < 4; i++)
  {
    switch(type)
    {
      case I:
        pieceBlocks[i] = new Block(m_x + i - 1, m_y);
        break;
      case J:
        if (i == 3)
          pieceBlocks[i] = new Block(m_x - 1, m_y + 1);
        else
          pieceBlocks[i] = new Block(m_x + i - 1, m_y);
        break;
      case L:
        if (i == 3)
          pieceBlocks[i] = new Block(m_x + 1, m_y + 1);
        else
          pieceBlocks[i] = new Block(m_x + i - 1, m_y);
        break;
      case O:
        switch(i)
        {
          case 0:
            pieceBlocks[i] = new Block(m_x, m_y);
            break;
          case 1:
            pieceBlocks[i] = new Block(m_x + 1, m_y);
            break;
          case 2:
            pieceBlocks[i] = new Block(m_x + 1, m_y - 1);
            break;
          case 3:
            pieceBlocks[i] = new Block(m_x, m_y - 1);
            break;
        }
        break;
      case S:
        if (i == 0 || i == 1)
          pieceBlocks[i] = new Block(m_x + i - 1, m_y);
        else
          pieceBlocks[i] = new Block(m_x + i - 2, m_y + 1);
        break;
      case T:
        if (i == 3)
          pieceBlocks[i] = new Block(m_x, m_y + 1);
        else
          pieceBlocks[i] = new Block(m_x + i - 1, y);
        break;
      case Z:
        if (i == 0 || i == 1)
          pieceBlocks[i] = new Block(m_x + i - 1, m_y + 1);
        else
          pieceBlocks[i] = new Block(m_x + i - 2, m_y);
        break;
    }
  }
  for (int16_t i = 0; i < 4; i++)
  {
    if (!pieceBlocks[i]->canDraw(pieceBlocks[i]->getX(), pieceBlocks[i]->getY()))
      m_state = false;
  }
}

bool Piece::moveDown()
{
  for (int16_t i = 0; i < 4; i++)
  {
    if (!pieceBlocks[i]->canDraw(pieceBlocks[i]->getX(), pieceBlocks[i]->getY() - 1))
    {
      m_state = false;
      return false;
    }
  }
  for (int16_t i = 0; i < 4; i++)
  {
    pieceBlocks[i]->moveDown();
  }
  m_y--;
  return true;
}

bool Piece::moveDir(int16_t dir)
{
  for (int16_t i = 0; i < 4; i++)
  {
    if (!pieceBlocks[i]->canDraw(pieceBlocks[i]->getX() + dir, pieceBlocks[i]->getY()))
      return false;
  }

  for (int16_t i = 0; i < 4; i++)
  {
    pieceBlocks[i]->move(pieceBlocks[i]->getX() + dir, pieceBlocks[i]->getY());
    // Serial.println("moving right/left");
  }
  m_x += dir;
  return true;
}

bool Piece::rotate()
{
  int16_t x;
  int16_t y;

  for (int16_t i = 0; i < 4; i++)
  {
    x = m_x - (pieceBlocks[i]->getY() - m_y);
    y = m_y + (pieceBlocks[i]->getX() - m_x);
    if (!pieceBlocks[i]->canDraw(x, y))
    {
      return false;
    }
  }

  for (int16_t i = 0; i < 4; i++)
  {
    x = m_x - (pieceBlocks[i]->getY() - m_y);
    y = m_y + (pieceBlocks[i]->getX() - m_x);
    pieceBlocks[i]->move(x, y);
  }
  return true;
}

void Piece::draw()
{
  for (int16_t i = 0; i < 4; i++)
  {
    pieceBlocks[i]->draw(pieceBlocks[i]->getX(), pieceBlocks[i]->getY());
  }
}


// MEMBER FUNCTIONS
void displayGame()
{
  for (int16_t i = 0; i < display.width() - 5; i++)
  {
    display.drawPixel(i, 0, SSD1306_WHITE);
    display.drawPixel(i, display.height() - 2, SSD1306_WHITE);
  }

  for (int16_t i = 0; i < display.height() - 1; i++)
  {
    display.drawPixel(0, i, SSD1306_WHITE);
    display.drawPixel(display.width() - 6, i, SSD1306_WHITE);
  }

  for (int16_t i = 0; i < 10; i++)
  {
    for (int16_t j = 0; j < 20; j++)
    {
      if (blockMap[j][i] == 1)
      {
        display.drawRect(j * (blockWidth + 1) + 2, i * (blockHeight + 1) + 2, blockWidth, blockHeight, SSD1306_WHITE);
      }
    }
  }
  display.setRotation(0);

  display.display();
}

int joystickDir(int x, int y)
{
  if (x < 800 && x > 250)
  {
    if (y > 650)
      return RIGHT;
    else if (y < 400)
      return LEFT;
  }
  if (y < 800 && y > 250)
  {
    if (x > 650)
      return UP;
    else if (x < 250)
      return DOWN;
  }
  return MIDDLE;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT);
  digitalWrite(SW, INPUT_PULLUP);
  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  randomSeed(analogRead(A15));

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  // display.setRotation(1);
  display.drawBitmap(0,0,myBitmap, 128, 64, WHITE);
  display.setRotation(1);
  display.setCursor(0, 100);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Press to  start.");
  display.display();
  double brightness = 30;
  bool reverse = false;
  while (digitalRead(SW) != buttonPressed)
  { 
    analogWrite(B_LED, brightness);
    if (!reverse)
      brightness -= 0.001; 
    else
      brightness += 0.001;
    if (brightness <= 0)
      reverse = true;
    else if (brightness >= 30)
      reverse = false;
  }
  display.clearDisplay();
  display.setRotation(0);

  for (int16_t i = 0; i < display.width() - 5; i++)
  {
    display.drawPixel(i, 0, SSD1306_WHITE);
    display.drawPixel(i, display.height() - 2, SSD1306_WHITE);
  }

  for (int16_t i = 0; i < display.height() - 1; i++)
  {
    display.drawPixel(0, i, SSD1306_WHITE);
    display.drawPixel(display.width() - 6, i, SSD1306_WHITE);
  }

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
}

void resetMap()
{
  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < 20; j++)
      blockMap[j][i] = 0;
  }
  score = 0;
  level = 0;
  clearedLines = 0;
  combo = -1;
  blinkTimes = 0;
  isOn = true;
  heldPressed = false;
  heldPiece = -1;
  originalTickCount = 20000;
}

int16_t clearRow()
{
  int16_t sum = 0;
  int16_t rows = 0;
  for (int16_t h = 0; h < 20; h++)
  {
    for (int16_t w = 0; w < 10; w++)
    {
      sum += blockMap[h][w];
    }
    if (sum == 10)
    {
      rows++;
      
      for (int16_t i = h; i < 19; i++)
      {
        for (int16_t j = 0; j < 10; j++)
          blockMap[i][j] = blockMap[i + 1][j];
      }
      for (int16_t j = 0; j < 10; j++)
      {
        blockMap[19][j] = 0;
      }
      h--;
    }
    sum = 0;
  }
  return rows;
}

void turnOnLED()
{
  if (level <= 20)
  {
    analogWrite(B_LED, 0);
    analogWrite(R_LED, 20 - level);
    analogWrite(G_LED, 10 + level*.5);
  }
  else
  {
    analogWrite(B_LED, 0);
    analogWrite(G_LED, 0);
    analogWrite(R_LED, 20);
  }
}

void turnOffLED()
{
  analogWrite(B_LED, 0);
  analogWrite(R_LED, 0);
  analogWrite(G_LED, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  turnOnLED();
  isOn = true;
  heldPressed = false;

  int vrxValue = analogRead(VRX);
  int vryValue = analogRead(VRY);
  int swValue = digitalRead(SW);
  int direc = joystickDir(vrxValue, vryValue);
  bool wasFalse = true;

  int16_t rInt = random(0, 7);

  Piece* block = new Piece(5, 18, rInt);
  bool reset = false;
  if (!block->getState())
  {
    display.clearDisplay();
    reset = true;
  }

  displayGame();
  
  while (block->getState())
  {
    tickCount--;
    if (!wasFalse && digitalRead(SW) != buttonPressed)
      wasFalse = true;
    if (tickCount % 3500 == 0)
    {
      // Serial.println(digitalRead(BUTTON));
      if (digitalRead(BUTTON) == 1 && heldPressed == false)
      {
        heldPressed = true;
        if (heldPiece == -1)
        {
          heldPiece = block->getType();
          delete block;
          block = new Piece(5, 18, random(0, 7));
        }
        else
        {
          int16_t oldHeldPiece = heldPiece;
          heldPiece = block->getType();
          delete block;
          block = new Piece(5, 18, oldHeldPiece);
        }
      }
      if (blinkTimes > 0 && isOn)
      {
        turnOffLED();
        isOn = false;
      }
      else if (blinkTimes > 0)
      {
        turnOnLED();
        blinkTimes--;
        isOn = true;
      }
      vrxValue = analogRead(VRX);
      vryValue = analogRead(VRY);
      swValue = digitalRead(SW);
      direc = joystickDir(vrxValue, vryValue);

      if (wasFalse && swValue == buttonPressed)
      {
        block->rotate();
        wasFalse = false;
        display.clearDisplay();
        block->draw();
        displayGame();
      }
      else if (direc == RIGHT)
      {
        block->moveDir(right);
        display.clearDisplay();
        block->draw();
        displayGame();
      }
      else if (direc == LEFT)
      {
        block->moveDir(left);
        display.clearDisplay();
        block->draw();
        displayGame();
      }
      else if (direc == DOWN)
      {
        block->moveDown();
        display.clearDisplay();
        block->draw();
        displayGame();
      }
      // display.clearDisplay();
      // displayGame();
      block->draw();
    }
    // block->rotate();
    if (tickCount == 0)
    {
      display.clearDisplay();
      block->moveDown();
      displayGame();
      tickCount = originalTickCount;
    }
  }
  for (int i = 0; i < 4; i++)
  {
    blockMap[block->blocks()[i]->getY()][block->blocks()[i]->getX()] = 1;
  }
  delete block;

  int16_t rows = clearRow();

  if (combo != -1 && rows == 0)
  {
    combo = -1;
  }
  else if (rows == 1)
  {
    score += 40 * level;
    combo++;
    blinkTimes = 1;
  }
  else if (rows == 2)
  {
    score += 100 * level;
    combo++;
    blinkTimes = 2;
  }
  else if (rows == 3)
  {
    score += 300 * level;
    combo++;
    blinkTimes = 3;
  }
  else if (rows == 4)
  {
    score += 1200 * level;
    combo++;
    blinkTimes = 4;
  }
  clearedLines += rows;
  if (combo > 0)
    score += 50 * combo * level;

  if (clearedLines >= level * 10)
  {
    level++;
    clearedLines = 0;
    if (level <= 10)
      originalTickCount -= 1000;
    else if (level == 13 || level == 16 || level == 19 || level == 29)
      originalTickCount -= 2000;
  }

  if (reset)
  {
    analogWrite(G_LED, 0);
    analogWrite(R_LED, 0);
    analogWrite(B_LED, 0);
    if (score > hiScore)
      hiScore = score;
    display.setRotation(1);
    // Serial.println("resetting...");
    display.clearDisplay();
    // Serial.println(score);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.println("Score");
    display.setTextSize(1);
    display.println();
    if (score <= 99999)
      display.setTextSize(2);
    display.println(score);
    display.setTextSize(1);
    display.println();
    display.print("High: ");
    if (hiScore > 9999)
      display.println();
    display.println(hiScore);
    display.setTextSize(0);
    display.println();
    display.println("Press to");
    display.println("continue.");
    display.display();
    double brightness = 30;
    bool reverse = false;
    while (swValue != buttonPressed)
    {
      swValue = digitalRead(SW);
      analogWrite(B_LED, brightness);
      if (!reverse)
        brightness -= 0.001; 
      else
        brightness += 0.001;
      if (brightness <= 0)
        reverse = true;
      else if (brightness >= 30)
        reverse = false;
    }
    resetMap();
  }
  displayGame();
}
