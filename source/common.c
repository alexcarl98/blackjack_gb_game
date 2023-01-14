#include <gb/gb.h>
#include <string.h>
#include <stdlib.h>

void performantdelay(uint8_t numloops){
	uint8_t i;
    // less CPU intensive than the delay method.
    for(i = 0; i < numloops; i++){
        wait_vbl_done();    //gbdk method that waits until one screen has been drawn. 
    }
}

void fadeout(){
	uint8_t k;
	for(k=0;k<4;k++){
		switch(k){
			case 0:
				BGP_REG = 0xE4;
				break;
			case 1:
				BGP_REG = 0xF9;
				break;
			case 2:
				BGP_REG = 0xFE;
				break;
			case 3:
				BGP_REG = 0xFF;	
				break;						
		}
		performantdelay(3);
	}
}

void fadein(){
	uint8_t k;
	for(k=0;k<3;k++){
		switch(k){
			case 0:
				BGP_REG = 0xFE;
				break;
			case 1:
				BGP_REG = 0xF9;
				break;
			case 2:
				BGP_REG = 0xE4;
				break;					
		}
		performantdelay(5);
	}
}

void DrawNumberBkg(uint8_t x,uint8_t y, uint16_t number,uint8_t digits){
    //tried to use BCD but i found this function online and used it instead
    unsigned char buffer[]="00000000";
    // Convert the number to a decimal string (stored in the buffer char array)
    uitoa(number, buffer, 10);

    // Get the length of the number so we can add leading zeroes
    uint8_t len =strlen(buffer);

    // Add some leading zeroes
    // uitoa will not do this for us
    for(uint8_t i=0;i<digits-len;i++){
        VBK_REG=0;
        set_bkg_tile_xy(x++,y,0);
    }

    // Draw our number
    for(uint8_t i=0;i<len;i++){
        VBK_REG=0;
        set_bkg_tile_xy(x++,y,(buffer[i]-'0'));
    }
}

void DrawNumber(uint8_t x,uint8_t y, uint16_t number,uint8_t digits){
    //tried to use BCD but i found this function online and used it instead
    unsigned char buffer[]="00000000";
    // Convert the number to a decimal string (stored in the buffer char array)
    uitoa(number, buffer, 10);

    // Get the length of the number so we can add leading zeroes
    uint8_t len =strlen(buffer);

    // Add some leading zeroes
    // uitoa will not do this for us
    for(uint8_t i=0;i<digits-len;i++){
        VBK_REG=0;
        set_win_tile_xy(x++,y,0);                      // for the new background tiles, this 70 will be 0.
    }
    
    // Draw our number
    for(uint8_t i=0;i<len;i++){
        VBK_REG=0;
        set_win_tile_xy(x++,y,(buffer[i]-'0'));
    }
}