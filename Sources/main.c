#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */
#include "main_asm.h"   /* interface to the assembly module */
#include "lcd.h"
#include <stdio.h>
#include "time.h" 


void move(void);
void left(void);
void right(void);
void go(int);
void search(void);
void update_xy(void);
void update_dir(char);
void default_walls(void);
void make_turn(int,int,int);
void turn_around();
void update_yx();


char dir='N';
int x=0,y=0;


struct maze
{
	int n_wall,s_wall,e_wall,w_wall;
	int visited;

}m[6][6];

void main(void) 
{  
                        
  int i,c=0;
   initial();   
    
  DDRB = 0x00;            /* Port B Data Dir Reg        */   
  RDRT = 0x00;            /* Reduce drive enabled       */
  PUCR = PUCR|0x02;
 
   
  SoftXIRQ = StartAddr;   // Set XIRQ to start of assembly /
  
 
  intrNum = 0;
             
  EnableInterrupts;
  
  
  /*wait(150);
  wait(150);
  wait(150);
  wait(150);
  wait(150);
  search();
  stop_dead();
  
  
    */
   
   while (1) 
  {  
      while (intrNum == 0); // check for interrupt 
       
    wait(1);
      switch (intrNum) 
      {
       
        
        default:       // not implemented bits 
           
           
           //printf("%d ",wsns_r());
           
           search();
           stop_dead();         
           break;
      }               // end of switch 
      intrNum = 0;    // reset intr num to 0   
  }            // end of while 
  for(;;) {
    _FEED_COP(); // feeds the dog 
  } // loop forever 
  // please make sure that you never leave main 
}                                                                     



/***********************************************
PACN0 - used for right wheel
PACN3 - used for left wheel
To adjust the segment count, change value of i
***********************************************/
void move(void){
   
  int s1=0,s2=0,s3=0;
  unsigned int i=24;      
  char buf[25];
  
  reset_cnt();
  

 straighten();
 mtr_g();
 
  
  while(PACN3<i||PACN0<i){
       
       if(PACN3>30||PACN0>30)
        break;
       
      forward();
      s2=sns_l();
      s2=sns_l();
   
      s3=sns_r();
      s3=sns_r();
      
      s1=sns_f();
      s1=sns_f();
      
      if(s3==0){             //adjust from right side
   
      straighten();
      right_adjust();      
   }
   
   if(s2==0){        //adjust from left side 
   
      straighten();                        
      left_adjust(); 
   }
   
   if(s1==0){
   
      if(PACN0<10||PACN3<10)
      update_yx();
      
      break;
   }
   
   
      
     // lcd_clear(0x01);
     // sprintf(buf, "Left =%d \n Right=%d",PACN3,PACN0);
     //void)lcd_puts(buf); 
      wait(1);
 }    
  
}

 
/***********************************************************
Makes a left turn and updates the direction of mouse
***********************************************************/
void left(){
  
  unsigned int i=12;      
  
  //i=(~i+1);
  //PACN0=i;
  //PACN3=i;
   
  
  reset_cnt();
  turn_left();
  mtr_go();
  while(PACN3<i||PACN0<i){
        
      //lcd_clear(0x01);
      //printf("Left =%d \n Right=%d", (PACN3+i), (PACN0+i));
     // (void)lcd_puts(buf); 
     // wait(1);
     
  }           
   
  while(PACN0<i) {
  //printf("Right=%d", PACN3);
    right_adjust();
  }
        
  while(PACN3<i)  {
    //printf("Left =%d",PACN0);
    left_adjust();   
  }
  update_dir('L');
  
}

void turn_around(){
  
  unsigned int i=5; 
  char buf[20];     
  
  
   reset_cnt();
  
  straighten();
  mtr_g();
  while(PACN0<i||PACN0<i) {
    
    wait(1);
  }
  
  reset_cnt();
  i=30;
  
  
  turn_left();
  mtr_go();
  while(PACN3<i||PACN0<i){
        
      lcd_clear(0x01);
      printf("Left =%d \n Right=%d", (PACN3+i), (PACN0+i));
      (void)lcd_puts(buf); 
      wait(1);
     
  }           
   
  while(PACN0<i) {
  //printf("Right=%d", PACN3);
    right_adjust();
  }
        
  while(PACN3<i)  {
    //printf("Left =%d",PACN0);
    left_adjust();   
  }
  update_dir('L');
  update_dir('L');
  //printf(" %c ",dir);
}



/***********************************************************
Makes a right turn and updates the direction of mouse
***********************************************************/
void right(){
  
  unsigned int i=12;        
  
  
  
  reset_cnt();
  turn_right();
  mtr_go();                 
  
  while(PACN3<i||PACN0<i){
        
      //lcd_clear(0x01);
      //printf("Left =%d \n Right=%d", (PACN3+i), (PACN0+i));
     // (void)lcd_puts(buf); 
     // wait(1);
     
  }           
    
  while(PACN0<i) {
  //printf("Right=%d", PACN3);
    right_adjust();
  } 
    
  while(PACN3<i)  {
    //printf("Left =%d",PACN0);
    left_adjust();   
  }
  update_dir('R');
  
  
  
  
}

/**********************************************************
Update x and y based on direction
**********************************************************/

void update_xy(){
 
  switch(dir){
   
    case 'N' : x++;break;
    case 'S' : x--;break;
    case 'E' : y++;break;
    case 'W' : y--;break;
    default: break;
   
   
    
  }
}


/**********************************************************
Update x and y based on direction
**********************************************************/

void update_yx(){
 
  switch(dir){
   
    case 'N' : x--;break;
    case 'S' : x++;break;
    case 'E' : y--;break;
    case 'W' : y++;break;
    default: break;
   
   
    
  }
}

/*********************************************************
Update mouse direction based on turn
*********************************************************/
void update_dir(char turn){
 
  printf(" %c" , dir);
 
 if(turn=='R'){
 
    switch(dir){
      case 'N': dir='E';break;
      case 'S': dir='W';break;
      case 'E': dir='S';break;
      case 'W': dir='N';break;
      default:break; 
    }
 } 
 else{
   switch(dir){
      case 'N': dir='W';break;
      case 'S': dir='E';break;
      case 'E': dir='N';break;
      case 'W': dir='S';break;
      default:break; 
 }
  printf(" %c" , dir);
 
}
}


/******************************************************
Count # of squares searched
******************************************************/
int squares_done(){

  int count=0,i,j;
  
  for(i=0;i<6;i++)
    for(j=0;j<6;j++)
      count+=m[i][j].visited;
      
  return count;      
  
}

/*************************************************
Assign the walls according to direction of mouse
*************************************************/
void assign_walls(){

    switch(dir){

	case 'N': m[x][y].e_wall=wsns_r();
		  m[x][y].n_wall=sns_f();
		  m[x][y].w_wall=wsns_l();
		  default_walls();
		  make_turn(m[x][y].e_wall,m[x][y].n_wall,m[x][y].w_wall);
		  break;
	case 'S': m[x][y].e_wall=wsns_l();
		  m[x][y].w_wall=wsns_r();
		  m[x][y].s_wall=sns_f();
		  default_walls();
		  make_turn(m[x][y].w_wall,m[x][y].s_wall,m[x][y].e_wall);
		  break;
	case 'E': m[x][y].e_wall=sns_f();
		  m[x][y].n_wall=wsns_l();
		  m[x][y].s_wall=wsns_r();
		  default_walls();
		  make_turn(m[x][y].s_wall,m[x][y].e_wall,m[x][y].n_wall);
		  break;
	case 'W': m[x][y].w_wall=sns_f();
		  m[x][y].n_wall=wsns_r();
		  m[x][y].s_wall=wsns_l();
		  default_walls();
		  make_turn(m[x][y].n_wall,m[x][y].w_wall,m[x][y].s_wall);
		  break;

	default: break;


    }

}
/*************************************************************************
Check if a particular cell is visited or not
*************************************************************************/
int check_visited(char side){

    int v,i,j;  

    i=x;
    j=y;

    switch(side){

	case 'R'://printf("\n R"); 
		if(dir=='N')v=m[i][(j+1)].visited;
		  else if(dir=='S')v=m[i][(j-1)].visited;
		  else if(dir=='E')v=m[(i-1)][j].visited;
		  else v=m[(i+1)][j].visited;       
	  	break;	
			
	case 'L': 
		if(dir=='N')v=m[i][(j-1)].visited;
		      if(dir=='S')v=m[i][(j+1)].visited;
		  if(dir=='E')v=m[(i+1)][j].visited;
		  if(dir=='W')v=m[(i-1)][j].visited;  
			break;

	case 'F':if(dir=='E')v=m[i][(j+1)].visited;
		     if(dir=='W')v=m[i][(j-1)].visited;
		     if(dir=='S')v=m[(i-1)][j].visited;
		     if(dir=='N')v=m[(i+1)][j].visited;  
		   break;
	default: break;



    }
    return v; 

}

/******************************************************
  Assigns the boundries of the maze
 ******************************************************/
void default_walls(){

    int i,j;

    //Assign the boundries of the maze
    for(i=0;i<6;i++)m[i][0].w_wall=0;
    for(i=0;i<6;i++)m[i][5].e_wall=0;
    for(j=0;j<6;j++)m[0][j].s_wall=0;
    for(j=0;j<6;j++)m[5][j].n_wall=0;

}

/*********************************************************************
Make a turn right or left based on conditions
*********************************************************************/
void make_turn(int r,int f,int l){

    char turn_made='n';
                    


    if(r!=0){
	if(check_visited('R')==0) {
	  //  printf(" Current direction = %c ", dir);
	    right();
//	    printf(" Right ");

	    turn_made='y';  //Mark turn_made so no further turn is made     
	}
    }

    //Go front if no wall in front and no wall on right or right side cell was not visited
    if(f!=0 && turn_made=='n'){
	if(check_visited('F')==0) {
	   // printf(" front ");
	    turn_made='y';  //Mark turn_made so no further turn is made in this loop
	}
    }

    //Make a left turn if there is no wall on left + cell on left was not visited before + no turns were made before
    if(l!=0 && turn_made=='n'){
	if(check_visited('L')==0) {
//	    printf(" Left ");
	    left();
	    turn_made='y';  //Mark turn_made so no further turn is made     
	}
    }

    //Turn right if no wall on right and all adjacent cells were visited
    if(r!=0&&turn_made=='n'){
	//printf("sdf");
	right();
	turn_made='y';
    }

    //Go front if no wall in front and all adjacent cells were visited
    if(f!=0&&turn_made=='n'){
	turn_made='y'; 
    }

    //Go left if no wall on left and all adjacent cells were visited
    if(l!=0&&turn_made=='n'){
	left();
	turn_made='y';  
    }
    
    //Turn around if it's a dead end(i.e walls on all 3 sides) and no turns were made      
    if(turn_made=='n'){
	turn_around();
	turn_made=='y';
    }


}






/*************************************************************************
Searches the maze
Basic Algorithm : Go right first if possible
              if not right go front if possible
                if not front then go left if possible
                  else turn around
*************************************************************************/
void search(){

  int i,j;
  int r,f,l;
  char turn_made='n',buf[20];
  dir='N';
  x=0;y=0;
  
  
  reset_cnt();
  straighten();
  mtr_g();
  while(PACN0<3||PACN0<3) {
    
    wait(1);
  }
    
  default_walls();
  
  //No cell is visited intially
  for(i=0;i<6;i++)
    for(j=0;j<6;j++)
        m[i][j].visited=0;
  
 assign_walls();   
 while(squares_done()<34)     //Go on until 33 squares are done
 {
  
  lcd_clear(0x01);
  sprintf(buf, "Cell= %d , %d", x, y);
  (void)lcd_puts(buf); 
  wait(1);
  
  printf("1st Cell = %d , %d ",x,y);
  
  move();
  m[x][y].visited=1;        //Mark last visited cell.  
  update_xy();
   
  assign_walls();
  
  
 }
  stop_dead();  
  
}






