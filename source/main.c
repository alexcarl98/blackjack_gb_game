#include <gb/gb.h>
#include <stdio.h>
#include <stdlib.h>
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
#include "..\res\change_bet.h"           // bkg_chg_bet
#include "..\res\chips_win.h"           // chips
#include "..\res\hole_card.h"           // hole
#include "..\res\table_change.h"        // bkg_tab_c
#include "..\res\bkg_shuf.h"
#include "..\res\bkg_tiles_o.h"

#include "..\res\starting_amt.h"
#include "..\res\end_amt.h"
#include "..\res\press_to_chg_bet.h"


#include "..\res\bkg_tiles_alt.h"
#include "..\res\card_sprites.h"
#include "cards_n_such.c"
#include "common.h"

uint8_t numberOfDecks = 4;
// uint16_t numberOfGames = 0;
uint8_t i,j,k;
//uint8_t limits[] = [0,0];

UBYTE game_flags;
/*-------------(game_flags)----------------*
UBYTE game_flags
0 - using the same bet
1 - show hand values
2 - dealer hits on soft 17
3 - double down after splitting
_
4 - insurance
5 - Surrenders
*------------------------------------------*/
deck myDeck;
uint8_t sprite_count;
hand dealer_hd;
player human;
uint16_t start_cash;

//settle_score , dealer_draws, playgame

/*--------(will be global variables)--------*
uint8_t sprite_count;
player human;
*------------------------------------------*/

//gamestate - Running
void init_gfx();
void show_score(hand *self);
void dealer_draws(hand *dlr_h);
UBYTE outcome(UBYTE p_score, UBYTE p_flags, UBYTE d_score, UBYTE d_flags);
void init_split_hand_gfx();
void split_hand_game( hand *split_hand);
void player_choices(hand *split_hand);
void settle_score();
void insurance();
void reset_game_board();
void deal_first_two_cards(UBYTE * hole_rank, UBYTE * hole_suit);
void game_player_deciding(hand *sp_hd);
void game_ending_sequence(hand sp_hd, UBYTE hole_rank,UBYTE hole_suit);
void gamestate_running();





//gamestate - change/view parameters
void display_game_info(uint16_t number);
void player_bet();
void gamestate_restart();


//---------------------------------------------------------------------------------------------------------------------------------//
//==========================================================(gamestate - Running)==========================================================//
//from when the first cards are dealt to when the dealer's cards are revealed
void init_gfx(){
    set_bkg_data(0,123,bkg_tiles_alt);                  //load in the background tiles (also used in window layer)
    init_bkg(48);                                         //draws the blank "table" background to the screen
    init_win(47);
    set_sprite_data(0,114,card_tiles);                  //loads in the tiles used for the cards.
    //used to displayer player's cards
    set_bkg_tiles(4, 8, 4, 5, bkg_1_cards);             //loads in the "5" card background for
    set_bkg_tiles(0, 8, 4, 5, bkg_score_w_card1);       //loads in the "1st" card and the score
    //used to display dealer's cards
    set_bkg_tiles(4, 1, 4, 5, bkg_1_cards);
    set_bkg_tiles(3,1,1,5,hole);

    //displays the hit or stand options
    set_bkg_tiles(15, 1, 5, 5, bkg_hs_o);
    set_win_tiles(1,0,14,1, bkg_cash_bet);              //displays the cash and bet stuff
    move_win(7,136);                                    //offset from tile grid by a single pixel
    set_win_tile_xy(10,0,45);

    SHOW_BKG;
    SHOW_SPRITES;
    SHOW_WIN;
    DISPLAY_ON;
}


void show_score(hand *self){
    // the hand's x & y pivot is passed in and the new x and y positions are calculated in the function
    uint8_t score_x = (self->h_x/8) - 3;
    uint8_t score_y = (self->h_y/8);

    if ((self->flags & 1) != 0){            //display 'BJ' if player has blackjack
        VBK_REG=0;
        set_bkg_tile_xy(score_x, score_y, 11);
        set_bkg_tile_xy(score_x+1, score_y, 19);
    }
    else{
        if((self->flags & 0b100) != 0){
            VBK_REG=0;
            set_bkg_tile_xy(score_x-1, score_y, 49);
        }else{
            VBK_REG=0;
            set_bkg_tile_xy(score_x-1, score_y, 48);
        }
        DrawNumberBkg(score_x, score_y, (uint16_t)self->score, 2);
    }
}

//experimental
void show_card(UBYTE ID_rank, UBYTE ID_suit, uint8_t x, uint8_t y){
    // UBYTE rank_tile_offset = 97;       //Ace card tile starts at 98, so we will start it at 97
    // UBYTE suit_tile_offset = 110;     // suit tiles begin at 111, so we will start at 110
    UBYTE something_1;
    UBYTE something_2;
    UBYTE tmp_s;
    uint8_t tmp_x;
    uint8_t tmp_y;
    tmp_x = (x/8);
    tmp_x -= 1;
    tmp_y = (y/8);
    tmp_y -= 2;

    something_1 = 97 +ID_rank;
    tmp_s = ID_suit;
    tmp_s >>= 4;
    something_2 = tmp_s+ 110;

    set_bkg_tile_xy(tmp_x,tmp_y,(uint8_t)(something_1));
    set_bkg_tile_xy(tmp_x,(tmp_y+1),(uint8_t)(something_2));
    
}


void display_hand_static(hand * self){
    uint8_t i;
    uint8_t temp_x = 0;
    for(i = 0; i < self->size; i++){
        show_card((self->cards[i].suit_rank & 0xF), 
                        (self->cards[i].suit_rank & 0xF0),
                        self->h_x + temp_x, self->h_y);
        temp_x += 8;
    }
    display_last_card(&(*self));
}
/////////////////////////////////////////////


void dealer_draws(hand *dlr_h){
    uint8_t hidden_card_x = 5;
    if (dlr_h->score < 17){
        while ((dlr_h->score < 17) && dlr_h->size < 7){
            recieve_card(dlr_h, &myDeck, 1);
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

void init_split_hand_gfx(){
    set_bkg_tile_xy(2,8, 50);
    set_bkg_tiles(0, 13, 4, 5, bkg_score_w_card1);
    set_bkg_tiles(4, 13, 4,5, bkg_1_cards);
}

void split_hand_game(hand *split_hand){
    init_split_hand_gfx();
    uint8_t hidden_card_x = 0;
    uint8_t hidden_card_y = 8;
    // declare a new "split hand" and move the second card of player's hand to it
    hand_init(split_hand, 32,120, &sprite_count);
    card_init(&(split_hand->cards[0]), human.hd.cards[1].suit_rank, 32,120);
    split_hand->size = 1;
    split_hand->card_reps[0] = human.hd.cards[1].suit_rank; // for consistencies sake
    split_hand->flags |= 0b10000000;         // so you can't split again

    //get a second card for your initial hand, reset values as follows
    // in order to maintain the sprite ID's
    UBYTE second_card = myDeck.cards[myDeck.size];
    count_card(second_card);
    human.hd.cards[1].suit_rank = second_card;
    human.hd.card_reps[1] = second_card;
    // set_sprite_tile(human.hd.cards[1].spr_id[0], (int)(second_card & 0xF));
    // set_sprite_tile(human.hd.cards[1].spr_id[1], (int)(second_card & 0xF0));
    myDeck.size--;
    calculate_score(&human.hd);
    show_score(&human.hd);

    recieve_card(split_hand, &myDeck, 1);
    show_score(split_hand);

    display_hand_static(&(human.hd));
    display_hand_static(split_hand);
}

void player_choices(hand *split_hand){
    uint8_t hidden_card_x = 5;      //rename this variable
    uint8_t split_x = 5;
    uint8_t hidden_card_y = (human.hd.h_y/8)-2;
    uint8_t tmp;
    uint8_t break_cond = 0;
    UBYTE tmp_card;
    
    while(1){
        if (human.hd.score >= 21){
            break_cond = 1;
        }
        else{
            waitpad(J_A | J_B | J_SELECT | J_DOWN | J_LEFT | J_UP);
            switch(joypad()){
                case J_DOWN:
                case J_A:
                    if (joypad() == J_DOWN){
                        if ((human.hd.flags & 0b10000000) == 0b0){              //if double down hand flag is set to 1(i.e. you are allowed to double down), then...
                            human.hd.flags |= 0b1000000;        //set double down accepted flag to 1 [%0100_0000]
                            human.cash -= human.bet;
                            human.bet += human.bet;
                            }
                        else{                           //otherwise, the input is invalue
                            break;
                        }
                    }
                    if (human.hd.size < 7){
                        tmp_card = myDeck.cards[(myDeck.size-1)];
                        recieve_card(&human.hd, &myDeck, 1);
                        set_bkg_tiles(hidden_card_x, hidden_card_y, 4, 5, bkg_1_cards);
                        hidden_card_x += 1;
                        show_card((tmp_card & 0xF),(tmp_card & 0xF0),
                                (human.hd.h_x + (8 *(human.hd.size - 1))),human.hd.h_y);
                        display_last_card(&human.hd);
                        show_score(&(human.hd));
                    }
                    if (joypad() == J_DOWN){break_cond = 1;}//if J_DOWN, no more turns
                    break;
                case J_B:
                    break_cond = 1;
                    break;
                case J_SELECT:
                    if(human.hd.size == 2){
                        human.hd.flags |= 0b1000;      //set surrendered flag to 1
                        break_cond = 1;
                    }
                    break;
                case J_LEFT:
                    /*with a split hand:
                        moves ->hit ->press left : there will be an extra card in the second hand below
                    */
                    fadeout();
                    HIDE_SPRITES;
                    display_game_info(myDeck.size);
                    fadein();
                    waitpadup();
                    fadeout();
                    init_gfx();

                    if ((human.hd.flags & 0b10000000) != 0){                                //if the player has split
                        init_split_hand_gfx();                                              //set up the split hand gfx
                        show_score(split_hand);                                             // show the score of the split hand
                        tmp = split_x;
                        for(i = 0; i < (split_hand->size - 2); i++){
                            set_bkg_tiles(tmp, (split_hand->h_y/8)-2, 4, 5, bkg_1_cards);
                            tmp += 1;
                        }
                        display_hand_static(split_hand);
                        if(human.hd.h_y > 100){
                            set_bkg_tile_xy(2,8, 48);
                            set_bkg_tile_xy(2,13, 50);
                        }
                    }
                    if(human.hd.size > 2){
                        hidden_card_x -= (human.hd.size - 2);
                        for(i = 0; i < (human.hd.size - 2); i++){
                            set_bkg_tiles(hidden_card_x, hidden_card_y, 4, 5, bkg_1_cards);
                            hidden_card_x += 1;
                        }
                    }
                    display_hand_static(&dealer_hd);
                    set_bkg_tiles(3,1,1,5,hole);
                    
                    display_hand_static(&human.hd);
                    show_score(&(human.hd));
                    SHOW_SPRITES;
                    DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
                    set_win_tile_xy(10,0,45);
                    DrawNumber(15, 0, (uint16_t)human.bet,4);
                    fadein();
                    wait_vbl_done();
                    break;
                case J_UP:
                    // if ((human.hd.flags & 0b10000000) == 0){
                    if (((human.hd.flags & 0b100000) != 0)
                        & ((human.hd.flags & 0b10000000) == 0)){
                        human.cash -= human.bet;                                  //remove an amount equal to the original bet
                        DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);          //display a new cash balance
                        set_win_tile_xy(10,0,45);
                        DrawNumber(15, 0, (uint16_t)human.bet,4);
                        human.hd.flags |= 0b10000000;                              //set split accepted flag to 1
                        human.hd.flags &= 0b11011111;                              //set split hand flag to 0  (ensures no further splitting)
                        split_hand_game(split_hand);    //set up split hands
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

void settle_score(){
    UBYTE result = outcome(human.hd.score, human.hd.flags, dealer_hd.score, dealer_hd.flags);
    uint16_t temp;
    switch(result){
        case 0x1:
            //if player wins, display win
            if ((human.hd.flags & 0b10000000) == 0){
                set_bkg_tiles(5, 5, 10, 3, bkg_winner);
            }else{
                set_bkg_tile_xy((human.hd.h_x/8) - 2, (human.hd.h_y/8)-2, 32);
            }
            human.cash += (human.bet);
            human.cash += (human.bet);
            if ((human.hd.flags & 0b1) != 0b0){
                temp = (human.bet)/2;
                if((human.bet % 2) != 0){
                    human.half_dollar = true;
                }
                human.cash += (temp);
                }
            break;
        case 0x2:
            if ((human.hd.flags & 0b10000000) == 0){
                set_bkg_tiles(5, 5, 10, 3, bkg_loser);
            }else{
                set_bkg_tile_xy((human.hd.h_x/8) - 2, (human.hd.h_y/8)-2, 21);
            }
            break;
        case 0x3:
            if ((human.hd.flags & 0b10000000) == 0){
                set_bkg_tiles(4, 5, 6, 3, bkg_push);
            }else{
                set_bkg_tile_xy((human.hd.h_x/8) - 2, (human.hd.h_y/8)-2, 25);
            }
            human.cash += human.bet;
            break;
        default:
            break;
    }
    if ((human.hd.flags & 0b01000000) != 0b0){          // if double down accepted flag is not 0:
        temp = human.bet/2;
        human.bet = temp;                  // reset the bet to it's original value
        human.hd.flags &= 0b10111111;     //set 'double down accepted flag' to 0
    }
    if((human.hd.flags & 0b10) != 0){
        if((dealer_hd.flags & 1) != 0){           //if insurance was accepted &
            human.cash += human.bet;
        }
    }
}

void insurance(){
    bool break_cond = false;
    uint8_t temp = (human.bet)/2;
    if ((human.hd.flags & 1) == 0){                // if player doesn't have blackjack
        set_bkg_tiles(8, 1, 12, 5, bkg_ins);    // display insurance prompt
        while(1){
            waitpad(J_A | J_B | J_LEFT);
            switch(joypad()){
                case(J_A):                      // A accepts insurance
                    human.cash -= temp;
                    human.hd.flags |= 0b10;
                    break_cond = true;
                    break;
                case(J_B):                      // B forgoes insurance
                    break_cond = true;
                    break;
                case(J_LEFT):                   // left lets you check out the count
                    fadeout();
                    HIDE_SPRITES;
                    display_game_info(myDeck.size);
                    fadein();
                    waitpadup();
                    fadeout();
                    init_gfx();
                    set_bkg_tiles(8, 1, 12, 5, bkg_ins);
                    show_score(&(human.hd));
                    display_hand_static(&dealer_hd);
                    set_bkg_tiles(3,1,1,5,hole);
                    display_hand_static(&human.hd);
                    SHOW_SPRITES;
                    DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
                    set_win_tile_xy(10,0,45);
                    DrawNumber(15, 0, (uint16_t)human.bet,4);
                    fadein();
                    wait_vbl_done();
                    break;
                default:
                    break;
            }
            waitpadup();
            if(break_cond){break;}
        }
        set_bkg_tiles(8, 1, 12, 5, bkg_tab);
        set_bkg_tiles(15, 1, 5, 5, bkg_hs_o);
    }
}

void reset_game_board(){
    //remove played cards from deck, i.e. free up space in memory of the used hex values.
    init_gfx();

    //displays the hit or stand options
    for(i = 0; i < 8; i++){human.hd.card_reps[i] = 0;}
    for(i = 0; i < 8; i++){dealer_hd.card_reps[i] = 0;}
    human.hd.size = 0;
    human.hd.score = 0;
    human.hd.flags &= 0;
    dealer_hd.size = 0;
    dealer_hd.score = 0;
    dealer_hd.flags &= 0;
    for (i=0;i < sprite_count;i++){set_sprite_tile(i, 0);}
    //*p_numOG = (*p_numOG) + 1;
}

void initialize_game(){
    //function that sets up a new game, making sure theres enough cards in the deck, 
    //taking away the amount that the player bet, and then displaying the player's cash
        // (0) if there are enough cards to deal, then proceed, otherwise, re-shuffle the deck
    start_cash = human.cash;

    if (myDeck.size <= 20){
        HIDE_WIN;
        init_bkg(48);
        set_bkg_tiles(4,4,11,3,bkg_shuffle);
        fadein();
        count = 0;
        free(myDeck.cards);
        shuffle(&myDeck, numberOfDecks);
        performantdelay(10);
        fadeout();
        init_gfx();
    }

    //(0.a) remove money from the player's bankroll
    if (human.cash >= human.bet){
        human.cash -= human.bet;      //should eventually be put in the end
    }

    // (0.b) display player's cash
    DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
    set_win_tile_xy(10,0,45);
    DrawNumber(15, 0, (uint16_t)human.bet,4);
}

void deal_first_two_cards(UBYTE * hole_rank, UBYTE * hole_suit){
    // (1)      two cards are dealt to both the dealer and human
    // debug split using 2 aces
    // deck tmp_d;
    // tmp_d.size = 2;
    // tmp_d.cards = (UBYTE*)malloc(2 * sizeof(UBYTE));
    // tmp_d.cards[0] = 0x11;
    // tmp_d.cards[1] = 0x21;

    recieve_card(&human.hd, &myDeck, 2);

    // (1.a) withhold hole card from count by offsetting count's value
    *hole_rank = myDeck.cards[(myDeck.size - 1)] & 0xF;
    *hole_suit = myDeck.cards[(myDeck.size - 1)] & 0xF0;
    if ((*hole_rank > 9) | (*hole_rank < 1)){
        count += 1;
    }else if ((1 < *hole_rank)&& (*hole_rank < 7)){
        count -= 1;
    }
    recieve_card(&dealer_hd, &myDeck, 2);
    // (1.b) hide tiles of the hole card


    // (1.c) actually display both cards
    display_hand_static(&dealer_hd);
    set_bkg_tiles(3,1,1,5,hole);

    display_hand_static(&(human.hd));

    // display_hand(&(human.hd));
    show_score(&(human.hd));

    fadein();

    // (1.d) if the parameters to offer insurance are met, then pause and offer insurance
    if ((dealer_hd.card_reps[1] & 0b1111) == 1){
        performantdelay(2);
        insurance();
        DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
        set_win_tile_xy(10,0,45);
        DrawNumber(15, 0, (uint16_t)human.bet,4);
    }
}

void game_player_deciding(hand *sp_hd){
    display_hand_static(&(human.hd));
    // (2) Offer player their choices on how they wish to continue with the hand they were dealt
    player_choices(sp_hd);

    // (2.a) if the player decided to split, proceed to call the player_choices function again with the split hand.
    if ((human.hd.flags & 0b10000000) != 0){
        //only triggers if a split accepted flag is detected
        hand swap_hand;
        swap_hand = human.hd;
        set_bkg_tile_xy(2,8, 48);
        set_bkg_tile_xy(2,13, 50);
        human.hd = *sp_hd;
        player_choices(&swap_hand);
        *sp_hd = human.hd;
        human.hd = swap_hand;
    }
    // (2.b) after the turn is over, hide tiles that display the play options
    set_bkg_tiles(15, 1, 5, 5, bkg_tab);

}

void game_ending_sequence(hand sp_hd, UBYTE hole_rank,UBYTE hole_suit){
        // (3) Dealer display's hole card
    // set_sprite_tile((int)dealer_hd.cards[0].spr_id[0], (int)hole_rank);
    // set_sprite_tile((int)dealer_hd.cards[0].spr_id[1], (int)hole_suit);
    set_bkg_tiles(0, 1, 4, 5, bkg_score_w_card1);
    count_card(hole_rank);                // now you count the card

    //(3.a) Dealer's cards determine the outcome of the game
    if ((human.hd.flags & 0b1000) == 0b0){     //if the surrendered flag is 0 (player didn't sur.)
        // (3.a.i) if the player hasn't busted, or got a split hand, then call function for dealer to draw cards
        if ((human.hd.score <= 21 && ((human.hd.flags & 1)==0)) |
            ((human.hd.flags & 0b10000000) != 0)){                     //in case 1 hand busts and the other doesnt
            dealer_draws(&dealer_hd); //proceed to evaluate the scores
        }
        // (3.a.ii) display scores, then determine which hands win against which
        display_hand_static(&dealer_hd);
        show_score(&(dealer_hd));
        settle_score();
        if ((human.hd.flags & 0b10000000) != 0){
            hand swap_hand;
            swap_hand = human.hd;
            human.hd = sp_hd;
            settle_score();
            human.hd = swap_hand;
        }
    }
    //(3.b) if the player surrendered, then complete those functions
    else{
        human.cash += ((human.bet)/2);
        if((human.bet % 2) != 0){
            human.half_dollar = true;          //if the bet is an uneven number, then set the half dollar to true
        }
        display_hand_static(&dealer_hd);
        show_score(&(dealer_hd));
        set_bkg_tiles(4, 5, 13, 3, bkg_surrender);
    }

    //(3.c) display the player's new cash amount
    performantdelay(10);
    // display player's new cash amount
    DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
    set_win_tile_xy(10,0,45);
    DrawNumber(15, 0, 0,4);
    myDeck.cards = realloc(myDeck.cards, myDeck.size * sizeof(UBYTE));

    waitpad(J_A | J_UP);

}

void gamestate_running(){
    sprite_count = 12;               //general sprite index (sprite count)  [starts at 16 since preceding sprite ID's were assigned to the human an bot hands]
    hand sp_hd;

    UBYTE tmp_hole_card_rank;
    UBYTE tmp_hole_card_suit;

    // (0)
    initialize_game();
    // (1) 
    deal_first_two_cards(&tmp_hole_card_rank, &tmp_hole_card_suit);
    // (2)
    if (dealer_hd.score != 21){
        game_player_deciding(&sp_hd);
    }
    // (3)
    game_ending_sequence(sp_hd, tmp_hole_card_rank,tmp_hole_card_suit);

    fadeout();
}

//===========================================(gamestate - Changing/Viewing Game Parameters)===========================================//
// changing the bet, viewing the count, changing the rules of the game
void display_game_info(uint16_t number){
    // the size of the deck is what is input already.
    init_bkg(48);
    set_bkg_tiles(3,4,12,9,bkg_gm_info);
    uint16_t temp;
    uint16_t temp_c;
    uint8_t digits = 1;
    int tmp_count;
    temp = ((number/52)+1);
    DrawNumberBkg(9,8, temp, 1);           //display remaining decks
    tmp_count = count;
    if(tmp_count < 0){
        tmp_count *= -1;
        set_bkg_tile_xy(10,5,13);           //D
        if (temp != numberOfDecks){
            set_bkg_tile_xy(10,11,13);           //D
        }
    }
    if (tmp_count > 9){
        digits = 2;
    }
    temp_c = (uint16_t)(tmp_count);
    DrawNumberBkg(11, 5,temp_c, digits);      //display running count

    //displaying true count
    if (temp != (uint16_t)numberOfDecks){
        temp /= tmp_count;
        DrawNumberBkg(11,11,temp, digits);
    }

}

void player_bet(){
    // uint8_t temp_y;
    UBYTE breakCond = 0;
    // HIDE_SPRITES;
    // uint16_t change_in_bet;
    int n;
    n = 0;
    human.cash -= human.bet;
    int position[] = {1, 4, 7, 10,13, 16};
    int position_2[] = {2, 5, 8, 11,14, 17};
    uint16_t value[] = {1, 5, 10, 25, 100,500};

    // set_bkg_tiles(0,0,20,25, bkg_tab_c);
    
    // set_bkg_tiles(12,12,8,5, bkg_chg_bet);
    
    set_win_tiles(1,2,17,3, chips);

    set_win_tiles(1,0,14,1, bkg_cash_bet);

    set_win_tile_xy(1,5,58);
    set_win_tile_xy(2,5,59);

    set_win_tile_xy(15,0,47);
    set_win_tile_xy(16,0,47);
    set_win_tile_xy(17,0,47);
    set_win_tile_xy(18,0,47);

    for(i = 0; i < 14;i++){
        scroll_bkg(0,4);
        scroll_win(0,-4);
        wait_vbl_done();
    }
    DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
    set_win_tile_xy(10,0,45);
    DrawNumber(15, 0, (uint16_t)human.bet,4);
    // set_win_tile_xy()

    while(breakCond == 0){
        waitpad(J_LEFT | J_RIGHT | J_A | J_B | J_UP);
        switch(joypad()){
            case J_LEFT:
                if (n > 0){
                    set_win_tile_xy(position[n], 5, 47);
                    set_win_tile_xy(position_2[n], 5, 47);
                    n -= 1;
                    set_win_tile_xy(position[n], 5, 58);
                    set_win_tile_xy(position_2[n], 5, 59);
                }
                waitpadup();
                break;
            case J_RIGHT:
                if (n < 5){
                    set_win_tile_xy(position[n], 5, 47);
                    set_win_tile_xy(position_2[n], 5, 47);
                    n += 1;
                    set_win_tile_xy(position[n], 5, 58);
                    set_win_tile_xy(position_2[n], 5, 59);
                }
                waitpadup();
                break;
            case J_A:
                if(human.bet < 2000){
                    human.bet += value[n];
                    human.cash -= value[n];
                    DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
                    set_win_tile_xy(10,0,45);
                    DrawNumber(15, 0, (uint16_t)human.bet,4);
                    performantdelay(6);
                }
                break;
            case J_B:
                if (human.bet > 5){
                    //      bet = 10,     chip is 25
                    if(((int)human.bet - (int)value[n]) >= 5){
                        // if (human.bet > 5){
                        human.bet -= value[n];
                        // if(human.bet < 5){human.bet = 5;}
                        //else{}
                        human.cash += value[n];
                    }else{
                        human.cash += (human.bet - 5);
                        human.bet = 5;
                    }
                    DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
                    set_win_tile_xy(10,0,45);
                    DrawNumber(15, 0, (uint16_t)human.bet,4);
                    performantdelay(6);
                }
                
                break;
            case J_UP:
                human.cash += human.bet;
                DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
                set_win_tile_xy(10,0,45);
                DrawNumber(15, 0, 0,4);
                breakCond = 1;
                break;
            default:
                break;
        }
        performantdelay(3);
        if(breakCond != 0){break;}
    }
    
    for(i = 0; i < 14;i++){
        scroll_bkg(0,-4);
        scroll_win(0,4);
        wait_vbl_done();
    }
    set_win_tile_xy(position[n], 5, 47);
    set_win_tile_xy(position_2[n], 5, 47);
}

void gamestate_restart(){
    //-----------------------------------(gs - parameters - START)-----------------------------------//
    uint8_t break_cond = 0;

    init_bkg(48);                                         //draws the blank "table" background to the screen
    set_bkg_data(0,99,bkg_tiles_opt);
    set_bkg_tiles(4,4,17,4, starting_amt);
    set_bkg_tiles(4,8,17,3, ending_amt);
    set_bkg_tiles(4,11,17,4, dwn_to_chg_bet);
    DrawNumberBkg(13,9,start_cash, 4);
    DrawNumberBkg(13,12, human.cash, 4);
    DrawNumberBkg(13,15,(uint16_t)human.bet, 4);
    HIDE_SPRITES;
    fadein();
    waitpad(J_A | J_B|J_DOWN|J_LEFT);

    if(!((joypad() & J_A) | (joypad() & J_B))){
        break_cond = 0;
        while(!(break_cond)){
            switch(joypad()){
                case J_A:
                case J_B:
                    break_cond = 1;
                    break;
                case J_DOWN:
                    waitpadup();
                    player_bet();
                    set_bkg_tiles(12,12,8,5, bkg_chg_bet);
                    DrawNumberBkg(15,14,(uint16_t)human.bet, 4);
                    break;
                case J_LEFT:
                    fadeout();
                    display_game_info(myDeck.size);
                    fadein();
                    waitpadup();
                    fadeout();
                    init_bkg(48);
                    set_bkg_tiles(12,12,8,5, bkg_chg_bet);
                    DrawNumberBkg(15,14,(uint16_t)human.bet, 4);
                    DrawNumber((uint8_t)4,0, (uint16_t)human.cash,6);
                    set_win_tile_xy(10,0,45);
                    DrawNumber(15, 0, 0,4);
                    fadein();
                    wait_vbl_done();
                    break;
            }
        }   
    }
    fadeout();
    SHOW_SPRITES;
    //-----------------------------------(gs - parameters - END)-----------------------------------//
}

void gamestate_options(){
    init_bkg(48);
    set_bkg_data(0,99,bkg_tiles_opt);
}

void initial_bet_screen(){
    set_bkg_data(0,99,bkg_tiles_opt);
    init_bkg(48);
    set_bkg_tiles(4,4,17,4, starting_amt);
    set_bkg_tiles(4,4,17,4, starting_amt);
}

void main(){
    printf("\n\n\n\n\n\n\t\t   Blackjack\n\n\n\n\n\n\t\t\tPRESS START");
    waitpad(J_START);
    fadeout();
    init_gfx();

    sprite_count = 0;
    shuffle(&myDeck, numberOfDecks);

    game_flags = 1;
    /***********
    0 - use the same bet
    **************/
    human.cash = 1500;
    human.bet = 25;
    human.half_dollar = false;
    hand_init(&human.hd, 32, 80, &sprite_count);
    hand_init(&dealer_hd, 32, 24, &sprite_count);           //dealer's starting x = 32, y = 24

    while(1){
        gamestate_running();
        gamestate_restart();
        reset_game_board();
        performantdelay(9);
    }
}

/******************************
issues noticed after implementation:
uncategorized:
    
tasks:
//=======================( Gameplay )=======================//
    >> splitting and then getting an ace and a 10 is considered 21, not blackjack, so change this.      #5

    >> Fix that garbage pixels for the upper left corner of the first card.                             #6

    >> play through the game and keep a personal tally of the count.                                    #7
        if it's off, then fix it
    >> implement true count display                                                                     #7.a

for now, focus on 4 decks:                                                                              #8
    >> so when we show the decks remaining, 
        >> show integer decks remaining for the first 2 decks, 
        >> last 2 decks, display half decks.
later:                                                                                                  #9
    >> implement 6 decks    
        >> save a separate external constant unsigned char array that contains 8 iterations of the card values
        >> in the card structures function, declare a global variable that contains a copy of all of this array's values
        >> in the shuffle function, the input deck should be returned with a size of 4 decks.
        >> when the 4 decks is found to be done with, continue to get the remaining decks after that. 
        >> essentially you'll never have an array in use larger than 4 decks, but it will still have
            values from all 8 decks in either scenario. so 2 shoes with 4 decks of the same 8 decks 
            will amount to 8 decks rather than 2 instances of 4 decks.
        
    >> Change the limit of cards in the player's hand to 14 rather than 7.                              #9.a

    >> implement basic strategy tracker                                                             #11

    >> implement deviations tracker                                                                 #12

    >> add sounds                                                                                   #13

    >> add color                                                                                    #14

//=======================( Restarting (i.e. Betting) )=======================//
    >> change betting limit to be however much the player has in their bankroll.                    #4

//=======================( Options )=======================//
    >> Implement options menu:                                                                      #10
        - decks: (1-8)
        - Bet Min:
        - Bet Max:
        - DD on: (9-10-11), (10-11), (Any)
        - Show Hand Value
        - Dealer hits on soft 17
        - DD after split
        - Insurance
        - Surrenders

//=======================( Extras )=======================//
notes about true count:
    >> generally if your true count division results in a floating point number, round up
    >> if you are playing with 1-2 decks, count by quarter decks
*******************************/