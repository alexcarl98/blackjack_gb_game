#include <gb/gb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <rand.h>

#include "..\res\bkg_single_cards.h"
#include "..\res\bkg_sc_c1.h"
#include "..\res\bkg_hs_opt.h"
#include "..\res\bkg_win.h"
#include "..\res\bkg_pushgm.h"
#include "..\res\bkg_lose.h"
#include "..\res\bkg_sur.h"
#include "..\res\bkg_table.h"
#include "..\res\bkg_insurance.h"
#include "..\res\bkg_win_cash_bet.h"

#include "..\res\bkg_tiles_alt.h"
#include "..\res\card_sprites.h"
#include "card_structures.c"

uint16_t numberOfGames = 0;
uint8_t i,j,k;
UBYTE player_split_cards[12];

void dealer_draws(deck *d, hand *dlr_h, uint8_t * sprite_count);
void play_game(deck * myDeck, player * human, player * bot, bool same_bet);
void DrawNumber(uint8_t x,uint8_t y, uint16_t number,uint8_t digits);
void performantdelay(uint8_t numloops);
void fadeout();
void fadein();
void DealCards(deck * myDeck, player * human, player * bot, uint8_t * sprite_count);
void PlayerMoves();
void ResetGame();
void DisplayDealerUpcard();
UBYTE outcome(UBYTE p_score, UBYTE p_flags, UBYTE d_score, UBYTE d_flags);
void settle_score(player *dlr, player *p);
void reveal_hole_card(player *dlr, player *p);
void insurance(player *p);


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
        while ((dlr_h->score < 17) && dlr_h->size < 6){
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
            set_bkg_tiles(5, 5, 10, 3, bkg_winner);
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
            set_bkg_tiles(5, 5, 10, 3, bkg_loser);
            break;
        case 0x3:
            set_bkg_tiles(4, 5, 11, 3, bkg_push);
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

// void split_hand_game(deck *mydeck, player *human, uint8_t *sprite_count){
//     hand split_hand;
//     uint8_t hidden_card_x = 0;
//     uint8_t hidden_card_y = 8;
//     hand_init(&split_hand, 32,120,sprite_count);
//     card_init(&split_hand.cards[0], human->hd.cards[1].suit_rank, 32,120, sprite_count);
//     // player_split_cards[0] = human->hd.cards[0].suit_rank;
//     // player_split_cards[7] = human->hd.cards[1].suit_rank;
//     // human->hd.size = 1;
//     split_hand.size = 1;
//     // recieve_card(&(human->hd), mydeck,sprite_count,1);
//     //replacing the second card in the first hand with a new hand, whilst, maintaining the sprite ID's 
//     human->hd.cards[1].suit_rank = mydeck->card_rep[mydeck->size];
//     human->hd.cards[1].value = (mydeck->card_rep[mydeck->size] & 0b1111);
//     if (human->hd.cards[1].value > 10){
//         human->hd.cards[1].value = 10;
//     }
//     mydeck->size--;
//     score_calc(&(human->hd));
//     recieve_card(&split_hand, mydeck,sprite_count,1);
//     // recieve_card(mydeck);

//     set_bkg_tiles(hidden_card_x, hidden_card_y, 9, 5, bkg_6_cards);
//     hidden_card_x += 1;
//     set_bkg_tiles(0, hidden_card_y, 4, 5, bkg_score_w_card1);
//     set_bkg_tiles(hidden_card_x, hidden_card_y+5, 9, 5, bkg_6_cards);
//     hidden_card_x += 1;
//     set_bkg_tiles(0, hidden_card_y+5, 4, 5, bkg_score_w_card1);

    
//     display_hand(&(human->hd));
//     display_hand(&split_hand);
// }

void play_game(deck *myDeck, player *human, player *bot, bool same_bet){
    uint8_t bet = 0;    //this will hold the value of the input in that window
    uint8_t spr_c = 16;      //general sprite index (sprite count)  [starts at 16 since preceding sprite ID's were assigned to the human an bot hands]
    uint8_t hidden_card_x = 5;      //rename this variable
    uint8_t temp_x = 0;
    uint8_t break_cond = 0;
    uint16_t *p_numOG = &numberOfGames;
    // UBYTE
    UBYTE tmp_hole_card_rank;
    UBYTE tmp_hole_card_suit;
    uint8_t hole_card_spr_id[3];
    uint8_t temp_added = 16;        //this is for the hole card

    if (myDeck->size <= 11){
        free(myDeck->cards);
        shuffle(myDeck);
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
    // hand_init(&(human->hd), 32, 80, &spr_c);
    // hand_init(&bot->hd, 32, 24, &spr_c);           //dealer's starting x = 32, y = 24
    //test
    // deck tmp_d;
    // tmp_d.size = 2;
    // tmp_d.card_rep[0] = 0x11;
    // tmp_d.card_rep[1] = 0x21;
    // tmp_d.card_rep[2] = 0x31;
    // recieve_card(&human->hd, &tmp_d, &spr_c, 2);
    

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

    SHOW_BKG;
    SHOW_SPRITES;
    SHOW_WIN;
    DISPLAY_ON;

    fadein();

    //test for insurance HERE
    if (bot->hd.cards[1].value == 1){
        performantdelay(2);
        insurance(human);
        DrawNumber((uint8_t)4,0, (uint16_t)human->cash,6);
        set_win_tile_xy(10,0,3);
        DrawNumber(14, 0, (uint16_t)human->bet,4);
    }
    
    // if((human->hd.flags & 0b10000) != 0b0){
    //     set_bkg_tiles(15, 1, 5, 2, bkg_hs_o);
    // }

    while(1){
        if (human->hd.score >= 21){
            break_cond = 1;
        }
        else{
            // waitpad(J_A | J_B | J_SELECT | J_DOWN | J_UP);
            waitpad(J_A | J_B | J_SELECT | J_DOWN);
            switch(joypad()){
                case J_DOWN:
                case J_A:
                    if (joypad() == J_DOWN){
                        if ((human->hd.flags & 0b10000) != 0b0){              //if double down hand flag is set to 1(i.e. you are allowed to double down), then...
                            human->hd.flags |= 0b1000000;        //set double down accepted flag to 1 [%0100_0000]
                            human->cash -= human->bet;
                            human->bet += human->bet;
                            }           
                        else{                           //otherwise, the input is invalue 
                            //play invalid input noise code goes here
                            break;
                        }
            }
                    if (human->hd.size < 6){

                        recieve_card(&human->hd, myDeck, &spr_c, 1);
                        set_bkg_tiles(hidden_card_x, 8, 4, 5, bkg_1_cards);
                        hidden_card_x += 1;
                        display_hand(&(human->hd));
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
                
                // case J_UP:
                //     human->cash -= human->bet;     
                //     DrawNumber((uint8_t)4,0, (uint16_t)human->cash,6);
                //     split_hand_game(myDeck,human, &spr_c);
                default:
                    break;
            }
            performantdelay(5);
            waitpadup();
            if (break_cond > 0){break;}
        }
        if(break_cond > 0){break;}
    }
    //dealer 
    tmp_hole_card_rank = (bot->hd.cards[0].suit_rank & 0xF); 
    tmp_hole_card_suit = (bot->hd.cards[0].suit_rank & 0xF0);
    // bot->hd.cards[i].suit_rank;
    
    set_sprite_tile((int)bot->hd.cards[0].spr_id[0], (int)tmp_hole_card_rank);
    set_sprite_tile((int)bot->hd.cards[0].spr_id[1], (int)tmp_hole_card_suit);

    for(i = 0; i <= 2; i++){
        set_sprite_tile(hole_card_spr_id[i], 0);
    }
    if ((human->hd.flags & 0b1000) == 0b0){     //if the surrendered flag is 0 (player didn't sur.)
        if (human->hd.score <= 21 && ((human->hd.flags & 1)==0)){
            dealer_draws(myDeck, &bot->hd, &spr_c); //proceed to evaluate the scores
        }
        // display_score(&(bot->hd));
        display_hand(&bot->hd);
        reveal_hole_card(bot,human);
            
    }else{
        human->cash += ((human->bet)/2);
        if((human->bet % 2) != 0){
            human->half_dollar = true;          //if the bet is an uneven number, then set the half dollar to true
        }
        display_hand(&bot->hd);
        set_bkg_tiles(4, 5, 13, 3, bkg_surrender);
    }

    performantdelay(10);

    set_win_tiles(0,0,18,1, bkg_cash_bet);
    //display player's new cash amount
    DrawNumber((uint8_t)4,0, (uint16_t)human->cash,6);
    set_win_tile_xy(10,0,3);
    DrawNumber(14, 0, 0,4);

    waitpad(J_START | J_A | J_B);
    fadeout();

    //reset values in the human and bot hands.

    myDeck->cards = realloc(myDeck->cards, myDeck->size * sizeof(UBYTE));

    HIDE_BKG;
    HIDE_SPRITES;
    HIDE_WIN;

    // set_bkg_tiles(0, 0, 20, 18, bkg_tab);
    
    // init_bkg(37);
    if(human->hd.size > 2){
        set_bkg_tiles(4, 8, 4, 5, bkg_1_cards);
        set_bkg_tiles(8, 8, 5, 5, bkg_tab);
    }
    if(bot->hd.size > 2){ 
        set_bkg_tiles(8, 1, 5, 2, bkg_tab);
    }
    set_bkg_tiles(4, 1, 4, 5, bkg_1_cards);
    set_bkg_tiles(8,3, 12, 3, bkg_tab);
    set_bkg_tiles(4,6, 14, 2, bkg_tab);
    // set_bkg_tiles(0, 8, 4, 5, bkg_score_w_card1);
    for(i = 0; i < 6; i++){
        human->hd.cards[i].value = 0;
        bot->hd.cards[i].value = 0;
    }
    human->hd.size = 0;
    human->hd.score = 0;
    human->hd.flags &= 0;
    bot->hd.size = 0;
    bot->hd.score = 0;
    bot->hd.flags &= 0;

    for (i=0;i < spr_c;i++){
        set_sprite_tile(i, 0);
    }
    
    // set_bkg_tiles(0, 1, 4, 5, bkg_score_w_card1);
    // set_bkg_tiles(15, 1, 5, 2, bkg_hs_o);

    // set_win_tiles(0,0,18,1, bkg_cash_bet);
    // move_win(7,135);
    // if (human->cash == 0){
    //     main();
    // }
    // else{
    *p_numOG = (*p_numOG) + 1;
    performantdelay(5);
    play_game(myDeck, human, bot, true);
    // }
}
void main(){
    //the starting position for the player's tiles to be displayed
    printf("\n\n\n\n\n\n\t\t  Blackjack\n\n\n\n\n\n\t\t\tPRESS START");
    
    waitpad(J_START);
    fadeout();
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
    //note on how the card display works:
    // 1) loads in the 5 card image to the backgorund 
    // 2) covers part of the image with the score to only show 2 cards
    // 3) each time a card is added, the card background basically shifts over by 1 tile.
    uint8_t sprite_count = 0;
    deck mdeck;
    shuffle(&mdeck);
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

    play_game(&mdeck, &human, &bot, true);
}