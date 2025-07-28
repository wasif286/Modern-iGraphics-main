#include "iGraphics.h"
#include<unistd.h>
#include<time.h>
#include<math.h>
#include <iostream>
#include "iSound.h"
#include <fcntl.h>
using namespace std;
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 1000
int menuSoundPlayed = 0;
int kickSoundChannel = -1;
int scoreP1 = 0;
int scoreP2 = 0;
int goal=0;
int goalanicount=0;
int isShowingGoalAnimation = 0;
int current_game=0;
int isGoldenGoal = 0;
// Game States:
enum GameState {MENU,GAME,CONTINUE,PRACTICE,GOLDEN_GOAL,LEADREBOARD, HELP};
int currentState=MENU ;
int menuSelection = 0; // 0=Start, 1=Continue ,2=practice,3=golden_goal, 4=Leaderboard,5=Help, 6= Exit

double ball_x1=10,ball_y1=10,radius=15,dx=5,head_x=450,head_x2=550,head_y=100,head_y2=100,head_radius=25;
int ball_x =500,ball_y=200,timer_start=0,ball_touched_ceil=0,ball_shoot=0,kick_off=0,p2_jump=0,up=10,p1_jump=0,timer2=0,ball_touched_ground=0;
double leg_top=30 ,leg_top2=30, leg_bottom=0, leg_bottom2=0,dy=3,time_count=0;
int leaderboard_update = 0, gg_end= 0;
char input_name[30];
int input_index= 0,taking_input=0;
int goal_diff[5];
int flag=0,flag2=0; //to not call leaderboard update too many times unnecessarily
typedef struct {
    char name[30];
    int win;
}player;
int a[5], b[5];
char s1[5][20], s2[5][20];
player player1,player2;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void sort_leaderboard()
{
    int i,j;
    for(i=0;i<5;i++) goal_diff[i] = abs(a[i]-b[i]);
    for(i=0;i<4;i++)
        for(j=i+1;j<5;j++)
            if(goal_diff[j]>goal_diff[i])
            {
                int t;
                char c[30];
                t=  goal_diff[i];
                goal_diff[i] = goal_diff[j];
                goal_diff[j] = t;
                t= a[i];
                a[i]=a[j];
                a[j]=t;
                t=b[i];
                b[i]=b[j];
                b[j]=t;
                strcpy(c,s1[i]);
                strcpy(s1[i],s1[j]);
                strcpy(s1[j],c);
                strcpy(c,s2[i]);
                strcpy(s2[i],s2[j]);
                strcpy(s2[j],c);
            }
}

void update_leaderboard(int diff, char name1[], char name2[])
{
    FILE *fp = fopen("leaderboard.txt", "r");
    if (!fp) return;

    int i = 0, min = 1000, min_idx = -1;

    while (i < 5 && fscanf(fp, "%s %d-%d %s", s1[i], &a[i], &b[i], s2[i]) == 4) {
        int curr_diff = abs(a[i] - b[i]);
        if (curr_diff < min) {
            min = curr_diff;
            min_idx = i;
        }
        i++;
    }
    fclose(fp);

    int entry_count = i; // Number of valid lines read

    if (entry_count < 5) {
        // Fill next slot with current match
        strcpy(s1[entry_count], name1);
        strcpy(s2[entry_count], name2);
        a[entry_count] = scoreP1;
        b[entry_count] = scoreP2;
        entry_count++;
        printf("Inserted as new entry (fewer than 5 entries).\n");
    }
    else if (min_idx != -1 && (diff > min || (diff == min && (scoreP1 + scoreP2 > a[min_idx] + b[min_idx])))) {
        strcpy(s1[min_idx], name1);
        strcpy(s2[min_idx], name2);
        a[min_idx] = scoreP1;
        b[min_idx] = scoreP2;
    }

    // Write back only the valid part
    fp = fopen("leaderboard.txt", "w");
    for (i = 0; i < entry_count; i++)
        fprintf(fp, "%s %d-%d %s\n", s1[i], a[i], b[i], s2[i]);
    fclose(fp);
    fp= fopen("leaderboard.txt", "r");
    i=0;
    while (i < 5 && fscanf(fp, "%s %d-%d %s", s1[i], &a[i], &b[i], s2[i]) == 4)
     i++;
    fclose(fp);
}
void jump()
{ // when heading while jumping player should not go up to avoid multitple collisions
   if(p2_jump==1)
   {if ((head_y2 > 300) || 
    ((((ball_x > head_x2 ? (ball_x - head_x2) : (head_x2 - ball_x)) <= 40) &&
    (ball_y <= head_y2 + head_radius) && 
    (ball_y >= leg_bottom2))))
        up=-up; 
    head_y2+=up;
        leg_top2+=up;
        leg_bottom2+=up;
     if(head_y2 < 100) 
           { head_y2 = 100;
            leg_top2 = 30;
            leg_bottom2 =0;
            p2_jump =0;
            up=-up;
        }}
    if(p1_jump==1)
   {if ((head_y > 300) || 
    ((((ball_x > head_x ? (ball_x - head_x) : (head_x - ball_x)) <= 40) &&
    (ball_y <= head_y + head_radius) && 
    (ball_y >= leg_bottom))))
       up=-up; 
     head_y+=up;
        leg_top+=up;
        leg_bottom+=up;
     if(head_y < 100) 
           { head_y = 100;
            leg_top = 30;
            leg_bottom =0;
            p1_jump =0;
        up=-up;}}
           }

void ball_move()
{    if (isShowingGoalAnimation) return; 
    if(kick_off ==0) {ball_x =500;
                      if(ball_y >=200 || ball_y <=head_radius) dy=-dy;
                       ball_y+=dy;
                        goal = 0;
                        goalanicount= 0;}
    if(kick_off == 1) ball_x+=dx;
   // This is the parabolic eq of the ball when headed.
   if(ball_touched_ceil == 0 && ball_touched_ground ==0 && ball_shoot == 0 && kick_off ==1) ball_y= (ball_x-ball_x1-300)*(ball_x-ball_x1-300)/(-450) + ball_y1 + 200;
   else if(ball_touched_ground==1)  ball_y= (ball_x-ball_x1-300)*(ball_x-ball_x1-300)/(-900) + ball_y1 + 100;
   else if(ball_touched_ceil ==1) ball_y-=10;
   if(currentState!=PRACTICE    && (((ball_x > head_x)? (ball_x - head_x) : (head_x - ball_x) )<=40) && ball_y <= head_y + head_radius && ball_y >=leg_bottom)
        {   
            //iStopSound();
               iPlaySound("assets/sounds/kick.wav", false,100);
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
     ball_touched_ground=0;
        }
    else if((((ball_x > head_x2)? (ball_x - head_x2) : (head_x2 - ball_x) )<=40) && ball_y <= head_y2 + head_radius && ball_y >=leg_bottom2)
        {
            // iStopSound();
               iPlaySound("assets/sounds/kick.wav", false,100);
             if(ball_x > head_x2) dx = (dx>0)?dx:-dx;
            else dx = (dx>0)?-dx:dx;
            // iPlaySound("assets/sounds/kick.wav", false);
            
             kick_off =1;
         if(dx<0){ball_x1 = ball_x-600;
         ball_y1 = ball_y;}
        else{
            ball_x1 = ball_x;
            ball_y1 = ball_y;
        }
     ball_touched_ceil=0;
     ball_shoot =0;
     ball_touched_ground=0;
        }
   if(ball_x + radius>SCREEN_WIDTH ){
    iPlaySound("assets/sounds/kick.wav", false,100);
     ball_x = SCREEN_WIDTH - radius;
     ball_x1=ball_x-600;
     ball_y1 = ball_y;
     dx=-dx;
     ball_touched_ceil=0;
     ball_shoot=0;
   }
   if(ball_x > 940 && ball_y + radius > 160 && ball_y - radius < 160){
    iPlaySound("assets/sounds/kick.wav", false,100);
     ball_x1=ball_x-600;
     ball_y1 = ball_y;
     dx=-dx;
     ball_touched_ceil=0;
     ball_shoot=0; 

   }
   else if(ball_x < radius ){
    iPlaySound("assets/sounds/kick.wav", false,100);
     ball_x = radius;
     ball_x1 = ball_x;
     ball_y1 = ball_y;
     dx=-dx;
     ball_touched_ceil=0;
     ball_shoot=0;
   }
   if(ball_x <60  && ball_y + radius > 160 && ball_y - radius < 160){
    iPlaySound("assets/sounds/kick.wav", false,100);
     ball_x1 = ball_x;
     ball_y1 = ball_y;
     dx=-dx;
     ball_touched_ceil=0;
     ball_shoot=0;
   }
   if(ball_y + radius > SCREEN_HEIGHT){
    iPlaySound("assets/sounds/kick.wav", false,100);
     ball_y = SCREEN_HEIGHT - radius;
     if(dx<0) ball_x1 = ball_x-400;
     else ball_x1 = ball_x;
     ball_y1 = ball_y;
     ball_touched_ceil=1;
     ball_shoot = 0;
   }
   if(ball_y < radius){
    iPlaySound("assets/sounds/kick.wav", false,100);
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
    ball_touched_ground=1;
   }
  
  
  if(currentState==GAME)
   { if((ball_x < 40 && ball_y + radius < 150) || (ball_x > 960 && ball_y + radius < 150))
    { 
         iPlaySound("assets/sounds/goal.wav", false,100);
        goal=1;
         goalanicount = 100;
             isShowingGoalAnimation = 1; 
         
        // iResumeTimer(1);  // Resume the goal animation timer

        if(ball_x < 40 && ball_y + radius < 150) {
            scoreP2++;
        } else if (ball_x > 960 && ball_y + radius < 150) {
            scoreP1++;
        }
        if(isGoldenGoal) gg_end =1;
    
     ball_x1=10,ball_y1=10,radius=15,dx=5,head_x=450,head_x2=550,head_y=100,head_y2=100,head_radius=25;
 ball_x =500,ball_y=200,ball_touched_ceil=0,ball_shoot=0,kick_off=0,p2_jump=0,up=10,p1_jump=0;
leg_top=30 ,leg_top2=30, leg_bottom=0, leg_bottom2=0,dy=3,ball_touched_ground=0;

}}
else if (currentState==PRACTICE)
   { if((ball_x < 40 && ball_y + radius < 150) )
    { 
         iPlaySound("assets/sounds/goal.wav", false,100);
        goal=1;
         goalanicount = 100;
             isShowingGoalAnimation = 1; 
         
        // iResumeTimer(1);  // Resume the goal animation timer

        if(ball_x < 40 && ball_y + radius < 150) {
            scoreP2++;
        } else if (ball_x > 960 && ball_y + radius < 150) {
            scoreP1++;
        }
     
     ball_x1=10,ball_y1=10,radius=15,dx=5,head_x=450,head_x2=550,head_y=100,head_y2=100,head_radius=25;
 ball_x =500,ball_y=200,ball_touched_ceil=0,ball_shoot=0,kick_off=0,p2_jump=0,up=10,p1_jump=0;
leg_top=30 ,leg_top2=30, leg_bottom=0, leg_bottom2=0,dy=3,ball_touched_ground=0;

}}

if(currentState==GAME && !isGoldenGoal) time_count +=.01;

}
   


Image bg, goalim,ball,field,field2;
void loadResources()
{
	iLoadImage(&bg, "assets/images/footballbg.png");
     iResizeImage(&bg, 1000, 600);
     iLoadImage(&goalim, "assets/images/goal.png");
     iResizeImage(&goalim, 1000, 600);
     iLoadImage(&ball, "assets/images/SoccerBall.png");
     iResizeImage(&ball, 30, 30);
     iLoadImage(&field, "assets/images/field.png");
     iResizeImage(&field, 1000, 600);
     iLoadImage(&field2, "assets/images/field.png");
     iResizeImage(&field2, 1000, 600);

 
}
/*
function iDraw() is called again and again by the system.
*/
void iDraw() {


    iClear();
    
    if(currentState == MENU) {
        if(!menuSoundPlayed) {
            iPlaySound("assets/sounds/footballbg.wav", true,50); // true = loop
            menuSoundPlayed = 1;
        }

        // your menu drawing code...

     

        
        	iShowLoadedImage(0, 0, &bg);
        iSetColor(255, 255, 255);
iSetTransparentColor(255,255,255,0.2);
 iFilledRectangle(445,445,150,28);
 iFilledRectangle(445,405,150,28);
 iFilledRectangle(445,365,150,28);
 iFilledRectangle(445,325,150,28);
 iFilledRectangle(445,285,150,28);
 iFilledRectangle(445,245,150,28);
 iFilledRectangle(445,205,150,28);
       iSetColor(255, 255, 0);
        iText(400, 550, "2 Player Football Game", GLUT_BITMAP_TIMES_ROMAN_24);
        
        // if(menuSelection == 0) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
        // iText(450, 400, "Start Game", GLUT_BITMAP_HELVETICA_18);

        
    if(menuSelection == 0) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
iText(450, 450, "Start Match", GLUT_BITMAP_HELVETICA_18);
if(menuSelection == 1) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
iText(450, 410, "Continue", GLUT_BITMAP_HELVETICA_18);
if(menuSelection == 2) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
iText(450,370 , "Practice Match", GLUT_BITMAP_HELVETICA_18);
if(menuSelection == 3) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
iText(450, 330, "Golden Goal", GLUT_BITMAP_HELVETICA_18);
if(menuSelection == 4) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
iText(450, 290, "leaderboard", GLUT_BITMAP_HELVETICA_18);

if(menuSelection == 5) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
iText(450, 250, "HELP", GLUT_BITMAP_HELVETICA_18);
if(menuSelection == 6) iSetColor(255, 0, 0); else iSetColor(255, 255, 255);
iText(450, 210, "EXIT", GLUT_BITMAP_HELVETICA_18);
    
    
    }
    
    else if(currentState == HELP) {
        iSetColor(255, 255, 0);
        iText(400, 500, "Help / Instructions", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 255, 255);
        iText(200, 450, "Player 1 : WASD to move");
        iText(200, 400, "Player 2 : Arrow Keys to move");
        iText(400, 200, "Press 'b' to go back to Menu");
    }
    else if(currentState==LEADREBOARD)
    { 
        iSetColor(255, 255, 0);
        iText(400, 500, "Top 5 matches (Hall of fame)", GLUT_BITMAP_TIMES_ROMAN_24);
        if(flag==0){update_leaderboard(-1,player1.name,player2.name);
            flag=1;}
        //sort the leaderboard
        sort_leaderboard();
    for (int i = 0; i <5; i++) {
        char line[200];
        sprintf(line,"%s %d-%d %s",s1[i],a[i],b[i],s2[i]);
        iText(300, 450 - i * 30, line, GLUT_BITMAP_HELVETICA_18);
    }
}

    
    else if(currentState == PRACTICE) {
    iShowLoadedImage(0, 0, &field2);

    // Single Player on right corner
    iSetColor(0, 0, 255);
    iFilledCircle(head_x2, head_y2, head_radius); // Player 2
    iFilledRectangle(head_x2 - head_radius, leg_top2, 2 * head_radius, head_y2 - leg_top2 - head_radius);
    iLine(head_x2, leg_top2, head_x2 - 20, leg_bottom2);
    iLine(head_x2, leg_top2, head_x2 + 20, leg_bottom2);

    // Ball
    iSetColor(255, 0, 0);
    iFilledCircle(ball_x, ball_y, radius);
    iShowLoadedImage(ball_x - 15, ball_y - 15, &ball);

    // Scoreboard (only score, no time)
    iSetColor(0, 0, 0);  // black bar
    iFilledRectangle(420, 570, 160, 30);
    iSetColor(255, 255, 255);  // text

    char scoreText1[20];
    sprintf(scoreText1, "Score: %d", scoreP2);
    iText(460, 580, scoreText1, GLUT_BITMAP_HELVETICA_18);

    if(goal) {
        iShowLoadedImage(0, 0, &goalim);
        goalanicount--;
        if (goalanicount <= 0) {
            goal = 0;
            isShowingGoalAnimation = 0;
            goalanicount = 0;
        }
    }
}



    
    else if(currentState == GAME) {



    if(taking_input&&!current_game){
    iClear();
    iSetColor(255, 255, 255);

    if (taking_input == 1) {
        iText(300, 400, "Enter Player 1 Name:", GLUT_BITMAP_TIMES_ROMAN_24);
    } else if (taking_input == 2) {
        iText(300, 400, "Enter Player 2 Name:", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    iText(300, 350, input_name, GLUT_BITMAP_HELVETICA_18);
    return;
}

 


        
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

        	iShowLoadedImage(0,0, &field);
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
    iShowLoadedImage(ball_x-15, ball_y-15, &ball);
    // Draw background bar
    iSetColor(0, 0, 0);  // black background/
    iFilledRectangle(390, 570, 230, 30);

    // Set text color
   
    // Draw score texts
    char scoreText1[20];
    char scoreText2[20];
    char time[50];
    
     iSetColor(255, 0, 0);  // red text

     sprintf(time,"   TIME: %0.2f",time_count);
    iSetColor(255, 255, 255);  // white text

    sprintf(scoreText1, "%s: %d ",player1.name, scoreP1);
    sprintf(scoreText2, "%s: %d ",player2.name, scoreP2);
    

    iText(420, 580, scoreText1, GLUT_BITMAP_HELVETICA_18);
    iText(520, 580, scoreText2, GLUT_BITMAP_HELVETICA_18);


if(gg_end )
{
     current_game=0;


    iPauseTimer(0);
    char win_text [50];
    iSetColor(0, 0, 0);  // black background/
    iFilledRectangle(100, 30, 800, 540);
    iSetColor(255, 255, 255);
if(scoreP1>scoreP2)
    {
    win_text[0]='\0';
    strcpy(win_text,player1.name);
    strcat(win_text," has won the match");
    iText(490, 320, "HURRAH", GLUT_BITMAP_HELVETICA_18);
    iText(420, 290,win_text, GLUT_BITMAP_HELVETICA_18);
    }
else if(scoreP1<scoreP2)
    {
    win_text[0]='\0';
    strcpy(win_text,player2.name);
    strcat(win_text," has won the match");
    iText(490, 320, "HURRAH", GLUT_BITMAP_HELVETICA_18);
    iText(420, 290,win_text, GLUT_BITMAP_HELVETICA_18);
    }
else 
    { iText(425, 320, "YOU TWO PLAYED WELL", GLUT_BITMAP_HELVETICA_18);
    iText(460, 290, "The match is draw", GLUT_BITMAP_HELVETICA_18);
    }
iText(430,260,"Press 'b' to go back to menu",GLUT_BITMAP_HELVETICA_18);
// update leaderboard
isGoldenGoal=0;
}
if(time_count<=90.00){iText(660,580,time,GLUT_BITMAP_HELVETICA_18);}

if(time_count>90/*||(isGoldenGoal==1&&(scoreP1>=2||scoreP2>=2))*/){
    current_game=0;


    iPauseTimer(0);
    char win_text [50];
    iSetColor(0, 0, 0);  // black background/
    iFilledRectangle(100, 30, 800, 540);
    iSetColor(255, 255, 255);
if(scoreP1>scoreP2)
    {
    win_text[0]='\0';
    strcpy(win_text,player1.name);
    strcat(win_text," has won the match");
    iText(490, 320, "HURRAH", GLUT_BITMAP_HELVETICA_18);
    iText(420, 290,win_text, GLUT_BITMAP_HELVETICA_18);
    }
else if(scoreP1<scoreP2)
    {
    win_text[0]='\0';
    strcpy(win_text,player2.name);
    strcat(win_text," has won the match");
    iText(490, 320, "HURRAH", GLUT_BITMAP_HELVETICA_18);
    iText(420, 290,win_text, GLUT_BITMAP_HELVETICA_18);
    }
else 
    { iText(425, 320, "YOU TWO PLAYED WELL", GLUT_BITMAP_HELVETICA_18);
    iText(460, 290, "The match is draw", GLUT_BITMAP_HELVETICA_18);
    }
iText(430,260,"Press 'b' to go back to menu",GLUT_BITMAP_HELVETICA_18);
// update leaderboard

    int goal_diff = fabs(scoreP1 - scoreP2);
if(leaderboard_update==0)update_leaderboard(goal_diff,player1.name,player2.name);
leaderboard_update=1;
sort_leaderboard();
}






if (goal) {
  iShowLoadedImage(0, 0, &goalim);  
  goalanicount--;
  

  if (goalanicount <=0) { 
    goal = 0;   isShowingGoalAnimation = 0;
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
  if(currentState==LEADREBOARD && key=='b') {currentState= MENU;
    iResumeTimer(0);}
if( currentState==GAME&&taking_input&&!current_game)
{  
   if(taking_input==1)
   {
    if(key == '\r'){ input_name[input_index] ='\0';
    input_index=0;
    taking_input=2;
    strcpy(player1.name,input_name);
     input_name[input_index]='\0';
     /* now we have to find or add players using these input names.*/
     }
    else if(key =='\b'){
        if(input_index>0) input_index--;
        input_name[input_index] ='\0';
    }
    else {
        input_name[input_index++] = key ;
        input_name[input_index] ='\0';
    }
   }
    else if(taking_input==2)
   {
    if(key == '\r'){ input_name[input_index] ='\0';
    input_index=0;
    taking_input=0;
    time_count=0;
    strcpy(player2.name,input_name);
    input_name[input_index]='\0';
    key = 'a';//the key is at '\r' so enter will be treated as start game if key is '\r'
    currentState= GAME;
    current_game=1;
  
    /* now we have to find or add players using these input names.*/
    }
    else if(key =='\b'){
        if(input_index>0) input_index--;
        input_name[input_index] ='\0';
    }
    else {
        input_name[input_index++] = key ;
        input_name[input_index] ='\0';
    }
   }
  

}
if(currentState == MENU ) {
        if(key == '\r') { // ENTER pressed
            iPlaySound("assets/sounds/press.wav", false,100);
            if(menuSelection == 0){ 
                current_game=0;
                taking_input=1;
                time_count=0;
                scoreP1 = 0;
                scoreP2 = 0;
                goal=0;
                goalanicount=0;
                isShowingGoalAnimation = 0;
                ball_x1=10,ball_y1=10,radius=15,dx=5,head_x=450,head_x2=550,head_y=100,head_y2=100,head_radius=25;
                ball_x =500,ball_y=200,ball_touched_ceil=0,ball_shoot=0,kick_off=0,p2_jump=0,up=10,p1_jump=0;
                leg_top=30 ,leg_top2=30, leg_bottom=0, leg_bottom2=0,dy=3,ball_touched_ground=0,gg_end=0;
               currentState = GAME;
               leaderboard_update=0;


        
               iResumeTimer(0);
            }
            if(menuSelection == 1) {currentState = GAME;
               iResumeTimer(0);
            }
            if(menuSelection == 2)
{currentState = PRACTICE;
  iResumeTimer(0);  
head_x2 = 800;
head_y2 = 100;
leg_top2 = 30;
leg_bottom2 = 0;
ball_x = 770;
ball_y = 150;
dx = -5; // ball moves left
kick_off = 0;
ball_shoot = 0;
ball_touched_ceil = 0;
gg_end =0;
    scoreP2 = 0;}



             if(menuSelection == 3) {
             isGoldenGoal=1;
               current_game=0;
                taking_input=1;
                time_count=0;
                scoreP1 = 0;
                scoreP2 = 0;
                goal=0;
                goalanicount=0;
                isShowingGoalAnimation = 0;
                ball_x1=10,ball_y1=10,radius=15,dx=5,head_x=450,head_x2=550,head_y=100,head_y2=100,head_radius=25;
                ball_x =500,ball_y=200,ball_touched_ceil=0,ball_shoot=0,kick_off=0,p2_jump=0,up=10,p1_jump=0;
                leg_top=30 ,leg_top2=30, leg_bottom=0, leg_bottom2=0,dy=3,ball_touched_ground=0;
              currentState = GAME;
               leaderboard_update=0,gg_end=0;


        
               iResumeTimer(0);
             }

              if(menuSelection == 4) currentState = LEADREBOARD;
               if(menuSelection == 5) currentState = HELP;
            
            if(menuSelection==6) exit(0);
        }
    }
    
    else if(currentState == HELP) {
        if(key == 'b') {iPlaySound("assets/sounds/press.wav", false,100);
            iPauseTimer(0); currentState = MENU;}
    }
else if(currentState == GAME) {
      
    switch (key)
    {
        case 'b':
    {if(!taking_input){iPauseTimer(0);  // pause ball movement timer
     currentState = MENU;}
    break;}
      

    case 'p':{
        // do something with 'p'
        iPauseTimer(0);
        break;}
   case 'r':{
        iResumeTimer(0);
        break;
    case '\r':{ iResumeTimer(0);
    break;}
       
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
        if(head_x>=110)
        head_x-=30;
        else head_x = 110;
        break;
    }
    case 'd':{
        if(head_x <= SCREEN_WIDTH - 120)
        head_x+=30;
        else head_x = SCREEN_WIDTH - 120;
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
   
        }}

else if(currentState == PRACTICE) {
    switch (key) {
        case 'b': // back to menu
            iPauseTimer(0);
            currentState = MENU;
            break;

        case 'r': // resume
            iResumeTimer(0);
            break;
            case 'p':
             iPauseTimer(0);
             break;

        case 'k': // shoot
            if((((ball_x > head_x2)? (ball_x - head_x2) : (head_x2 - ball_x) )<=40) &&
               ball_y <= head_y2 + head_radius && ball_y >= leg_bottom2)
            {
                kick_off = 1;
                ball_shoot = 1;
                ball_touched_ceil = 0;
                dx = (dx < 0) ? dx : -dx;
            }
            break;
    }
}
}


void iSpecialKeyboard(unsigned char key)
{
     if(currentState == MENU) {
        if(key == GLUT_KEY_DOWN) {
            iPlaySound("assets/sounds/press.wav", false,100);
            menuSelection = (menuSelection + 1) % 7;
        }
        if(key == GLUT_KEY_UP) {iPlaySound("assets/sounds/press.wav", false,100);
            menuSelection = ((menuSelection - 1 )>=0? menuSelection-1 : menuSelection-1+7) % 7;
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
      if(head_x2>=110)
        head_x2-=30;
        else head_x2 = 110;
        break;}
    case GLUT_KEY_RIGHT:{
        if(head_x2 <= SCREEN_WIDTH - 120)
        head_x2+=30;
        else head_x2 = SCREEN_WIDTH - 120;
        break;
    }
    default:
        break;
    }

}
if(currentState == PRACTICE) {
    switch (key) {
        case GLUT_KEY_UP:
            p2_jump = 1;
            break;
        case GLUT_KEY_LEFT:
            if(head_x2 >= 30) head_x2 -= 30;
            break;
        case GLUT_KEY_RIGHT:
            if(head_x2 <= SCREEN_WIDTH - 30) head_x2 += 30;
            break;
    }
}


       
    }
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    loadResources();
     iInitializeSound();
    //iSetTimer(20, goalAnimationUpdate);  // Timer 1 for animation
   //  iPauseTimer(1);
     if(timer_start==0 ){
                iSetTimer(10,ball_move);
               timer_start =1;}
    if(timer2==0) {iSetTimer(10,jump);
                   timer2 =1;}

    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Soccer");
    return 0;
}
