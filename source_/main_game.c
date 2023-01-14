#include <gb\gb.h>
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


void init_gfx(){
    set_bkg_data(0,121,bkg_tiles);                  //load in the background tiles (also used in window layer)
    init_bkg(48);                                         //draws the blank "table" background to the screen
    set_sprite_data(0,115,card_tiles);                  //loads in the tiles used for the cards. 
    //used to displayer player's cards
    set_bkg_tiles(3, 8, 1, 5, bkg_first_card);       //loads in the "1st" card
    set_bkg_tiles(4, 8, 4, 5, bkg_next_card);             //loads in the whole card background for 
    
    //used to display dealer's cards
    set_bkg_tiles(3, 8, 1, 5, hole);
    set_bkg_tiles(4, 1, 4, 5, bkg_next_card);
    
    //displays the hit or stand options
    set_bkg_tiles(15, 1, 5, 5, bkg_player_options);
    set_win_tiles(1,0,14,1, bkg_cash_bet);              //displays the cash and bet stuff
    move_win(7,136);                                    //offset from tile grid by a single pixel
    set_win_tile_xy(10,0,45);                           // .00

    SHOW_BKG;
    SHOW_SPRITES;
    SHOW_WIN;
    DISPLAY_ON;
}

void play_game(deck *myDeck, player *human, player *bot, UBYTE *game_flags){
    /*******
     * You're going to write code up to the point of displaying the initial graphics, then you'll compile it
     * with a new main function
     * 
    */
   fadein();


}

