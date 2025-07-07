#include "iGraphics.h"
#include<unistd.h>
#include<math.h>
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 1000
double ball_x1=10,ball_y1=10,radius=15,dx=5,head_x=450,head_x2=550,head_y=120,head_y2=120,head_radius=25;
int ball_x =500,ball_y=0,timer_start=0,ball_touched_ceil=0,ball_shoot=0,kick_off=0;
double leg_top=30 ,leg_top2=30, leg_bottom=0, leg_bottom2=0;
/*
function iDraw() is called again and again by the system.
*/
void ball_move()
{  
    if(kick_off == 1) ball_x+=dx;
   // This is the parabolic eq of the ball when headed.
   if(ball_touched_ceil == 0 && ball_shoot == 0 && kick_off ==1) ball_y= (ball_x-ball_x1-400)*(ball_x-ball_x1-400)/(-800) + ball_y1 + 200;
   else if(ball_touched_ceil ==1) ball_y-=10;
   if(ball_x + radius>SCREEN_WIDTH ){
     ball_x = SCREEN_WIDTH - radius;
     ball_x1=ball_x-800;
     ball_y1 = ball_y;
     dx=-dx;
     ball_touched_ceil=0;
     ball_shoot=0;
   }
   if(ball_x > 950 && ball_y + radius > 150 && ball_y - radius < 150){
     ball_x1=ball_x-800;
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
     if(dx<0) ball_x1 = ball_x-800;
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
            ball_x1 = ball_x -800;
            ball_y1 = ball_y;
        }
    ball_touched_ceil=0;
    ball_shoot =0;
   }
    if((ball_x < 40 && ball_y + radius < 150) || (ball_x > 960 && ball_y + radius < 150))
    { 
     sleep(1);
     ball_touched_ceil =0;
     ball_shoot =0;
     ball_x = 500, ball_y =0;
    kick_off =0;}
   
}
void iDraw()
{   iClear();
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
}

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
    switch (key)
    {
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
        {if(timer_start==0 ){
                iSetTimer(10,ball_move);
               timer_start =1;}
        if(kick_off == 10) kick_off =1;
        ball_shoot =1, ball_touched_ceil=0;
        dx=(dx>0)?dx:-dx;
        }break;
    }
    case '\r':{
        if(kick_off ==0) {ball_x =500,ball_y =0;
                         kick_off =10;}
        break;
    }
    case 'w':{
        if(head_y <= SCREEN_HEIGHT - 30)
       { head_y+=30;
        leg_top+=30;
        leg_bottom+=30;}
        else{ head_y = SCREEN_HEIGHT - head_radius;
        leg_top = head_y - 90;
        leg_bottom = leg_top - 30; }
        break;
    }
    case 'z':{
        if(head_y >=150) {head_y-=30;
        leg_top-=30;
        leg_bottom-=30;}
        else {
            head_y = 120;
            leg_top = 30;
            leg_bottom =0;
        }
        break;
    }
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
    case 'h':{
         if((((ball_x > head_x)? (ball_x - head_x) : (head_x - ball_x) )<=40) && ball_y <= head_y + head_radius && ball_y >=leg_bottom)
        {
            if(kick_off == 10) kick_off =1;
            if(timer_start==0 ){
                iSetTimer(10,ball_move);
               timer_start =1;}
        if(dx<0) dx =-dx;
         ball_x1 = ball_x;
         ball_y1 = ball_y;
     ball_touched_ceil=0;
     ball_shoot =0;
        }
        break;
    }
    case 'l':{
        if((((ball_x > head_x2)? (ball_x - head_x2) : (head_x2 - ball_x) )<=40) && ball_y <= head_y2 + head_radius && ball_y >=leg_bottom2)
        {
            if(kick_off == 10) kick_off =1;
            if(timer_start==0 ){
                iSetTimer(10,ball_move);
               timer_start =1;}
        if(dx>0) dx =-dx;
         ball_x1 = ball_x-800;
         ball_y1 = ball_y;
     ball_touched_ceil=0;
     ball_shoot =0;
        }
        break;
    }
    case 'k':{
        if((((ball_x > head_x2)? (ball_x - head_x2) : (head_x2 - ball_x) )<=40) && ball_y <= head_y2 + head_radius && ball_y >=leg_bottom2)
        {if(timer_start==0 ){
                iSetTimer(10,ball_move);
               timer_start =1;}
        if(kick_off == 10) kick_off =1;
        ball_shoot =1,ball_touched_ceil =0;
        dx=(dx<0)?dx:-dx;
        }break;
    }
    // place your codes for other keys here
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
    switch (key)
    {
    case GLUT_KEY_END:
        // do something
        break;
    // place your codes for other keys here
    case GLUT_KEY_UP:{
     if(head_y2 <= SCREEN_HEIGHT - 30)
       { head_y2+=30;
        leg_top2+=30;
        leg_bottom2+=30;}
        else{ head_y2 = SCREEN_HEIGHT - head_radius;
        leg_top2 = head_y2 - 90;
        leg_bottom2 = leg_top2 - 30; }
        break;}
    case GLUT_KEY_DOWN:{
       if(head_y2 >=150) {head_y2-=30;
        leg_top2-=30;
        leg_bottom2-=30;}
        else {
            head_y2 = 120;
            leg_top2 = 30;
            leg_bottom2 =0;
        }
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

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    // place your own initialization codes here.
    iInitialize(SCREEN_WIDTH,SCREEN_HEIGHT, "Simple Soccer");
    return 0;
}