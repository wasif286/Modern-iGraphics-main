#include "iGraphics.h"
#include<unistd.h>
#include<time.h>
#include<math.h>
#include <iostream>
#include "iSound.h"
using namespace std;
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 1000
int scoreP1 = 0;
int scoreP2 = 0;
int goal=0;
int goalanicount=0;
// Game States:
enum GameState { MENU, HELP, GAME };
int currentState = MENU;
int menuSelection = 0; // 0=Start, 1=Help, 2=Exit

double ball_x1=10,ball_y1=10,radius=15,dx=4,head_x=450,head_x2=550,head_y=70,head_y2=70,head_radius=20;
int ball_x =500,ball_y=200,timer_start=0,ball_touched_ceil=0,ball_shoot=0,kick_off=0,p2_jump=0,up=10,p1_jump=0,timer2=0,up1=10,up2=10;
double leg_top=15 ,leg_top2=15, leg_bottom=0, leg_bottom2=0,dy=3,time_count=0;

void jump()
{
   if(p2_jump==1)
   {if(head_y2 > 300)
        up1=-up1; 
    head_y2+=up1;
        leg_top2+=up1;
        leg_bottom2+=up1;
     if(head_y2 < 70) 
           { head_y2 = 70;
            leg_top2 = 15;
            leg_bottom2 =0;
            p2_jump =0;
            up1=-up1;
        }}
    if(p1_jump==1)
   {if(head_y > 300)
       up2=-up2; 
     head_y+=up2;
        leg_top+=up2;
        leg_bottom+=up2;
     if(head_y < 70) 
           { head_y = 70;
            leg_top = 15;
            leg_bottom =0;
            p1_jump =0;
        up2=-up2;}}
}
void ball_move()
{  
       if(kick_off ==0) {ball_x =500;
                      if(ball_y >=200 || ball_y <=head_radius) dy=-dy;
                       ball_y+=dy;
                        goal = 0;
                        goalanicount= 0;}
    if(kick_off == 1) ball_x+=dx;
   // This is the parabolic eq of the ball when headed.
   if(ball_touched_ceil == 0 && ball_shoot == 0 && kick_off ==1) 
   {
     ball_y= (ball_x-ball_x1-300)*(ball_x-ball_x1-300)/(-450) + ball_y1 + 200;
   }
   else if(ball_touched_ceil ==1) ball_y-=10;
   if((((ball_x > head_x)? (ball_x - head_x) : (head_x - ball_x) )<=40) && ball_y <= head_y + head_radius && ball_y >=leg_bottom)
        {   
            if(ball_x > head_x) dx = (dx>0)?dx:-dx;
            else dx = (dx>0)?-dx:dx;
            kick_off =1;
        if(dx>0) {ball_x1 = ball_x;
         ball_y1 = ball_y;}
        else {ball_x1 = ball_x-600;
         ball_y1 = ball_y;
        }
     ball_touched_ceil=0;
     ball_shoot =0;
        }
    else if((((ball_x > head_x2)? (ball_x - head_x2) : (head_x2 - ball_x) )<=40) && ball_y <= head_y2 + head_radius && ball_y >=leg_bottom2)
        {  
            if(ball_x > head_x2) dx = (dx>0)?dx:-dx;
            else dx = (dx>0)?-dx:dx;
             kick_off =1;
         if(dx<0){ball_x1 = ball_x-600;
         ball_y1 = ball_y;}
        else{
            ball_x1 = ball_x;
            ball_y1 = ball_y;
        }
     ball_touched_ceil=0;
     ball_shoot =0;
        }
   if(ball_x + radius>SCREEN_WIDTH ){
     ball_x = SCREEN_WIDTH - radius;
     ball_x1=ball_x-600;
     ball_y1 = ball_y;
     dx=-dx;
     ball_touched_ceil=0;
     ball_shoot=0;
   }
   if(ball_x > 950 && ball_y + radius > 150 && ball_y - radius < 150){
     ball_x1=ball_x-600;
     ball_y1 = ball_y;
     dx=-dx;
     ball_touched_ceil=0;
     ball_shoot=0; 
   }
   else if(ball_x < radius ){
     ball_x = radius;
     ball_x1 = ball_x;
     ball_y1 = ball_y;
     dx=-dx;
     ball_touched_ceil=0;
     ball_shoot=0;
   }
   if(ball_x < 50 && ball_y + radius > 150 && ball_y - radius < 150){
     ball_x1 = ball_x;
     ball_y1 = ball_y;
     dx=-dx;
     ball_touched_ceil=0;
     ball_shoot=0;
   }
   if(ball_y + radius > SCREEN_HEIGHT){
     ball_y = SCREEN_HEIGHT - radius;
     if(dx<0) ball_x1 = ball_x-400;
     else ball_x1 = ball_x;
     ball_y1 = ball_y;
     ball_touched_ceil=1;
     ball_shoot = 0;
   }
   if(ball_y < radius){
       if(dx>0){
        ball_y = radius;
        ball_x1 = ball_x;
        ball_y1 = ball_y;}
        else{
            ball_y = radius;
            ball_x1 = ball_x -600;
            ball_y1 = ball_y;
        }
    ball_touched_ceil=0;
    ball_shoot =0;
   }
    if((ball_x < 40 && ball_y + radius < 150) || (ball_x > 960 && ball_y + radius < 150))
    { 
        goal=1;
         goalanicount = 1;
        // iResumeTimer(1);  // Resume the goal animation timer

        if(ball_x < 40 && ball_y + radius < 150) {
            scoreP2++;
        } else if (ball_x > 960 && ball_y + radius < 150) {
            scoreP1++;
        }
     //sleep(1);
     ball_touched_ceil =0;
     ball_shoot =0;
     ball_x = 500, ball_y =200;
    kick_off =0;
    dy=3;
    p1_jump=0;
    p2_jump =0;
    up1=10;
    up2=10;
    head_y = 70;
    leg_top = 15;
    leg_bottom =0;
    head_y2 = 70;
    leg_top2 = 15;
    leg_bottom2 =0;
}
 time_count +=.01;
}
   


Image bg, goalim;
void loadResources()
{
	iLoadImage(&bg, "assets/images/footballbg.png");
     iResizeImage(&bg, SCREEN_WIDTH,SCREEN_HEIGHT);
     iLoadImage(&goalim, "assets/images/goal.png");
     iResizeImage(&goalim, SCREEN_WIDTH,SCREEN_HEIGHT);  // resize to match screen
	// iLoadImage(&mario, "assets/images/mario.png");
	// double marioAspectRatio = (double)mario.width / mario.height;
	// iResizeImage(&mario, 48, 48.0 / marioAspectRatio);
}
/*
function iDraw() is called again and again by the system.
*/
void iDraw() {
    iClear();
    
    if(currentState == MENU) {
        	iShowLoadedImage(0, 0, &bg);
        iSetColor(255, 255, 255);
iSetTransparentColor(255,255,255,0.2);
    iFilledRectangle(445,245,100,28);
    iFilledRectangle(445,295,100,28);
     iFilledRectangle(445,345,100,28);
       iSetColor(255, 255, 0);
        iText(400, 550, "2 Player Football Game", GLUT_BITMAP_TIMES_ROMAN_24);
        
        if(menuSelection == 0) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
        iText(450, 350, "Start Game", GLUT_BITMAP_HELVETICA_18);
        
        if(menuSelection == 1) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
        iText(450, 300, "   Help   ", GLUT_BITMAP_HELVETICA_18);
        
        if(menuSelection == 2) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
        iText(450, 250, "   Exit  ", GLUT_BITMAP_HELVETICA_18);
    }
    
    else if(currentState == HELP) {
        iSetColor(255, 255, 0);
        iText(400, 500, "Help / Instructions", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 255, 255);
        iText(200, 450, "Player 1 : WASD to move, S=Shoot, H=Header");
        iText(200, 400, "Player 2 : Arrow Keys to move, K=Shoot, L=Header");
        iText(200, 350, "First to score wins");
        iText(400, 200, "Press 'b' to go back to Menu");
    }
    

    
    else if(currentState == GAME) {

 


        
    iSetColor(135,206,235);
    iFilledRectangle(0,150,1000,450);
    iSetColor(154,236,153);
     iFilledRectangle(50,0,900,150);
    iSetTransparentColor(255,255,255,0.5);
    iFilledRectangle(0,0,45,140);
    iFilledRectangle(955,0,45,140);
    iSetColor(0,0,0);
    iFilledRectangle(0,140,50,10);
    iFilledRectangle(45,0,5,40);
    iFilledRectangle(950,140,50,10);
    iFilledRectangle(950,0,5,40);
    iSetColor(255,0,0);
    iFilledCircle(ball_x,ball_y,radius);
    // place your drawing codes here
    iSetColor(0,0,0);
// This is the head of player1
    iFilledCircle(head_x,head_y,head_radius);
// This is the body
    iFilledRectangle(head_x - head_radius,leg_top,2*head_radius,head_y - leg_top - head_radius);
// Leg
    iLine(head_x,leg_top,head_x - 20 ,leg_bottom);
    iLine(head_x,leg_top,head_x + 20 ,leg_bottom);
    iSetColor(0,0,255);
    // This is the head of player2
    iFilledCircle(head_x2,head_y2,head_radius);
// This is the body 2
    iFilledRectangle(head_x2 - head_radius,leg_top2,2*head_radius,head_y2 - leg_top2 - head_radius);
// Leg 2
    iLine(head_x2,leg_top2,head_x2 - 20 ,leg_bottom2);
    iLine(head_x2,leg_top2,head_x2 + 20 ,leg_bottom2);
    iSetColor(255,0,0);
    iFilledCircle(ball_x,ball_y,radius);
    // Draw background bar
    iSetColor(0, 0, 0);  // black background
    iFilledRectangle(390, 570, 230, 30);

    // Set text color
    iSetColor(255, 255, 255);  // white text

    // Draw score texts
    char scoreText1[50];
    char scoreText2[50];
    char time[50];
    sprintf(scoreText1, "Player 1: %d", scoreP1);
    sprintf(scoreText2, "Player 2: %d", scoreP2);
    sprintf(time,"\n   TIME: %0.2f",time_count);

    iText(420, 580, scoreText1, GLUT_BITMAP_HELVETICA_18);
    iText(520, 580, scoreText2, GLUT_BITMAP_HELVETICA_18);
    iText(660,580,time,GLUT_BITMAP_HELVETICA_18);
   

if (goal) {
  iShowLoadedImage(0, 0, &goalim);  
  goalanicount++;

  if (goalanicount >= 50) { 
    goal = 0;
     goalanicount = 0;}
    
}

}}
/*
function iMouseMove() is called when the user moves the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    // place your codes here
}

/*
function iMouseDrag() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseDrag(int mx, int my)
{
    // place your codes here
}

/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
    }
}

/*
function iMouseWheel() is called when the user scrolls the mouse wheel.
dir = 1 for up, -1 for down.
*/
void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/

void iKeyboard(unsigned char key)
{
if(currentState == MENU) {
        if(key == '\r') { // ENTER pressed
            if(menuSelection == 0) currentState = GAME;
            if(menuSelection == 1) currentState = HELP;
            if(menuSelection == 2) exit(0);
        }
    }
    
    else if(currentState == HELP) {
        if(key == 'b') {iPauseTimer(0); currentState = MENU;}
    }
else if(currentState == GAME) {
      
    switch (key)
    {
        case 'b':
{    iPauseTimer(0);  // pause ball movement timer
    currentState = MENU;
    break;}
      

    case 'q':{
        // do something with 'q'
        iPauseTimer(0);
        break;}
    case 'r':{
        iResumeTimer(0);
        break;
    }
       
    case 's':{
        if((((ball_x > head_x)? (ball_x - head_x) : (head_x - ball_x) )<=40) && ball_y <= head_y + head_radius && ball_y >=leg_bottom)
        {
               kick_off =1;
        ball_shoot =1, ball_touched_ceil=0;
        dx=(dx>0)?dx:-dx;
        }break;
    }

    case 'w':{
       p1_jump =1;
    break;}
    
    case 'a' :{
        if(head_x>=30)
        head_x-=30;
        else head_x = 25;
        break;
    }
    case 'd':{
         if(head_x <= SCREEN_WIDTH - 30)
        head_x+=30;
        else head_x = SCREEN_WIDTH - head_radius;
        break;
    }

       
    case 'k':{
        if((((ball_x > head_x2)? (ball_x - head_x2) : (head_x2 - ball_x) )<=40) && ball_y <= head_y2 + head_radius && ball_y >=leg_bottom2)
        {
         kick_off =1;
        ball_shoot =1,ball_touched_ceil =0;
        dx=(dx<0)?dx:-dx;
        }break;
    }
}
   
}
}

/*
function iSpecialKeyboard() is called whenver user hits special keys likefunction
keys, home, end, pg up, pg down, arraows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
GLUT_KEY_INSERT */
void iSpecialKeyboard(unsigned char key)
{
     if(currentState == MENU) {
        if(key == GLUT_KEY_DOWN) {
            menuSelection = (menuSelection + 1) % 3;
        }
        if(key == GLUT_KEY_UP) {
            menuSelection = (menuSelection - 1 + 3) % 3;
        }
    }

    if(currentState == GAME) {
    switch (key)
    {
    case GLUT_KEY_END:
        // do something
        break;
    // place your codes for other keys here
    case GLUT_KEY_UP:{
     p2_jump =1;
    break;}
    case GLUT_KEY_LEFT:{
      if(head_x2>=30)
        head_x2-=30;
        else head_x2 = 25;
        break;}
    case GLUT_KEY_RIGHT:{
        if(head_x2 <= SCREEN_WIDTH - 30)
        head_x2+=30;
        else head_x2 = SCREEN_WIDTH - head_radius;
        break;}
    default:
        break;
    }
}
       
    }
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    loadResources();
    //iSetTimer(20, goalAnimationUpdate);  // Timer 1 for animation
   //  iPauseTimer(1);
     if(timer_start==0 ){
                iSetTimer(10,ball_move);
               timer_start =1;}
    if(timer2==0) {iSetTimer(20,jump);
                   timer2 =1;}
    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Soccer");
    return 0;
}

