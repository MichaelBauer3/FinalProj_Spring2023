// **************************************************************************** 
// Creators: Jacob Barron, Michael Bauer, Raju Pradhan
// File: main_terminal-ICT-06.c
// Description: This program will act as a racing game using the terminal.  
// **************************************************************************** 
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dg256.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h" /* interface to the assembly module */
#include "csc202_lab_support.h"
#include <stdlib.h>


// Definitions
#define y_plane 7
#define x_plane 30
#define lighton 150
#define lightondim 20
#define bottom 5
#define top 1
#define start_x 1
#define start_y 3
#define create_border_top 0
#define create_border_bottom 6
#define finish_line 35
#define difficulty_multi 2.5
#define amt_per_col 3


// Global Variables
char* array[y_plane][x_plane];
char* character = ">";
bool stopped = FALSE;
uint8 finished = FALSE;
int moves = 0;
int x_pos;
int y_pos;
int i, j;


// Prototypes 
void startlights(void);
void finishlights(void);
int upshift(short gear);
int downshift(short gear);
void my_alt_print(char*);
void generate_track(void);
void print_track();
char menu(void);
void game(int, int);
void shift_left(int); 
void race(int, int);
void move_y_up(void);
void move_y_down(void);
void collision(void);




// **************************************************************************** 
// Description: 
//      move_y_up() checks the position of the character, ensuring its not
//      at the highest point allowed. If it is, no upward movement is allowed
// 
// Input Parameters:
//      None 
//
// Output Parameters:
//      None
// 
// Returns:
//      None       
// **************************************************************************** 

void move_y_up()
{
  if(y_pos > top)
  {
    y_pos--;
    array[y_pos+1][x_pos] = " "; 
    
  }
  else
  {
    y_pos = top;
  }
  
}


// **************************************************************************** 
// Description: 
//      move_y_down() checks the position of the character, ensuring its 
//      not at the lowest point allowed. If it is, no downward movement is
//      allowed.
// 
// Input Parameters:
//      None 
//
// Output Parameters:
//      None
// 
// Returns:
//      None       
// **************************************************************************** 
void move_y_down()
{
  if(y_pos < bottom)
  {
    y_pos++;
    array[y_pos-1][x_pos] = " ";
  }
  else
  {
    y_pos = bottom;
  }
  
}


// **************************************************************************** 
// Description: 
//      my_alt_print(char* string) uses a while loop to print a string to
//      the terminal using the outchar1 function.
// 
// Input Parameters:
//      char* string - the string that is going to be broken down and printed
//                     to the screen.  
//
// Output Parameters:
//      None
// 
// Returns:
//      None       
// ****************************************************************************
void my_alt_print(char* string)
{
  int index = 0;
  while(string[index] != '\0')
  {
    outchar1(string[index]);
    index++;    
  }
}


// **************************************************************************** 
// Description: 
//      print_track() uses two nested for-loops in order to print the array
// 
// Input Parameters:
//      None
//
// Output Parameters:
//      None
// 
// Returns:
//      None       
// ****************************************************************************
void print_track()
{
  for(i = 0; i < y_plane; i++)
  {
    for(j = 0; j < x_plane; j++)
    {
      my_alt_print(array[i][j]);
    }
    my_alt_print("\r\n");
  }
}


// **************************************************************************** 
// Description: 
//      generate_track() creates a 2D array using nested for loops. It also
//      sets the users locations and creates the board to our specification
// 
// Input Parameters:
//      None
//
// Output Parameters:
//      None
// 
// Returns:
//      None       
// ****************************************************************************
void generate_track()
{
  for(i = 0; i < y_plane; i++)
  {
    for(j = 0; j < x_plane; j++)
    {
      if (i == create_border_top || i == create_border_bottom)
      {
        array[i][j] = "-";
      }
      else if(i == start_y && j == start_x)
      {
        array[i][j] = character;
        x_pos = start_x;
        y_pos = start_y;
        
      }
      else
      {
      
        array[i][j] = " ";
 
      }
    }
  }
}


// **************************************************************************** 
// Description: 
//      shift_left(int) shifts the entire array to the left. Once that is
//      completed, the program writes over the last column adding new obstacles 
//      or the finish line depending on the location
// 
// Input Parameters:
//      int level - the level of difficulty. Tells how many obstacles to spawn
//
// Output Parameters:
//      None
// 
// Returns:
//      None       
// ****************************************************************************
void shift_left(int level) {         

  
  int count = 0;
  char* obstacle = "X";
  char* finish = "|";
  char* finish_msg = "You won!";
  
  // Shift all elements of each row to the left
  for (i = 0; i < y_plane; i++) {
    for (j = 0; j < x_plane - 1; j++) {
      array[i][j] = array[i][j+1];
    }
  }
  
 
  array[y_pos][x_pos - 1] = " ";
  if(array[y_pos][x_pos] == obstacle)
  {
    collision();
  }
  else if(array[y_pos][x_pos] == finish)
  { 
    finished = TRUE;
    array[y_pos][x_pos] = character;
    motor4(0);
    motor5(0);
    motor6(0);
    clear_lcd();
    finishlights();
    ms_delay(250);
    motor1(0);
    seg7s_off();
    leds_off();
    type_lcd(finish_msg);
    
    ms_delay(3000);
    clear_lcd();
    moves=0;
  }
  else
  {
    array[y_pos][x_pos] = character;
  } 
  
  // Add a new column at the end with random obstacles
  for (i = 0; i < y_plane; i++) {
    if (i == create_border_top || i == create_border_bottom) 
    {
      array[i][x_plane - 1] = "-";
    }
    else 
    {
      if(moves!=50) // Track vision is 15, so to prevent obstacles from 
                    // spawning we ensure nothing spawns after the finish line
      {
        if ((rand() % 100 < level * difficulty_multi) && (count != amt_per_col) && (moves < finish_line)) 
        {
          array[i][x_plane - 1] = obstacle;
          count++;
        }
        else
        {
          array[i][x_plane-1] = " ";
        }
      }
      else
      {
        array[i][x_plane-1] = finish;
      }
    
    }
    count = 0;
  }
  
  moves++;
  alt_clear();
  print_track();
}


// **************************************************************************** 
// Description: 
//      the menu() function asks the user to enter their choice for the menu 
//      then it returns the choice to the switch menu in main()
// 
// Input Parameters:
//      None
//
// Output Parameters:
//      None
// 
// Returns:
//      char - the users choice.       
// ****************************************************************************
char menu()
{
  char* menuOptions = "\r\nMENU OPTIONS ";
  char* opt1 = "\n\r1. Start Game ";
  char* opt2 = "\n\r2. Select Difficulty ";
  char* opt3 = "\n\r3. Select Car ";
  char* opt4 = "\n\r4. End Program ";
  char* spaces = "\n\r";
  char choice = '2';

  alt_clear();
  my_alt_print(menuOptions);
  my_alt_print(opt1);
  my_alt_print(opt2);
  my_alt_print(opt3);
  my_alt_print(opt4);
  my_alt_print(spaces);

  choice = inchar1();
  outchar1(choice);

  return choice;
}


// **************************************************************************** 
// Description: 
//      The car_menu() function asks the user to enter their choice for car
//      then it returns the choice to the mini switch menu in main() 
// 
// Input Parameters:
//      None
//
// Output Parameters:
//      None
// 
// Returns:
//      char - the users choice.       
// ****************************************************************************
char car_menu()
{
  char* carOptions = "\r\nMENU OPTIONS ";
  char* car1 = "\n\r1. BMW 1 Series M ";
  char* car2 = "\n\r2. Pontiac G8 GXP ";
  char* car3 = "\n\r3. Audi RS5 ";
  char* car4 = "\n\r4. Corvette Z06 ";
  char* spaces = "\n\r";
  char choice = '2';
  
  alt_clear(); 
  my_alt_print(carOptions);
  my_alt_print(car1);
  my_alt_print(car2);
  my_alt_print(car3);
  my_alt_print(car4);
  my_alt_print(spaces);
  
  choice = inchar1();
  outchar1(choice);
  
  return choice;
}


// **************************************************************************** 
// Description: 
//      game function clears the terminal of the menu, then generates the
//      starting track and prints it. It then calls the race function. 
// 
// Input Parameters:
//      int level - the level of difficulty. Tells how many obstacles to spawn
//      int hp - the horse power of the selected car
//
// Output Parameters:
//      None
// 
// Returns:
//      None       
// ****************************************************************************
void game(int level, int hp)
{
  uint8 DONE = FALSE;
  alt_clear();
  generate_track();
  print_track();
  race(level, hp);
  alt_clear(); 

}
  

// **************************************************************************** 
// Description: 
//      main controls/calls all of the other functions of the program.
//      Switch function based off of users input tells the program 
//      what to do
// Input Parameters:
//      None
//
// Output Parameters:
//      None
// 
// Returns:
//      None       
// ****************************************************************************
void main(void) 
{
  char user_input = 0;
  char car_input = 0;
  char* difficulty = "0";
  uint8 level = 0;
  uint8 WAITING = TRUE;
  char play_choice;
  int hp;

  PLL_init();
  SCI1_init(9600);
  led_enable();
  SW_enable();
  seg7_enable();
  lcd_init();
  ad0_enable();
  ad1_enable();
  motor1_init();
  motor1(0);
  motor4_init();
  motor4(0);
  motor5_init();
  motor5(0);
  motor6_init();
  motor6(0);
  DDRM = 0xFF;
  
  alt_clear();
  my_alt_print("Welcome to our game!");

  while(user_input != '4')
  {
    user_input = menu();
    switch(user_input)
    {
      case '1':
        if(level == 0)
        {
          level = 1;
          difficulty = "1";                         
          
        }
        
        my_alt_print("\r\n\n");
        game(level, hp);
        
        my_alt_print("\n\rWould you like to play again(Y or N): ");
        play_choice = inchar1();
        outchar1(play_choice);
        if(play_choice == 'n' || play_choice == 'N')
        {
          user_input = '4';
        }
        else
        {
          break;
        }

        break;
      case '2':
        if(difficulty == "0")
        {
          my_alt_print("\n\rTo select difficulty, press SW3(Easy), SW4(Medium), or SW5(Hard)");
        } 
        else
        {
          my_alt_print("\n\rChoose a new difficulty, press SW3(Easy), SW4(Medium), or SW5(Hard)");
          difficulty = "0";
          level = 0;
          WAITING = TRUE;
        }
         
        while(WAITING)
        {
          if(SW3_down())
          {
            WAITING = FALSE;
            ms_delay(50);
            difficulty = "1";
            level = 1;
          } 
          else if(SW4_down()) 
          {
            WAITING = FALSE;
            ms_delay(50);
            difficulty = "2";
            level = 2;

          }
          else if(SW5_down())
          {
            WAITING = FALSE;
            ms_delay(50);
            difficulty = "3";
            level = 3;
          }

        }
       
        my_alt_print("\n\rYou have selected level: ");
        my_alt_print(difficulty);
       
       
        ms_delay(2000);
        alt_clear();
       
        break;
        
        
      case '3':
        alt_clear();
        car_input = car_menu();
        switch(car_input)
        {
          case '1':
             my_alt_print("\r\nYou Selected: BMW 1 Series M");
             hp = 350;
             break;
          
          case '2':
             my_alt_print("\r\nYou Selected: Pontiac G8 GXP");
             hp = 415;
             break;
          
          
          case '3':
             my_alt_print("\r\nYou Selected: Audi RS5");
             hp = 450;
             break;
          
          case '4':
             my_alt_print("\r\nYou Selected: Corvette Z06");
             hp = 505;
             break;
          default:
             my_alt_print("\r\nYou Selected: BMW 1 Series M");
             hp = 350;
          
        }
        
        ms_delay(2000);
        
        
        break;
      case '4':
      
        my_alt_print("/r/nThank you for playing!");
        break;
      
      default:
        my_alt_print("Please enter 1-4");
        continue;
     }
  }

  alt_clear();

}
      
// Light sequence to start the race
void startlights(void){
  int i;
  motor4(lighton);  //flash Red
  ms_delay(500);
  motor4(0);
  ms_delay(500);
  motor4(lighton);  //flash Orange
  motor6(lightondim);
  ms_delay(500);
  motor4(0);
  motor6(0);
  ms_delay(500);
  motor4(lighton);  //flash Yellow
  motor6(lighton);
  ms_delay(500);
  motor4(0);
  motor6(0);
  ms_delay(500);
   for(i=0; i<3; i++)
   { //flash Green 3x
      motor6(lighton);
      ms_delay(100);
      motor6(0);
      ms_delay(100);
   }
}


// Light sequence to end the race
void finishlights(void){
   int j;
   for(j=0; j<3; j++){  //flash White 3x
      motor4(lighton);
      motor5(lighton);
      motor6(lighton);
      ms_delay(150);
      motor4(0);
      motor5(0);
      motor6(0);
      ms_delay(150);
   }
}


// Upshift gears
int upshift(short gear){
   if(gear<6)
   {
    gear++;
   }
   return gear; 
}


// Downshift gears
int downshift(short gear)
{
  if(gear>1)
  {
    gear--;
  }
  return gear;
}



void collision()
{
  char* collision_msg1 = "Collision";
  char* collision_msg2 = "Detected!";
  clear_lcd();
  set_lcd_addr(0x04);
  type_lcd(collision_msg1);
  set_lcd_addr(0x44);
  type_lcd(collision_msg2);
  finished = TRUE;
  motor1(0);
  motor4(0);
  motor5(0);
  motor6(0);
  for(i=0; i<3; i++)
  {
    motor4(lighton);
    ms_delay(500);
    motor4(0);
    ms_delay(500);
  }
  ms_delay(500);
  leds_off();
  seg7s_off();
  
  ms_delay(3000);
  clear_lcd();
  moves = 0;
}




// Calculate and return the RPM value
int getRPM(int joystick_x, int rpm, int rpm_low_limit, int rpm_high_limit, double rpm_hp_factor){
    if(joystick_x>550){
      rpm += (joystick_x - 513) * rpm_hp_factor;
      
      if(rpm>=rpm_high_limit){
        rpm=rpm_high_limit;
      }
    }

    rpm-=450;
     
    if(rpm<=rpm_low_limit){
      rpm=rpm_low_limit;
    }
    return rpm;
}

// Calculate and return the speed value
int getSpeed(int joystick_x, int speed, int speedlimit, int maxspeed, double spd_hp_factor){
  
  // Accelerator
  if(joystick_x>550){
    speed += ((joystick_x - 513)/(85 * (0.005*speed+1)))*spd_hp_factor;
    
    // Upper speed limit boundary (+3 is to account for coasting deceleration)
    if(speed>speedlimit+3){
     speed = speedlimit+3;
    }
  }
  
  // Brakes
  else if(joystick_x<480){
   speed -= (513-joystick_x)/50;
  }
  
  // Slowly coast/decelerate when no joystick motion
  speed -=3;
  
  // Make sure speed can't go below the low threshold for current gear
  if(speed<speedlimit-(maxspeed/6)){
   speed=speedlimit-(maxspeed/6);
  }
  return speed;
}
  






void race(int level, int hp) {
  
  char* rpm_message = "RPM: ";
  char* spd_message = "MPH";
  char* gear_message = "G";
  int speed=0;
  short gear = 1;
  uint16 joystick_x;
  uint16 joystick_y;
  uint16 joystick_z;
  int rpm = 0;
  int motorspd = 0;
  int maxspeed = hp * 0.4;
  int speedlimit = maxspeed/6;
  int rpm_high_limit = 8000 - ((gear-1)*800);
  int rpm_low_limit = 1000 + ((gear-1)*400);
  int distance = 0;
  double spd_hp_factor = 0.70 + (hp / 500.0);
  double rpm_hp_factor = ((1.0 + (hp/1000.0))*1.5);
  double speed_inc = 0;
  finished=FALSE;
  
  
  startlights();
  
  while(!finished){
     
     leds_on(SW1_dip());
     joystick_x=ad1conv(3);
     joystick_y=ad1conv(4);
     joystick_z = ad0conv(5);
     
     speedlimit = gear * (maxspeed/6);
     rpm_high_limit = 8000 - ((gear-1)*400);
     rpm_low_limit = 1000 + ((gear-1)*400);

     // Get rpm and speed input from the joystick
     rpm=getRPM(joystick_x, rpm, rpm_low_limit, rpm_high_limit, rpm_hp_factor);
     speed=getSpeed(joystick_x, speed, speedlimit, maxspeed, spd_hp_factor);
     
     
    
     
     // Alert the player when they should upshift
     if(speed==speedlimit)
     {
      motor6(lighton);
     }
     else
     {
      motor6(0);
     }
     
     if(joystick_y < 250)
     {
      motor5(lighton);
      move_y_up();
     }
     else if(joystick_y > 800)
     {
      motor4(lighton);
      move_y_down();
     }
     else
     {
      motor5(0);
      motor4(0); 
     }
     
     
     // Drive the motor based on rpm
     motorspd = rpm/40;
     motor1(motorspd);
     
     if((joystick_z < 400)  && ((joystick_x < 750) && (joystick_x > 280)) && (speed>=(speedlimit-7)))
     {
      if(gear<6)
      {
        gear++;
        rpm-=2000;
      }
     }
     
     if((joystick_z > 600) && ((joystick_x < 750) && (joystick_x > 280)) && (speed<=(speedlimit-(maxspeed/6)+3)))
     {
      if(gear>1)
      {
        gear--;
        rpm+=2000;
      }
     }
     // Display RPM messages to dashboard
     set_lcd_addr(0x00);
     type_lcd(rpm_message);
     set_lcd_addr(0x04);
     write_int_lcd(rpm);
     set_lcd_addr(0x0B);
     write_int_lcd(gear);
     set_lcd_addr(0x0A);
     type_lcd(gear_message);
     // Display speed messages to dashboard
     set_lcd_addr(0x40);
     write_int_lcd(speed);
     set_lcd_addr(0x46);
     type_lcd(spd_message);

     
     if(speed>200)
     {
      shift_left(level);
      shift_left(level);
      distance+=(speed-200);
     }
     else if(speed>100)
     {
      shift_left(level);
      distance+=(speed-100);
     }
     else
     {
      distance+=speed;
     }
     
     if(distance>=100)
    {
      distance=0;
      shift_left(level);
    
    }
     ms_delay(100);
     
    
  }
  
         

}



