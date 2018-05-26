#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>             // SPI library

//Define the pins
#define TFT_CS 7
#define TFT_RST 5
#define TFT_DC 6
//#define TFT_SCLK = 13
//#define TFT_MOSI = 11

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

//Color variables

//Button variables
const int up = 10; //Pin number
const int right = 8; //Pin number
const int down = 9; //Pin number
const int left = 12; //Pin number
bool up_bool = false; //Boolean for up
bool down_bool = true; //Boolean for down
bool left_bool = false; //Boolean for left
bool right_bool = false; //Boolean for right

//Game variables
int len = 20; //Snake body length
int head_x = 80; //Head X position
int head_y = 10; //Head Y postion
int last_headx = head_x; //Store the value of the last head.x
int last_heady = head_y; //Store the value of the last head.y
int bodyx[200] = {20, 40, 60, 80}; //X values for the body
int bodyy[200] = {10, 10, 10, 10}; //Y values for the body
int fx, fy = 10; //Fruit x and y position
int game_speed = 50; //Game speed
int score = 0; //Score
int last_score = 0; //Change the score for display
int cur = 2; //Cursor selector for the main menu
int speed_sel = 1; //For the speed selection flip flop
bool game_over = true; //Game over boolean
bool main_menu = true;
String speed_string = "LOW";

void setup() 
{
  Serial.begin(9600); //Start Serial
  Serial.print("Starting...");
  //Initialize the Display
  tft.setRotation(3);
  tft.initR(INITR_144GREENTAB);
  Serial.println("Initialized");
  tft.fillScreen(ST7735_BLACK);
}

void loop() 
{
  tft.setRotation(3);
  if (main_menu == false)
  {
    if (game_over == false)
    {
      game_loop(); //Main Game Loop
    }else
    {
      game_over_loop(); //Run game over loop
    }
    
  }else
  {
    menu_loop();
  }
}

void button_loop()
{
  //Button Logic pretty simple stuff
  int up_state = digitalRead(up);
  if (up_state == LOW and down_bool == false){
    Serial.println("Up pressed");
     up_bool = true;
     right_bool= false;
     left_bool = false;
     down_bool = false;
  }
  int down_state = digitalRead(down);
  if (down_state == LOW and up_bool == false){
    Serial.println("Down pressed");
    down_bool = true;
    right_bool= false;
    left_bool = false;
    up_bool = false;
  }
  int right_state = digitalRead(right);
  if (right_state == LOW and left_bool == false){
    Serial.println("Right pressed");
    right_bool= true;
    left_bool = false;
    down_bool = false;
    up_bool = false;
  }
  int left_state = digitalRead(left);
  if (left_state == LOW and right_bool == false){
    Serial.println("Left pressed");
    left_bool = true;
    right_bool= false;
    down_bool = false;
    up_bool = false;
  }
}

void game_loop()
{
  Serial.println("Game Running ");
  //Movement
  for (int i = 0; i < len; i++) //Looping through the array to change the values
  {
    tft.fillRect(bodyx[i], bodyy[i], 10, 10, ST7735_BLACK); //Clear the last segment
    if (right_bool == true)
    {
      bodyy[i] = head_y; //Move this current segment to the head.y
      bodyx[i] = head_x + 5; //Move this current segment to the head + 5 pixels
      if (bodyx[i] > 120) //If the head exceeds the screen
      {
        bodyx[i] = 0;
      }
    }
    if (down_bool == true)
    {
      bodyx[i] = head_x;
      bodyy[i] = head_y + 5;
      if (bodyy[i] > 120)
      {
        bodyy[i] = 0;
      }
    }
    if (up_bool == true)
    {
      bodyx[i] = head_x;
      bodyy[i] = head_y - 5; 
      if (bodyy[i] < 0)
      {
        bodyy[i] = 120;
      }
    }
    if (left_bool == true)
    {
      bodyy[i] = head_y;
      bodyx[i] = head_x - 5;
      if (bodyx[i] < 0)
      {
        bodyx[i] = 120;
      }
    }
    last_headx = head_x; // Update the last head pos.x
    last_heady = head_y; //Update the last head pos.y
    head_x = bodyx[i]; //Update this current segment as the new head.x
    head_y = bodyy[i]; //Update this current segment as the new head.y
    //Collision
    for (int j = 0; j < len; j++)
    {
      if (j != i) //If the head intersects with a segment
      {
        if (head_x == bodyx[j] and head_y == bodyy[j])
        {
          game_over = true;
          Serial.println("Game Over");
          len = 0; 
          
        }
      }
    }
    tft.fillRect(bodyx[i], bodyy[i], 5, 5, ST7735_GREEN); //Draw the current segment
    fruit(); //Fruit loop :P
    draw_score(); //Draw the score
    button_loop(); //Check for buttons being pressed
    
    delay(game_speed); //Delay the main game loop
  }
  if (game_over == true) //Clear the screen on the last loop iteration
  {
    tft.fillRect(0, 0, 128, 128, ST7735_BLACK); //Clear the screen
  }
}

void fruit()
{
  if (head_x >= fx and (head_x + 10) <= (fx + 10) and head_y >= fy and (head_y + 10) <= (fy + 10))
  {
    if (len < 199) //So the length of the snake doesnt exceed the array limit
    {
      len = len + 1; //Add a segment to the snake
    }
    //Create a new fruit in a random position
    fx = random(4, 11);
    fy = random(4, 11);
    fx = fx * 10;
    fy = fy * 10;
    score = score + 1; //Add to the score
  }
  tft.fillRect(fx, fy, 5, 5, ST7735_RED); //Draw the fruit
}

void draw_score()
{
  tft.setCursor(0, 2); //Set the cursor for the text to start
  tft.setTextColor(ST7735_WHITE); //Set the font color
  tft.setTextWrap(false); //I doubt anyone could get a score above 100000
  if (score > last_score)
  {
    tft.fillRect(35, 0, 25, 10, ST7735_BLACK); //Draw a black box to clear the last numbers
    last_score = score; //Update the last score
  }
  tft.print("Score:" + String(score)); //Print the Score string
}

void menu_loop()
{
  tft.setCursor(5, 10); //Set the cursor for the text to start
  tft.setTextSize(2);
  tft.setTextColor(ST7735_GREEN); //Set the font color
  tft.setTextWrap(false); //No text wrapping needed
  tft.print("SNAKE GAME"); //Print Snake to the screen

  //Draw the menu select items
  //Draw the play button
  tft.setCursor(50, 40);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
  tft.print("Start");

  //Draw Speed select
  tft.setCursor(38, 60);
  tft.print("Speed:" + speed_string);

  //Speed selecting
  //Set up the button states
  int up_state = digitalRead(up);
  int down_state = digitalRead(down);
  int right_state = digitalRead(right);
  int left_state = digitalRead(left);
  if (right_state == LOW or left_state == LOW)
  {
    if (cur == 2)
    {
      if (speed_string == "HIGH" and speed_sel == 1)
      {
        speed_string = "LOW";
        game_speed = 100;
        speed_sel = 0;
      }
      if (speed_string == "MED" and speed_sel == 1)
      {
        speed_string = "HIGH";
        game_speed = 20;
        speed_sel = 0;
      }
      if (speed_string == "LOW" and speed_sel == 1)
      {
        speed_string = "MED";
        game_speed = 50;
        speed_sel = 0;
      }
      speed_sel = 1;
      tft.fillRect(74, 60, 40, 20, ST7735_BLACK); //Draw a black box to clear the speed text
      delay(100);
    }else{
      game_over = false;
      main_menu = false;
      len = 4;
      score = 0;
      tft.fillRect(0, 0, 128, 128, ST7735_BLACK); //Clear the screen
    }
  }
    
  if(up_state == LOW)
  {
    if(cur == 1)
    {
      cur = 2;
      tft.fillTriangle(25, 60, 25, 68, 32, 64, ST7735_WHITE); //Draw a selection triangle
      tft.fillRect(25, 40, 8, 9, ST7735_BLACK); //Draw a black box to clear the arrow
      delay(100);
      
    }else
    {
      cur = 1;
      tft.fillTriangle(25, 40, 25, 48, 32, 44, ST7735_WHITE); //Draw a selection triangle
      tft.fillRect(25, 60, 8, 9, ST7735_BLACK); //Draw a black box to clear the arrow
      delay(100);
    }
  }
  if(down_state == LOW)
  {
    if(cur == 1)
    {
      cur = 2;
      tft.fillTriangle(25, 60, 25, 68, 32, 64, ST7735_WHITE); //Draw a selection triangle
      tft.fillRect(25, 40, 8, 9, ST7735_BLACK); //Draw a black box to clear the arrow
      delay(100);
    }else
    {
      cur = 1;
      tft.fillTriangle(25, 40, 25, 48, 32, 44, ST7735_WHITE); //Draw a selection triangle
      tft.fillRect(25, 60, 8, 9, ST7735_BLACK); //Draw a black box to clear the arrow
      delay(100);
    }
  }
}


void game_over_loop()
{
  delay(100);
  tft.setCursor(10, 2); //Set the cursor for the text to start
  tft.setTextSize(2);
  tft.setTextColor(ST7735_RED); //Set the font color
  tft.setTextWrap(false); //No text wrapping needed
  tft.print("GAME OVER"); //Print Game over to the screen
  //Show the final score
  tft.setTextColor(ST7735_WHITE); //Set the font color
  tft.setCursor(15, 50);
  tft.setTextSize(2);
  tft.print("Score:" + String(score));
  //Show the key text
  tft.setCursor(25, 100);
  tft.setTextSize(1);
  tft.print("Press any key");
  //Set up the button states
  int up_state = digitalRead(up);
  int down_state = digitalRead(down);
  int right_state = digitalRead(right);
  int left_state = digitalRead(left);
  if (right_state == LOW or left_state == LOW or up_state == LOW or down_state == LOW)
  {
    main_menu = true; //Go back to the main menu if any key is pressed
    tft.fillRect(0, 0, 128, 128, ST7735_BLACK); //Clear the screen
  }
}

