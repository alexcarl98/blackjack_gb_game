#ifndef __main_game_h_INCLUDE
#define __main_game_h_INCLUDE
#include <gb/gb.h>
#include "card_structures.c"


void init_gfx();
void play_game(deck *myDeck, player *human, player *bot, UBYTE *game_flags);
void change_bet(player *human);                             // will be called depending on whether a gamesflag is set, 
void show_score(hand *self);
void first_deal(deck *myDeck, player *human, player *bot);  //displays the dealer cards and the first two cards of the player
void insurance(player *p);
void display_game_info(uint16_t number);
void init_split_hand_gfx();
void init_split_hand(deck *mydeck, player *human, hand *split_hand, uint8_t *sprite_count);
void player_choices(deck *myDeck, player *human, hand *split_hand, uint8_t *sprite_count);

#endif