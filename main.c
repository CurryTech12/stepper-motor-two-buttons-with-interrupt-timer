//main.c
#include <SI_EFM8BB1_Register_Enums.h>
unsigned char c = 0;
void InitDevice(void)
{
	
//Disable Watchdog Timer with key sequence
WDTCN = 0xDE; //First key
WDTCN = 0xAD; //Second key

//Setup Defaults for P1
P1MDOUT = P1MDOUT_B0__OPEN_DRAIN | P1MDOUT_B1__OPEN_DRAIN
| P1MDOUT_B2__OPEN_DRAIN | P1MDOUT_B3__OPEN_DRAIN
| P1MDOUT_B4__PUSH_PULL | P1MDOUT_B5__OPEN_DRAIN
| P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B7__OPEN_DRAIN;
//Enable Weak Pullups
XBR2 = XBR2_WEAKPUD__PULL_UPS_ENABLED | XBR2_XBARE__ENABLED;
//Disable Interrupts
IE = IE_EA__ENABLED | IE_EX0__DISABLED | IE_EX1__DISABLED
| IE_ESPI0__DISABLED | IE_ET0__ENABLED | IE_ET1__DISABLED
| IE_ET2__ENABLED | IE_ES0__DISABLED;
}

void delayMs(unsigned char x)// calls delay and whatever number is called
{
	//each increment of c = 0.78125ms
	//need 
	c=0;// restarts c to  be zero
	while(c<x);//loop 10 times // when inter triggered from timer goes to isr reloads values then increments c however many times we want
}


void timer0_FlowISR(void) interrupt 1{// resets overflow flag when it gets set
	c++; // increment the global variable 
	TCON &= ~(1 << 4);// stop the timer 
	TH0 = 0xFF;	// for 0.78125ms delay reload the values 
	TL0 = 0x38;// for 0.78125ms delay
	TCON |= (1 << 4);// tells the timer to run by setting TR0 to on
}
void main(void){
	unsigned char x,y;
	InitDevice();
	IE = 0x82;// enables interupt
	TMOD = 0x01;//sets timer to 16b mode
	TH0 = 0xFF;	// for 0.78125ms delay
	TL0 = 0x38;// for 0.78125ms delay
	TCON |= (1 << 4);// tells the timer to run by setting TR0 to on define the timer inside main function
	
	while(1){
		if(!(P0 & 0x01)){ // when button on right is pushed zero makes if statement true run clockwise 90degree
			for(x=0;x<128;x++){//counterclockwise because each step is 0.17578 degrees
				P1 = 0x09; // 1100 blue yellow coils P1.3,P1.2
				delayMs(10);// times by 10 to get 7.8125ms per full step
				P1 = 0x03; // 0110 yellow orange P1.2,P1.1
				delayMs(10);
				P1 = 0x06; // 0011 orange pink coils on P1.1, P1.0
				delayMs(10);
				P1 = 0x0C; // 1001 pink blue coils on P1.0, P1.3
				delayMs(10);// 7.8125ms times four equals to 31.25ms delay for four steps 31.25ms times 128 iterations = 4000ms or 4 sec
			}
		}
		if(!(P0 & 0x02)){//if left button is pushed zero is true turn counter clockwise 45 degree
			for(y =0;y<64;y++){
				P1 = 0x0C; // 1100 blue yellow coils P1.3,P1.2 
				delayMs(40);// 0.78125ms times 40 = 31.25ms per step this delay times four equals to 125ms for four steps iterat 64 times = 8000ms
				P1 = 0x06; // 0110 yellow orange P1.2,P1.1
				delayMs(40);
				P1 = 0x03; // 0011 orange pink coils on P1.1, P1.0
				delayMs(40);
				P1 = 0x09; // 1001 pink blue coils on P1.0, P1.3
				delayMs(40);
			}
	}
}
}	
