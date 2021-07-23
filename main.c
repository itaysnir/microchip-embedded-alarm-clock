#include "config.h"
#include "lcd.h"
#include "led.h"
//#include "timed.h"
#include "utils.h"
#include "rgbled.h"
#include "btn.h"
#include <xc.h>  
#include <sys/attribs.h>
#include <string.h>
#include <stdlib.h>


     
//==================================================
//              Global Configuration
//==================================================
// Device Config Bits in  DEVCFG1:	
#pragma config FNOSC =	FRCPLL
#pragma config FSOSCEN =	OFF
#pragma config POSCMOD =	EC
#pragma config OSCIOFNC =	ON
#pragma config FPBDIV =     DIV_1
#pragma config FWDTEN = OFF    // disable Watchdog

// Device Config Bits in  DEVCFG2:	
#pragma config FPLLIDIV =	DIV_2
#pragma config FPLLMUL =	MUL_20
#pragma config FPLLODIV =	DIV_1
#pragma config JTAGEN = OFF   
 #define _XTAL_FREQ 8000000




void Timer4Setup()
{
    static int fTimerInitialised = 0;
    if(!fTimerInitialised)
    {        
                                          //  setup peripheral
        PR4 = 10000;                        //             set period register, generates one interrupt every 1 ms
        TMR4 = 0;                           //             initialize count to 0
        T4CONbits.TCKPS = 3;                //            1:256 prescale value
        T4CONbits.TGATE = 0;                //             not gated input (the default)
        T4CONbits.TCS = 0;                  //             PCBLK input (the default)
        T4CONbits.ON = 1;                   //             turn on Timer1
        IPC4bits.T4IP = 2;                  //             priority
        IPC4bits.T4IS = 0;                  //             subpriority
        IFS0bits.T4IF = 0;                  //             clear interrupt flag
        IEC0bits.T4IE = 1;                  //             enable interrupt
        fTimerInitialised = 1;
    }
}

//==============================================================================
//Interrupt Handler- handled every 1msec
//==============================================================================
long currentTime=0;
void __ISR(_TIMER_4_VECTOR, ipl2auto) Timer4SR(void)
{
    currentTime++;
    IFS0bits.T4IF = 0;                  // clear interrupt flag
}
//==============================================================================
//this function initializes the needed components
//==============================================================================
void init(){
    LCD_Init(); 
    LED_Init();
    BTN_Init();
    SWT_Init();
    RGBLED_Timer5Setup();
    Timer4Setup();
}

int offset_time_stamp1 = 0;
int offset_time_stamp2 = 0;
int audio_time_stamp = 0;
int offset_time = 0;
int saved_sec2 = 0;
int saved_sec1 = 0;
int saved_min2 = 0;
int saved_min1 = 0;
int saved_hour2 = 0;
int saved_hour1 = 0;
int ring_sec2 = 0;
int ring_sec1 = 0;
int ring_min2 = 0;
int ring_min1 = 0;
int ring_hour2 = 0;
int ring_hour1 = 0;
int ring_time = 0;
int led_num = 0;
int operation = 0;

char c[8];


char* print_time(){

    int i=0;
    for(i=0;i<8;i++)
        c[i]='0';
    c[2] = ':';
    c[5] = ':';
    
    if (currentTime - offset_time > 1000){ //every 1sec increase second by 1
         saved_sec2 ++;
         
         if (saved_sec2 > 9){
             saved_sec1 ++;
             saved_sec2 = (saved_sec2 % 10 );
         }
         
         if (saved_sec1 > 5){
             saved_min2 ++;
             saved_sec1 = (saved_sec1 % 6 );
             }
         
         if (saved_min2 > 9){
             saved_min1 ++;
             saved_min2 = (saved_min2 % 10);
         }
         
         if (saved_min1 > 5){
             saved_hour2 ++;
             saved_min1 = (saved_min1 % 6 );
             }
         
         if (saved_hour2 > 9){
             saved_hour1 ++;
             saved_hour2 = (saved_min2 % 10);
         } 
         
         if ((saved_hour2 > 3) && (saved_hour1 == 2)){
             saved_hour1 = 0;
             saved_hour2 = 0;
         }
         
         c[0] = saved_hour1 + '0';
         c[1] = saved_hour2 + '0';
         c[3] = saved_min1 + '0';
         c[4] = saved_min2 + '0';
         c[6] = saved_sec1 + '0';
         c[7] = saved_sec2 +'0';
         
         
         offset_time = currentTime; 
         }
    
    c[0] = saved_hour1 + '0';
    c[1] = saved_hour2 + '0';
    c[3] = saved_min1 + '0';
    c[4] = saved_min2 + '0';
    c[6] = saved_sec1 + '0';
    c[7] = saved_sec2 +'0';
    return c;
}


void increase_time1 (int BTNC1, int BTNC2, int BTNC3){     
    
    if (BTNC1 == 1){
        saved_hour2 ++;
        if (saved_hour1==2 && saved_hour2>3){
            saved_hour1 = 0;
            saved_hour2 = 0;
        }
        if (saved_hour2>9){
            saved_hour2 = 0;
            saved_hour1 ++;
        }               
    }
    
    if (BTNC2 == 1){
        saved_min2 ++;
        if (saved_min1==5 && saved_min2>9){
            saved_min1 = 0;
            saved_min2 = 0;
        }
        if (saved_min2>9){
            saved_min2 = 0;
            saved_min1 ++;
        }
    }
    
    if (BTNC3 == 1){
        saved_sec2 ++;
        if (saved_sec1==5 && saved_sec2>9){
            saved_sec1 = 0;
            saved_sec2 = 0;
        }
        if (saved_sec2>9){
            saved_sec2 = 0;
            saved_sec1 ++;
        }
    }
    
}

void decrease_time1 (int BTNC1, int BTNC2, int BTNC3){     
    
    if (BTNC1 == 1){
        saved_hour2 --;
        if (saved_hour1==0 && saved_hour2<0){
            saved_hour1 = 2;
            saved_hour2 = 3;
        }
        else if (saved_hour2<0){
            saved_hour2 = 9;
            saved_hour1 --;
        }               
    }
    
    if (BTNC2 == 1){
        saved_min2 --;
        if (saved_min1==0 && saved_min2<0){
            saved_min1 = 5;
            saved_min2 = 9;
        }
        else if (saved_min2<0){
            saved_min2 = 9;
            saved_min1 --;
        }
    }
    
    if (BTNC3 == 1){
        saved_sec2 --;
        if (saved_sec1==0 && saved_sec2<0){
            saved_sec1 = 5;
            saved_sec2 = 9;
        }
        else if (saved_sec2<0){
            saved_sec2 = 9;
            saved_sec1 --;
        }
    }
    
}

void increase_time2 (int BTNC1, int BTNC2, int BTNC3){     
    
    if (BTNC1 == 1){
        ring_hour2 ++;
        if (ring_hour1==2 && ring_hour2>3){
            ring_hour1 = 0;
            ring_hour2 = 0;
        }
        if (ring_hour2>9){
            ring_hour2 = 0;
            ring_hour1 ++;
        }               
    }
    
    if (BTNC2 == 1){
        ring_min2 ++;
        if (ring_min1==5 && ring_min2>9){
            ring_min1 = 0;
            ring_min2 = 0;
        }
        if (ring_min2>9){
            ring_min2 = 0;
            ring_min1 ++;
        }
    }
    
    if (BTNC3 == 1){
        ring_sec2 ++;
        if (ring_sec1==5 && ring_sec2>9){
            ring_sec1 = 0;
            ring_sec2 = 0;
        }
        if (ring_sec2>9){
            ring_sec2 = 0;
            ring_sec1 ++;
        }
    }
    
}

void decrease_time2 (int BTNC1, int BTNC2, int BTNC3){     
    
    if (BTNC1 == 1){
        ring_hour2 --;
        if (ring_hour1==0 && ring_hour2<0){
            ring_hour1 = 2;
            ring_hour2 = 3;
        }
        else if (ring_hour2<0){
            ring_hour2 = 9;
            ring_hour1 --;
        }               
    }
    
    if (BTNC2 == 1){
        ring_min2 --;
        if (ring_min1==0 && ring_min2<0){
            ring_min1 = 5;
            ring_min2 = 9;
        }
        else if (ring_min2<0){
            ring_min2 = 9;
            ring_min1 --;
        }
    }
    
    if (BTNC3 == 1){
        ring_sec2 --;
        if (ring_sec1==0 && ring_sec2<0){
            ring_sec1 = 5;
            ring_sec2 = 9;
        }
        else if (ring_sec2<0){
            ring_sec2 = 9;
            ring_sec1 --;
        }
    }
    
}

int its_a_match(int ss2, int ss1, int sm2, int sm1, int sh2, int sh1, int rs2, int rs1, int rm2, int rm1, int rh2, int rh1){
    int result = 0;
    if ((ss2 == rs2)&&(ss1==rs1)&&(sm2==rm2)&&(sm1==rm1)&&(sh2==rh2)&&(sh1==rh1))
        result = 1;
    return result;
}

void LEDS_PARTY(){
    
    LED_ToggleValue(led_num);
    //increasing leds
    if (operation == 0){ 
        led_num += 1;
        if (led_num == 7)
            operation = 1;
    }
    //decreasing leds
    else{  
        led_num -= 1;
        if (led_num == 0)
            operation = 0;
    }   
    LED_ToggleValue(led_num);
    
}

void start_ring(){
    
if (currentTime - audio_time_stamp > 1000){ //every 1sec increase ring_time by 1
    ring_time ++;         
    if (SWT_GetValue(6)==1){
        LEDS_PARTY();
    }
    
    
    audio_time_stamp = currentTime; 
}


}


void main(){
    init(); //initialize all needed components
    
    char time_array[16];   
    char ring_time_array[16];
    int mode = 0;   
    int first_BTNC = 0;
    int second_BTNC = 0;
    int third_BTNC = 0;    
    int audio_flag = 0;     
    strcpy (time_array,"00:00:00");
    strcpy (ring_time_array,"00:00:00");
    time_array[15]='\0';
    ring_time_array[15]='\0';
    LCD_WriteStringAtPos(time_array, 0, 0);
    
    
    while (1){
    
    
    if (audio_flag == 1){
        start_ring();
        if ((ring_time > 59)||(SWT_GetValue(7)==0)){
            audio_flag = 0;
            mode = 0;
            ring_time = 0;
            LED_SetValue(0,0);
            LED_SetValue(1,0);
            LED_SetValue(2,0);
            LED_SetValue(3,0);
            LED_SetValue(4,0);
            LED_SetValue(5,0);
            LED_SetValue(6,0);
            LED_SetValue(7,0);
        }
        /*insert update_song() here*/
        LCD_WriteStringAtPos("KACHIGA!       ", 1, 0);
         
            
        }
    if (SWT_GetValue(7)==1){
        if (its_a_match(saved_sec2,saved_sec1,saved_min2,saved_min1,saved_hour2,saved_hour1,ring_sec2,ring_sec1,ring_min2,ring_min1,ring_hour2,ring_hour1)){  
            mode = 4;
            audio_flag = 1;
            LED_ToggleValue(0);
            
        }       
    }  
        
    if (SWT_GetValue(0)==0 && SWT_GetValue(1)==0){
        mode = 1; 
        first_BTNC = 0;
        second_BTNC = 0;
        third_BTNC = 0;
        
        strcpy(time_array, print_time());
        LCD_WriteStringAtPos (time_array, 0, 0);
        LCD_WriteStringAtPos("            ", 0, 8);
        if (audio_flag == 0){
            LCD_WriteStringAtPos("Clock Mode       ", 1, 0);
        }
        
    }
    if (SWT_GetValue(0)==1 && SWT_GetValue(1)==0){
        mode = 2;               
        time_array[0] = saved_hour1 + '0'; //restore values
        time_array[1] = saved_hour2 + '0';
        time_array[3] = saved_min1 + '0';
        time_array[4] = saved_min2 + '0';
        time_array[6] = saved_sec1 + '0';
        time_array[7] = saved_sec2 +'0';
        
        if(BTN_GetValue(2)){ while(BTN_GetValue(2)){}  //determine which BTNC mode we're at
        
        if (first_BTNC == 0 && second_BTNC == 0 && third_BTNC == 0){
            first_BTNC = 1;
            
        }
        else if (first_BTNC == 1 && second_BTNC == 0 && third_BTNC == 0){
            second_BTNC = 1;
            first_BTNC = 0;
            
        } 
        else if (first_BTNC == 0 && second_BTNC == 1 && third_BTNC == 0){
            second_BTNC = 0;
            third_BTNC = 1;
           
        }       
        else if (first_BTNC == 0 && second_BTNC == 0 && third_BTNC == 1){
            third_BTNC = 0;
            
        }
        
        }
           
        
        if (first_BTNC == 1 && currentTime-offset_time_stamp1>500){
            time_array[0] = ' ';
            time_array[1] = ' ';
        }
        
        if (second_BTNC == 1 && currentTime-offset_time_stamp1>500){
            time_array[3] = ' ';
            time_array[4] = ' ';
        }
        
        if (third_BTNC == 1 && currentTime-offset_time_stamp1>500){
            time_array[6] = ' ';
            time_array[7] = ' ';
        }
        
        if(currentTime-offset_time_stamp1 > 1000)
            {
                 offset_time_stamp1=currentTime;
            }  
        
        LCD_WriteStringAtPos("SET ", 0, 0);
        LCD_WriteStringAtPos(time_array, 0, 4);
        if (audio_flag == 0){
            LCD_WriteStringAtPos("Set Time Mode     ", 1, 0);
        }
        
               
        
        if(BTN_GetValue(3))
            increase_time1 (first_BTNC,second_BTNC,third_BTNC);                                                  
        
        if (BTN_GetValue(1))
            decrease_time1 (first_BTNC,second_BTNC,third_BTNC); 
                                       
    }
    
    if (SWT_GetValue(0)==0 && SWT_GetValue(1)==1){
        mode = 3;              
        strcpy(time_array, print_time()); // we wish the clock won't stop 
        ring_time_array[0] = ring_hour1 + '0';
        ring_time_array[1] = ring_hour2 + '0';
        ring_time_array[3] = ring_min1 + '0';
        ring_time_array[4] = ring_min2 + '0';
        ring_time_array[6] = ring_sec1 + '0';
        ring_time_array[7] = ring_sec2 + '0';
        
        if(BTN_GetValue(2)){ while(BTN_GetValue(2)){}  //determine which BTNC mode we're at
        
        if (first_BTNC == 0 && second_BTNC == 0 && third_BTNC == 0){
            first_BTNC = 1;
            
        }
        else if (first_BTNC == 1 && second_BTNC == 0 && third_BTNC == 0){
            second_BTNC = 1;
            first_BTNC = 0;
            
        } 
        else if (first_BTNC == 0 && second_BTNC == 1 && third_BTNC == 0){
            second_BTNC = 0;
            third_BTNC = 1;
           
        }       
        else if (first_BTNC == 0 && second_BTNC == 0 && third_BTNC == 1){
            third_BTNC = 0;
            
        }
        
        }
           
        
        if (first_BTNC == 1 && currentTime-offset_time_stamp2>500){
            ring_time_array[0] = ' ';
            ring_time_array[1] = ' ';
        }
        
        if (second_BTNC == 1 && currentTime-offset_time_stamp2>500){
            ring_time_array[3] = ' ';
            ring_time_array[4] = ' ';
        }
        
        if (third_BTNC == 1 && currentTime-offset_time_stamp2>500){
            ring_time_array[6] = ' ';
            ring_time_array[7] = ' ';
        }
        
        if(currentTime-offset_time_stamp2 > 1000)
            {
                 offset_time_stamp2=currentTime;
            }  
         
        LCD_WriteStringAtPos("ALARM ", 0, 0);
        LCD_WriteStringAtPos(ring_time_array, 0, 6);
        if (audio_flag == 0){
            LCD_WriteStringAtPos("Set Ring Time      ", 1, 0);
        }
        
        if(BTN_GetValue(3))
            increase_time2 (first_BTNC,second_BTNC,third_BTNC);                                                  
        
        if (BTN_GetValue(1))
            decrease_time2 (first_BTNC,second_BTNC,third_BTNC); 
    }
    
    
    }
    
    
}

  






