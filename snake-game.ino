#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
using namespace std;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SDA_PIN 4
#define SCL_PIN  5
#define LEFT_PIN 21
#define RIGHT_PIN 19
#define UP_PIN 18
#define DOWN_PIN 20

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

class Point {
  public:
    int x, y;

    //Constructors
    Point(int xx, int yy){
      x = xx;
      y = yy;
    }
    Point(){
      x = 1000;
      y = 1000;
    }

  void print() {
    int x1 = x*4;
    int y1 = y*4;
    display.fillRect(x1, y1, 4, 4, SSD1306_WHITE);
  }
  boolean equals(Point right){
    return x == right.x && y == right.y;
  }
};

// Add functionality to prevent moving in opposite direction
class Snake{
  public:
    enum Direction{ UP, DOWN, LEFT, RIGHT };
    Point apple;
    Point mySnake[384];
    int size = 5;
    int score = 0;
    Direction direct;

    //Constructor
    Snake(){
      direct = Snake::LEFT;
      for(int i = 0; i < 5; i++){ //head moving left
        mySnake[i] = Point(i + 13, 6);
      }
      apple = Point(10,10);
    }

    //Prints snake, score and apple
    void printScreen(){
      display.clearDisplay();
      for(int i = 0; i < size; i++){
        mySnake[i].print();
      }
      apple.print();
      display.setCursor(0, 48);
      display.print("Score: ");
      display.print(String(score).c_str());
      display.display();
    }

    //moves head in direction of direct
    //shifts all points up in array and adds head
    //Last element lost unless apple is eaten
    //check loss condition
    void move(){
      int dy = 0; int dx = 0;
      switch(direct){
        case Snake::UP:     dy = -1; break;
        case Snake::DOWN:   dy = 1; break;
        case Snake::LEFT:   dx = -1; break;
        case Snake::RIGHT:  dx = 1; break;
        default: break;
      }
      dx = mySnake[0].x + dx;
      dy = mySnake[0].y + dy;
      if(dx > 31){ dx = 0;}
      if(dx < 0) { dx = 31;}
      if(dy > 11){ dy = 0;}
      if(dy < 0) { dy = 11;}
      Point newHead = Point(dx, dy);
      
      if(newHead.equals(apple)){
        genNewApple();
        size++;
      }

      for(int i = size - 1; i > 0; i--){
        if (mySnake[0].equals(mySnake[i])) { 
          gameOver(); 
        }
        mySnake[i] = mySnake[i - 1];
      }

      mySnake[0] = newHead;
    }

    //Exit the game
    void gameOver(){    
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("GAME OVER");
      display.setCursor(0, 48);
      display.print("Score: ");
      display.print(String(score).c_str());
      display.display();
      delay(5000);
      exit(0);
    }

    //Place apple at a random point
    void genNewApple(){
      int x0 = rand() % 32;
      int y0 = rand() % 12;
      apple.x = x0;
      apple.y = y0;
    }

    //Change direction of snake if possible
    void changeDirection(Snake::Direction dire){
      switch(dire){
        case Snake::UP:
          if(direct == Snake::DOWN){ break; }
            direct = dire;
        case Snake::DOWN: 
          if(direct == Snake::UP){ break; }
            direct = dire; break;
        case Snake::LEFT:
          if(direct == Snake::RIGHT){ break; }
          direct = dire;  break;
        case Snake::RIGHT: 
          if(direct == Snake::LEFT) { break; }
          direct = dire;
        default: break;
      }
    }


};

Snake slither  = Snake();

void setup() {
  //initialize control buttons
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  pinMode(LEFT_PIN, INPUT);
  pinMode(RIGHT_PIN, INPUT);
  pinMode(UP_PIN, INPUT);
  pinMode(DOWN_PIN, INPUT);
  //declare display
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  //initialize display
  display.clearDisplay();
  display.setRotation(2);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
}

void loop() {
  slither.genNewApple();
  for(int i = 0; i < 200; i++){
    catchInput(slither);
    delay(1);
  }
  slither.printScreen();
  slither.move();

}

  void catchInput(Snake &sna){
    if (digitalRead(LEFT_PIN) == HIGH) {
        sna.changeDirection(Snake::LEFT);      
    }
    else if ( digitalRead(RIGHT_PIN)  == HIGH) {
        sna.changeDirection(Snake::RIGHT);
    }
    else if ( digitalRead(UP_PIN)  == HIGH) {
        sna.changeDirection(Snake::UP);
    }
    else if ( digitalRead(DOWN_PIN)  == HIGH) {
        sna.changeDirection(Snake::DOWN);
    }
  }
  