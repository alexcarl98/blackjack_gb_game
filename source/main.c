#include <gb/gb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <rand.h>

#include "..\res\bkg_single_cards.h"    // bkg_1_cards
#include "..\res\bkg_sc_c1.h"           // bkg_score_w_card1
#include "..\res\bkg_hs_opt.h"          // bkg_hs_o
#include "..\res\bkg_win.h"             // bkg_winner
#include "..\res\bkg_pushgm.h"          // bkg_push
#include "..\res\bkg_lose.h"            // bkg_loser
#include "..\res\bkg_sur.h"             // bkg_surrender
#include "..\res\bkg_table.h"           // bkg_tab
#include "..\res\bkg_insurance.h"       // bkg_ins
#include "..\res\bkg_game_info.h"       // bkg_gm_info
#include "..\res\bkg_win_cash_bet.h"    // bkg_cash_bet

#include "..\res\bkg_tiles_alt.h"
#include "..\res\card_sprites.h"
#include "card_structures.c"

uint8_t numberOfDecks = 4;
uint16_t numberOfGames = 0;
uint8_t i,j,k;
UBYTE player_split_cards[12];

void dealer_draws(deck *d, hand *dlr_h, uint8_t * sprite_count);
void play_game(deck * myDeck, player * human, player * bot, bool same_bet);
void DrawNumber(uint8_t x,uint8_t y, uint16_t number,uint8_t digits);
void performantdelay(uint8_t numloops);
void fadeout();
void fadein();
void init_gfx();
// void DealCards(deck * myDeck, player * human, player * bot, uint8_t * sprite_count);
// void PlayerMoves();
// void ResetGame();
// void DisplayDealerUpcard();
UBYTE outcome(UBYTE p_score, UBYTE p_flags, UBYTE d_score, UBYTE d_flags);
void settle_score(player *dlr, player *p);
void reveal_hole_card(player *dlr, player *p);
void DrawNumberBkg(uint8_t x,uint8_t y, uint16_t number,uint8_t digits);
void insurance(player *p);

void display_game_info(uint16_t number){
    init_bkg(37);
    set_bkg_tiles(4,4,12,8,bkg_gm_info);
    uint16_t temp;
    temp = numberOfDecks - ((number/52)+1);
    DrawNumberBkg(14,7, temp, 1);
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
        set_bkg_tile_xy(x++,y,70);
    }
    
    // Draw our number
    for(uint8_t i=0;i<len;i++){
        VBK_REG=0;
        set_bkg_tile_xy(x++,y,(buffer[i]-'0')+70);
    }
}

void show_score(hand *self){
    // the hand's x & y pivot is passed in and the new x and y positions are calculated in the function
    uint8_t score_x = (self->h_x/8) - 3;
    uint8_t score_y = (self->h_y/8);

    if ((self->flags & 1) != 0){            //display 'BJ' if player has blackjack
        VBK_REG=0;
        set_bkg_tile_xy(score_x, score_y, 81);
        set_bkg_tile_xy(score_x+1, score_y, 89);
    }
    else{
        if((self->flags & 0b100) != 0){
            VBK_REG=0;
            set_bkg_tile_xy(score_x-1, score_y, 45);
        }else{
            VBK_REG=0;
            set_bkg_tile_xy(score_x-1, score_y, 37);
        }
        DrawNumberBkg(score_x, score_y, (uint16_t)self->score, 2);
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
        set_win_tile_xy(x++,y,70);
    }
    
    // Draw our number
    for(uint8_t i=0;i<len;i++){
        VBK_REG=0;
        set_win_tile_xy(x++,y,(buffer[i]-'0')+70);
    }
}

void performantdelay(uint8_t numloops){
    // less CPU intensive than the delay method.
    for(i = 0; i < numloops; i++){
        wait_vbl_done();    //gbdk method that waits until one screen has been drawn. 
    }
}

void fadeout(){
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

void dealer_draws(deck *d, hand *dlr_h, uint8_t * sprite_count){
    uint8_t hidden_card_x = 5;
    // uint8_t hidden_card_y = 1;
    if (dlr_h->score < 17){
        while ((dlr_h->score < 17) && dlr_h->size < 7){
            recieve_card(dlr_h, d, sprite_count, 1);
            set_bkg_tiles(hidden_card_x, 1,4,5,bkg_1_cards);
            hidden_card_x++;
        }
    }
    
}

UBYTE outcome(UBYTE p_score, UBYTE p_flags, UBYTE d_score, UBYTE d_flags){
    //win
    bool b_score = (((d_score < p_score) | (d_score > 21)) & (p_score <= 21));      // dealer busts or has a lower score, without the player busting
    bool b_hand = (((p_flags & 1) != 0) & ((d_flags & 1) == 0));                // player has a blackjack and the dealer doesn't
    if (b_score | b_hand){return 0x1;}
    //push
    bool same_sc = ((p_score == d_score) & (p_score <= 21));        //same score w/no one busting
    bool same_bj = ((p_flags & 1) == (d_flags & 1));            // same status of black jack
    if (same_sc & same_bj){return 0x3;}
    //loss
    else{return 0x2;}
}

void settle_score(player *dlr, player *p){
    UBYTE result = outcome(p->hd.score, p->hd.flags, dlr->hd.score, dlr->hd.flags);
    uint16_t temp;
    switch(result){
        case 0x1:
            //if player wins, display win 
            if ((p->hd.flags & 0b10000000) == 0){
                set_bkg_tiles(5, 5, 10, 3, bkg_winner);
            }else{
                set_bkg_tile_xy((p->hd.h_x/8) - 2, (p->hd.h_y/8)-2, 102);
            }
            p->cash += (p->bet);
            p->cash += (p->bet);
            if ((p->hd.flags & 0b1) != 0b0){
                temp = (p->bet)/2;
                if((p->bet % 2) != 0){
                    p->half_dollar = true;
                }
                p->cash += (temp);
                }
            break;
        case 0x2:
            if ((p->hd.flags & 0b10000000) == 0){
                set_bkg_tiles(5, 5, 10, 3, bkg_loser);
            }else{
                set_bkg_tile_xy((p->hd.h_x/8) - 2, (p->hd.h_y/8)-2, 91);
            }
            break;
        case 0x3:
            if ((p->hd.flags & 0b10000000) == 0){
                set_bkg_tiles(4, 5, 11, 3, bkg_push);
            }else{
                set_bkg_tile_xy((p->hd.h_x/8) - 2, (p->hd.h_y/8)-2, 95);
            }
            p->cash += p->bet;
            break;
        default:
            break;
    }
    if ((p->hd.flags & 0b01000000) != 0b0){          // if double down accepted flag is not 0:
        temp = p->bet/2;
        p->bet = temp;                  // reset the bet to it's original value
        p->hd.flags &= 0b10111111;     //set 'double down accepted flag' to 0
    }
    if((p->hd.flags & 0b10) != 0){
        if((dlr->hd.flags & 1) != 0){           //if insurance was accepted & 
            p->cash += p->bet;
        }
    }
}

void reveal_hole_card(player *dlr, player *p){
    // bool split_condition = false;
    // if (split_condition){printf("split");}
    //else{
    settle_score(dlr, p);
    //}
}

void insurance(player *p){
    bool break_cond = false;
    uint8_t temp = (p->bet)/2;
    if ((p->hd.flags & 1) == 0){
        set_bkg_tiles(8, 1, 12, 5, bkg_ins);
        while(1){
            waitpad(J_A | J_B);
            switch(joypad()){
                case(J_A):
                    p->cash -= temp;
                    p->hd.flags |= 0b10;
                    break_cond = true;
                    break;
                case(J_B):
                    break_cond = true;
                    break;
                default:
                    break;
            }
            waitpadup();
            if(break_cond){break;}
        }
        set_bkg_tiles(8, 1, 12, 5, bkg_tab);
        set_bkg_tiles(15, 1, 5, 2, bkg_hs_o);
    }
}
// /*
void init_split_hand_gfx(){
    set_bkg_tile_xy(2,8, 42);
    set_bkg_tiles(0, 13, 4, 5, bkg_score_w_card1);
    set_bkg_tiles(4, 13, 4,5, bkg_1_cards);
}

void split_hand_game(deck *mydeck, player *human, hand *split_hand, uint8_t *sprite_count){
    init_split_hand_gfx();
    uint8_t hidden_card_x = 0;
    uint8_t hidden_card_y = 8;
    // declare a new "split hand" and move the second card of player's hand to it
    // hand split_hand;
    // hand tmp_hand;
    hand_init(split_hand, 32,120,sprite_count);
    card_init(&(split_hand->cards[0]), human->hd.cards[1].suit_rank, 32,120, sprite_count);
    split_hand->size = 1;
    split_hand->card_reps[0] = human->hd.cards[1].suit_rank; // for consistencies sake
    split_hand->flags |= 0b10000000;         // so you can't split again

    //get a second card for your initial hand, reset values as follows
    // in order to maintain the sprite ID's
    UBYTE second_card = mydeck->cards[mydeck->size];
    human->hd.cards[1].suit_rank = second_card;
    human->hd.card_reps[1] = second_card;
    set_sprite_tile(human->hd.cards[1].spr_id[0], (int)(second_card & 0xF));
    set_sprite_tile(human->hd.cards[1].spr_id[1], (int)(second_card & 0xF0));
    mydeck->size--;
    calculate_score(&human->hd);
    show_score(&human->hd);

    recieve_card(split_hand, mydeck,sprite_count,1);
    show_score(split_hand);

    display_hand(&(human->hd));
    display_hand(split_hand);
}

// */

// void DealCards(deck * myDeck, player * human, player * bot, uint8_t * sprite_count){

// }

void player_choices(deck *myDeck, player *human, uint8_t *sprite_count, hand *split_hand){
    uint8_t hidden_card_x = 5;      //rename this variable
    uint8_t split_x = 5;
    uint8_t hidden_card_y = (human->hd.h_y/8)-2;
    uint8_t tmp;
    uint8_t break_cond = 0;
    while(1){
        if (human->hd.score >= 21){
            break_cond = 1;
        }
        else{
            waitpad(J_A | J_B | J_SELECT | J_DOWN | J_LEFT | J_UP);
            switch(joypad()){
                case J_DOWN:
                case J_A:
                    if (joypad() == J_DOWN){
                        if ((human->hd.flags & 0b10000000) == 0b0){              //if double down hand flag is set to 1(i.e. you are allowed to double down), then...
                            human->hd.flags |= 0b1000000;        //set double down accepted flag to 1 [%0100_0000]
                            human->cash -= human->bet;
                            human->bet += human->bet;
                            }
                        else{                           //otherwise, the input is invalue 
                            break;
                        }
                    }
                    if (human->hd.size < 7){
                        recieve_card(&human->hd, myDeck, sprite_count, 1);
                        set_bkg_tiles(hidden_card_x, hidden_card_y, 4, 5, bkg_1_cards);
                        hidden_card_x += 1;
                        display_hand(&(human->hd));
                        show_score(&(human->hd));
                    }
                    if (joypad() == J_DOWN){break_cond = 1;}//if J_DOWN, no more turns
                    break;
                case J_B:
                    break_cond = 1;
                    break;
                case J_SELECT:
                    if(human->hd.size == 2){
                        human->hd.flags |= 0b1000;      //set surrendered flag to 1
                        break_cond = 1;
                    }
                    break;
                case J_LEFT:
                    fadeout();
                    HIDE_SPRITES;
                    display_game_info(myDeck->size);
                    fadein();
                    waitpadup();
                    fadeout();
                    init_gfx();
                    if ((human->hd.flags & 0b10000000) != 0){
                        init_split_hand_gfx();
                        show_score(split_hand);
                        tmp = split_x;
                        for(i = 0; i < (human->hd.size - 2); i++){
                            set_bkg_tiles(tmp, (split_hand->h_y/8)-2, 4, 5, bkg_1_cards);
                            tmp += 1;
                        }
                        if(human->hd.h_y > 100){
                            set_bkg_tile_xy(2,8, 37);
                            set_bkg_tile_xy(2,13, 42);
                        }
                    }
                    if(human->hd.size > 2){
                        hidden_card_x -= (human->hd.size - 2);
                        for(i = 0; i < (human->hd.size - 2); i++){
                            set_bkg_tiles(hidden_card_x, hidden_card_y, 4, 5, bkg_1_cards);
                            hidden_card_x += 1;
                        }
                    }
                    show_score(&(human->hd));
                    SHOW_SPRITES;
                    DrawNumber((uint8_t)4,0, (uint16_t)human->cash,6);
                    set_win_tile_xy(10,0,3);
                    DrawNumber(14, 0, (uint16_t)human->bet,4);
                    fadein();
                    wait_vbl_done();
                    break;
                case J_UP:
                    // if ((human->hd.flags & 0b10000000) == 0){
                    if (((human->hd.flags & 0b100000) != 0)
                        & ((human->hd.flags & 0b10000000) == 0)){
                        human->cash -= human->bet;                                  //remove an amount equal to the original bet
                        DrawNumber((uint8_t)4,0, (uint16_t)human->cash,6);          //display a new cash balance
                        set_win_tile_xy(10,0,3);
                        DrawNumber(14, 0, (uint16_t)human->bet,4);
                        human->hd.flags |= 0b10000000;                              //set split accepted flag to 1
                        human->hd.flags &= 0b11011111;                              //set split hand flag to 0  (ensures no further splitting)
                        split_hand_game(myDeck, human, split_hand,sprite_count);    //set up split hands
                    }
                default:
                    break;
            }
            performantdelay(5);
            waitpadup();
            if (break_cond > 0){break;}
            
        }
        if(break_cond > 0){break;}
    }

}


void init_gfx(){
    set_bkg_data(0,120,bkg_tiles_alt);                  //load in the background tiles (also used in window layer)
    init_bkg(37);                                         //draws the blank "table" background to the screen
    set_sprite_data(0,114,card_tiles);                  //loads in the tiles used for the cards. 
    //used to displayer player's cards
    set_bkg_tiles(4, 8, 4, 5, bkg_1_cards);             //loads in the "5" card background for 
    set_bkg_tiles(0, 8, 4, 5, bkg_score_w_card1);       //loads in the "1st" card and the score
    //used to display dealer's cards
    set_bkg_tiles(4, 1, 4, 5, bkg_1_cards);
    set_bkg_tiles(0, 1, 4, 5, bkg_score_w_card1);
    //displays the hit or stand options
    set_bkg_tiles(15, 1, 5, 2, bkg_hs_o);
    set_win_tiles(0,0,18,1, bkg_cash_bet);              //displays the cash and bet stuff
    move_win(7,135);                                    //offset from tile grid by a single pixel
    set_win_tile_xy(10,0,3);

    SHOW_BKG;
    SHOW_SPRITES;
    SHOW_WIN;
    DISPLAY_ON;
}

void play_game(deck *myDeck, player *human, player *bot, bool same_bet){
    uint8_t bet = 0;    //this will hold the value of the input in that window
    
    // uint8_t spr_c = 16;      //general sprite index (sprite count)  [starts at 16 since preceding sprite ID's were assigned to the human an bot hands]
    uint8_t spr_c = 12;
    uint8_t hidden_card_x = 5;      //rename this variable
    
    uint8_t temp_x = 0;
    uint8_t break_cond = 0;
    uint16_t *p_numOG = &numberOfGames;
    hand sp_hd;
    // UBYTE tmp_crd_rps[8];
    // UBYTE tmp_flg;
    // uint8_t tmp_coord[2];
    // UBYTE
    UBYTE tmp_hole_card_rank;
    UBYTE tmp_hole_card_suit;
    uint8_t hole_card_spr_id[3];
    uint8_t temp_added = 16;        //this is for the hole card

    if (myDeck->size <= 11){
        init_bkg(37);
        fadein();
        free(myDeck->cards);
        // shuffle(myDeck);
        shuffle(myDeck, numberOfDecks);
        performantdelay(10);
        fadeout();
        init_gfx();        
        }

    if (!same_bet){
        // this is where I'd pull up a window prompting 
        // the user to give their cash and bet
        human->bet = bet;
        if (human->cash > human->bet){
            human->cash -= bet;    
        }
        
    }else{
        bet = human->bet;
        if (human->cash > human->bet){
            human->cash -= human->bet;      //should eventually be put in the end
        }
    }
    //display player's cash
    DrawNumber((uint8_t)4,0, (uint16_t)human->cash,6);
    set_win_tile_xy(10,0,3);
    DrawNumber(14, 0, (uint16_t)human->bet,4);

    //deals 2 cards to both dealer and human
    recieve_card(&human->hd, myDeck, &spr_c, 2);
    recieve_card(&bot->hd, myDeck, &spr_c, 2);     //2 cards dealt to dealer

    set_sprite_tile(bot->hd.cards[0].spr_id[0], 14);
    set_sprite_tile(bot->hd.cards[0].spr_id[1], 15);

    for(i = 0; i < 2; i++){
        set_sprite_tile(spr_c, 15);
        hole_card_spr_id[i] = spr_c;
        move_sprite(spr_c, bot->hd.h_x, (bot->hd.h_y + temp_added));
        temp_added += 8;
        spr_c ++;
    }

    set_sprite_tile(spr_c, 14);
    hole_card_spr_id[2] = spr_c;

    set_sprite_prop(spr_c, S_FLIPY);
    move_sprite(spr_c, bot->hd.h_x, (bot->hd.h_y + temp_added));
    spr_c++;

    for(i = 0; i < 2; i++){
        display_card_1(bot->hd.cards[i].spr_id[0],
                        bot->hd.cards[i].spr_id[1],
                        bot->hd.h_x + temp_x, bot->hd.h_y);
        temp_x += 8;
    }
    display_last_card(&bot->hd);
    display_hand(&(human->hd));
    show_score(&(human->hd));

    fadein();

    //test for insurance HERE
    if ((bot->hd.card_reps[1] & 0b1111) == 1){      // if the up card is an Ace (==1)
        performantdelay(2);
        insurance(human);
        DrawNumber((uint8_t)4,0, (uint16_t)human->cash,6);
        set_win_tile_xy(10,0,3);
        DrawNumber(14, 0, (uint16_t)human->bet,4);
    }
    
    //offer player their choices on how to continue
    player_choices(myDeck, human, &spr_c, &sp_hd);

    if ((human->hd.flags & 0b10000000) != 0){
        //only triggers if a split hand is detected
        hand swap_hand;
        swap_hand = human->hd;
        set_bkg_tile_xy(2,8, 37);
        set_bkg_tile_xy(2,13, 42);
        human->hd = sp_hd;
        player_choices(myDeck, human, &spr_c, &swap_hand);

        /****
         the part at the breakpoint must go later
        */
        sp_hd = human->hd;
        human->hd = swap_hand;
    }

    //dealer 
    tmp_hole_card_rank = (bot->hd.cards[0].suit_rank & 0xF); 
    tmp_hole_card_suit = (bot->hd.cards[0].suit_rank & 0xF0);
    
    set_sprite_tile((int)bot->hd.cards[0].spr_id[0], (int)tmp_hole_card_rank);
    set_sprite_tile((int)bot->hd.cards[0].spr_id[1], (int)tmp_hole_card_suit);

    for(i = 0; i <= 2; i++){
        set_sprite_tile(hole_card_spr_id[i], 0);
    }
    if ((human->hd.flags & 0b1000) == 0b0){     //if the surrendered flag is 0 (player didn't sur.)
        if ((human->hd.score <= 21 && ((human->hd.flags & 1)==0)) | 
            ((human->hd.flags & 0b10000000) != 0)){                     //in case 1 hand busts and the other doesnt
            dealer_draws(myDeck, &bot->hd, &spr_c); //proceed to evaluate the scores
        }
        display_hand(&bot->hd);
        show_score(&(bot->hd));
        reveal_hole_card(bot,human);
        if ((human->hd.flags & 0b10000000) != 0){
            hand swap_hand;
            swap_hand = human->hd;
            human->hd = sp_hd;
            reveal_hole_card(bot,human);
            human->hd = swap_hand;
        }
    }else{
        human->cash += ((human->bet)/2);
        if((human->bet % 2) != 0){
            human->half_dollar = true;          //if the bet is an uneven number, then set the half dollar to true
        }
        display_hand(&bot->hd);
        show_score(&(bot->hd));
        set_bkg_tiles(4, 5, 13, 3, bkg_surrender);
    }

    performantdelay(10);
    set_win_tiles(0,0,18,1, bkg_cash_bet);
    // display player's new cash amount
    DrawNumber((uint8_t)4,0, (uint16_t)human->cash,6);
    set_win_tile_xy(10,0,3);
    DrawNumber(14, 0, 0,4);

    waitpad(J_START | J_A | J_B);
    fadeout();

    //remove played cards from deck
    myDeck->cards = realloc(myDeck->cards, myDeck->size * sizeof(UBYTE));

    init_bkg(37);                                         //draws the blank "table" background to the screen
    //used to displayer player's cards
    set_bkg_tiles(4, 8, 4, 5, bkg_1_cards);             //loads in the "5" card background for 
    set_bkg_tiles(0, 8, 4, 5, bkg_score_w_card1);       //loads in the "1st" card and the score
    //used to display dealer's cards
    set_bkg_tiles(4, 1, 4, 5, bkg_1_cards);
    set_bkg_tiles(0, 1, 4, 5, bkg_score_w_card1);
    //displays the hit or stand options
    set_bkg_tiles(15, 1, 5, 2, bkg_hs_o);
    
    for(i = 0; i < 8; i++){human->hd.card_reps[i] = 0;}
    for(i = 0; i < 8; i++){bot->hd.card_reps[i] = 0;}
    human->hd.size = 0;
    human->hd.score = 0;
    human->hd.flags &= 0;
    bot->hd.size = 0;
    bot->hd.score = 0;
    bot->hd.flags &= 0;

    for (i=0;i < spr_c;i++){
        set_sprite_tile(i, 0);
    }
    *p_numOG = (*p_numOG) + 1;
}

void main(){
    printf("\n\n\n\n\n\n\t\t   Blackjack\n\n\n\n\n\n\t\t\tPRESS START");
    waitpad(J_START);
    fadeout();
    init_gfx();
    
    uint8_t sprite_count = 0;
    deck mdeck;
    shuffle(&mdeck, numberOfDecks);
    player bot;
    player human;
    bot.cash = 0;
    bot.bet = 0;
    bot.half_dollar = false;
    human.cash = 1500;
    human.bet = 25;
    human.half_dollar = false;
    hand_init(&human.hd, 32, 80, &sprite_count);
    hand_init(&bot.hd, 32, 24, &sprite_count);           //dealer's starting x = 32, y = 24

    while(1){
        play_game(&mdeck, &human, &bot, true);
        performantdelay(9);
    }
}

/******************************
Simple to do:
7 x 2 x 3 = 42
3 x 6 = 18
    >> we need to do something so that the sprite limit isn't surpassed when split hands happen

To Be Added:
    >> Changing Bets
    >> Press left to view
        the current count on the deck
    >> Option's menu to toggle rules
    >> Sidebets (KB & LL)
    >> should also have a little star or x as to 
        whether the player followed basic strategy

change all uses of tile #37 to tile #10

*******************************/