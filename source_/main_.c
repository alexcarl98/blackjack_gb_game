#include <gb/gb.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <rand.h>

#include "..\res_\bkg_tiles.h"
#include "..\res_\first_card.h"
#include "..\res_\next_card.h"
#include "..\res_\hole_card.h"
#include "..\res_\player_options.h"
#include "..\res_\table.h"
#include "..\res_\cash_window.h"

#include "..\res\card_sprites.h"
#include "card_structures.c"
#include "common.h"


#include "main_game.h"


uint8_t numberOfDecks = 4;
uint16_t numberOfGames = 0;
uint8_t i,j,k;




int main(){
    printf("\n\n\n\n\n\n\t\t   Blackjack\n\n\n\n\n\n\t\t\tPRESS START");
    waitpad(J_START);
    fadeout();
    init_gfx();    

    uint8_t sprite_count = 0;
    deck mdeck;
    shuffle(&mdeck, numberOfDecks);
    player bot;
    player human;
    UBYTE game_flags;
    game_flags = 0x0;
    bot.cash = 0;
    bot.bet = 0;
    bot.half_dollar = false;
    human.cash = 1500;
    human.bet = 25;
    human.half_dollar = false;
    hand_init(&human.hd, 32, 80, &sprite_count);
    hand_init(&bot.hd, 32, 24, &sprite_count);           //dealer's starting x = 32, y = 24

    while(1){
        play_game(&mdeck, &human, &bot, &game_flags);
        performantdelay(9);
    }

    return 0;
}