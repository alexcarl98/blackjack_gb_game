#include <gb/gb.h>
#include <stdio.h>
#include <stdbool.h>
int count = 0;

void count_card(UBYTE hex_card){
    int temp_val;
    int *count_ptr = &count;
    temp_val = (int)(hex_card & 0xF);
    if ((temp_val >= 10) || (temp_val == 1)){
        *count_ptr -= 1;
    }else if ((1 < temp_val) && (temp_val < 7)){
        *count_ptr += 1;
    }
}

//================================( CARD )================================//
typedef struct CardSprite{
    UBYTE spr_id[2];
    UBYTE suit_rank;
    uint8_t x;
    uint8_t y;
}card;

void card_init(card * self, UBYTE s_r, uint8_t x, uint8_t y);

void card_init(card * self, UBYTE s_r, uint8_t x, uint8_t y){
    UBYTE r;
    UBYTE s;
    UBYTE suit_isolater;
    UBYTE rank_isolater;

    suit_isolater = 0xF0;             // %1111_0000
    rank_isolater = 0xF;              // %0000_1111
    s = (s_r & suit_isolater);        //suit(in HL)= Suit-Rank anded with suit isolater
    r = (s_r & rank_isolater);        //rank = Suit-Rank anded with rank isolater
    
    self->suit_rank = s_r;
    self->x = x;                             //x position
    self->y = y;                             //y position

    // set_sprite_tile(*sprite_count, (int)r);
    // self->spr_id[0] = (UBYTE)(*sprite_count);
    // *sprite_count = (*sprite_count) + 1;        //  THIS is how you increment pointers

    // set_sprite_tile(*sprite_count, (int)s);
    // self->spr_id[1] = (UBYTE)(*sprite_count);
    // *sprite_count = (*sprite_count) + 1;
}
//================================( DECK )================================//

typedef struct Deck{
    uint16_t size;
    UBYTE *cards;
}deck;

void shuffle(deck *playing_deck, uint8_t n);

void shuffle(deck * playing_deck, uint8_t n){
    // The below hex values are used to represent a card
    // the values are out of order, but there's about 4 decks in these cards
    //The leftmost hex value determines the suit and the right hex value determine the card rank
    // Suits: 0x10 - spades, 0x20 - clubs, 0x30 - hearts, 0x40 - diamonds
    // Ranks: 0x01->0xA - Ace->10 , 0xB - Jack, 0xC - Queen, 0xD - King
    // so 0x11 is the ace of spades
    UBYTE card_values[] = {
    0x11, 0x2B, 0x13, 0x45, 0x26, 
    0x14, 0x15,0x38, 0x17,  0x19, 
    0x1A, 0x1B,0x43, 0x1C,  0x21, 
    0x22, 0x23, 0x37,0x27, 0x4D,
    0x3D, 0x28, 0x16, 0x29, 0x12,
    0x2A, 0x2D, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x24, 0x36, 0x18,
    0x39, 0x25, 0x1D, 0x3A, 0x3B,
    0x2C, 0x3C, 0x41, 0x42, 0x44,
    0x46,0x47, 0x48, 0x49, 0x4A,
    0x4B,0x4C,
    0x11, 0x2B, 0x13, 0x45, 0x26, 
    0x14, 0x15,0x38, 0x17,  0x19, 
    0x1A, 0x1B,0x43, 0x1C,  0x21, 
    0x22, 0x23, 0x37,0x27, 0x4D,
    0x3D, 0x28, 0x16, 0x29, 0x12,
    0x2A, 0x2D, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x24, 0x36, 0x18,
    0x39, 0x25, 0x1D, 0x3A, 0x3B,
    0x2C, 0x3C, 0x41, 0x42, 0x44,
    0x46,0x47, 0x48, 0x49, 0x4A,
    0x4B,0x4C,
    0x11, 0x2B, 0x13, 0x45, 0x26, 
    0x14, 0x15,0x38, 0x17,  0x19, 
    0x1A, 0x1B,0x43, 0x1C,  0x21, 
    0x22, 0x23, 0x37,0x27, 0x4D,
    0x3D, 0x28, 0x16, 0x29, 0x12,
    0x2A, 0x2D, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x24, 0x36, 0x18,
    0x39, 0x25, 0x1D, 0x3A, 0x3B,
    0x2C, 0x3C, 0x41, 0x42, 0x44,
    0x46,0x47, 0x48, 0x49, 0x4A,
    0x4B,0x4C,
    0x11, 0x2B, 0x13, 0x45, 0x26, 
    0x14, 0x15,0x38, 0x17,  0x19, 
    0x1A, 0x1B,0x43, 0x1C,  0x21, 
    0x22, 0x23, 0x37,0x27, 0x4D,
    0x3D, 0x28, 0x16, 0x29, 0x12,
    0x2A, 0x2D, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x24, 0x36, 0x18,
    0x39, 0x25, 0x1D, 0x3A, 0x3B,
    0x2C, 0x3C, 0x41, 0x42, 0x44,
    0x46,0x47, 0x48, 0x49, 0x4A,
    0x4B,0x4C,
    0x11, 0x2B, 0x13, 0x45, 0x26, 
    0x14, 0x15,0x38, 0x17,  0x19, 
    0x1A, 0x1B,0x43, 0x1C,  0x21, 
    0x22, 0x23, 0x37,0x27, 0x4D,
    0x3D, 0x28, 0x16, 0x29, 0x12,
    0x2A, 0x2D, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x24, 0x36, 0x18,
    0x39, 0x25, 0x1D, 0x3A, 0x3B,
    0x2C, 0x3C, 0x41, 0x42, 0x44,
    0x46,0x47, 0x48, 0x49, 0x4A,
    0x4B,0x4C,
    0x11, 0x2B, 0x13, 0x45, 0x26, 
    0x14, 0x15,0x38, 0x17,  0x19, 
    0x1A, 0x1B,0x43, 0x1C,  0x21, 
    0x22, 0x23, 0x37,0x27, 0x4D,
    0x3D, 0x28, 0x16, 0x29, 0x12,
    0x2A, 0x2D, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x24, 0x36, 0x18,
    0x39, 0x25, 0x1D, 0x3A, 0x3B,
    0x2C, 0x3C, 0x41, 0x42, 0x44,
    0x46,0x47, 0x48, 0x49, 0x4A,
    0x4B,0x4C
    };
    //I want to change this eventually but for now..
    if (n > 4){n = 4;}
    uint16_t i,t;
    // playing_deck->size = 208;
    playing_deck->size = 52 * n;
    playing_deck->cards = (UBYTE*)malloc(playing_deck->size * sizeof(UBYTE));
    
    uint16_t seed = LY_REG;
    seed |= (uint16_t)DIV_REG << 8;
    initarand(seed);
    
    for(i = 0;i < (playing_deck->size); i++){
        do{
            t = ((uint8_t)rand() % (uint8_t)playing_deck->size);
        }while (card_values[t]== 0x0);
        playing_deck->cards[i] = card_values[t];
        card_values[t] = 0x0;
    }
}

//================================( HAND )================================//
typedef struct Hand{
    card cards[7];
    UBYTE card_reps[8];
    UBYTE design_spr_id[6];     // sprite id's for player's upcard design
    UBYTE score;
    UBYTE size;
    uint8_t h_x;
    uint8_t h_y;
    UBYTE flags;                //see below for the flags
}hand;
/****************** UBYTE flags ******************
flags for the hand:
0 - blackjack       -- bit 0 determines whether the hand has blackjack   
1 - insurance       -- bit 1 determines whether the player has accepted insurance
2 - soft            -- bit 2 determines whehter the player's hand is soft
3 - surrendered     -- bit 3 determines whether the player surrendered
_
4 - dd_hand         -- bit 4 determines whether the player can double down
5 - sp_hand         -- bit 5 determines whtehr the player can split
6 - dd_accepted     -- determines whether the player doubled down
7 - split_accepted  -- determines whether the player split
**************************************************/

void hand_init(hand *self, uint8_t start_x, uint8_t start_y, uint8_t *sprite_count);
// void display_card_1(UBYTE ID_rank, UBYTE ID_suit, uint8_t x, uint8_t y);
// void display_hand(hand *self);
void recieve_card(hand *self, deck *d, uint8_t n);
void calculate_score(hand * self);
void display_last_card(hand *h);

void hand_init(hand *self, uint8_t start_x, uint8_t start_y, uint8_t *sprite_count){
    // pretty much initializes all values of a hand to 0
    uint8_t i = 0;      //new
    self->h_x = start_x;
    self->h_y = start_y;
    self->score = 0;
    self->size = 0;
    self->flags = 0x0;

    for(i = 0; i < 8; i++){self->card_reps[i] = 0;}     //set card_representation array to 0's

    //designate sprite tiles for the displaying card
    for(i = 0; i < 6; i++){
        set_sprite_tile(*sprite_count, 0);
        self->design_spr_id[i] = (UBYTE)(*sprite_count);
        *sprite_count += 1;
    }
}

// void display_card_1(UBYTE ID_rank, UBYTE ID_suit, uint8_t x, uint8_t y){
//     move_sprite(ID_rank, x, y);     //rank  !1
//     move_sprite(ID_suit, x, y+8);   // suit !H
// }

// void display_hand(hand *self){
//     uint8_t i;
//     uint8_t temp_x = 0;
//     // for(i = 0; i < self->size; i++){
//     //     display_card_1(self->cards[i].spr_id[0], 
//     //                     self->cards[i].spr_id[1],
//     //                     self->h_x + temp_x, self->h_y);
//     //     temp_x += 8;
//     // }
//     display_last_card(&(*self));
// }



void recieve_card(hand *self, deck *d, uint8_t n){
    //I will not be forgetting this any time soon
    // don't forget about pointer manipulation
    uint8_t i;
    UBYTE tmp;
    uint8_t tmp_x;
    tmp_x = self->h_x + ((self->size) * 8);
    for(i = 0; i < n; i++){
        // tmp = d->card_rep[d->size];
        tmp = d->cards[(d->size - 1)];
        count_card(tmp);
        self->card_reps[self->size] = tmp;
        d->size--;
        card_init(&(self->cards[(int)self->size]), tmp, tmp_x, self->h_y);
        self->size += 1;
        tmp_x += 8;
    }
    //function will re-evaluate the score
    calculate_score(self);
}

void calculate_score(hand * self){
    uint8_t num_of_aces = 0;
    uint8_t tmp_score = 0;
    uint8_t evaluation_constant = 21;
    uint8_t i;
    UBYTE temp;
    for (i = 0; i < self->size; i++){
        temp = self->card_reps[i] & 0xF;                        // Isolate the value of the card by anding the hex representation with 0xF
        // if its an ace, keep track of the number of aces
        if (temp == 1){num_of_aces += 1;}
        // if it's 11, 12, 13, its value is 10
        else if (temp > 10){tmp_score += 10;}
        //otherwise, add it's value
        else{tmp_score += temp;}
    }
    if (num_of_aces > 0){
        evaluation_constant -= (10 + num_of_aces);
        if (tmp_score <= evaluation_constant){
            self->flags |= 0b100;          // set soft bit in h_flags to 1
            tmp_score += (10 + num_of_aces);
            if (self->size == 2 && tmp_score == 21){self->flags |= 0b1;} // set blackjack flag to 1
            /*  This might be redundant     */
            else{self->flags &= 0b11111110;}     // set blackjack flag to 0
        }else{
            self->flags &= 0b11111011;         // set soft flag to 0
            tmp_score += num_of_aces;
            self->flags &= 0b11111110;     // set blackjack flag to 0  [this may be redundant]
        }
    }
    if (self->size == 2){
        if ((9 <= tmp_score) && (tmp_score <=11)){self->flags |= 0b10000;}     //set dd_hand flag to 1
        else{self->flags &= 0b11101111;}  //set dd_hand to 0
        if (((self->card_reps[0] & 0xF) == (self->card_reps[1] & 0xF)) |
            (((self->card_reps[0] & 0xF) >= 0xA) & ((self->card_reps[1] & 0xF) >= 0xA))){
                self->flags |= 0b100000;         // set split flag to 1
        }
    }else{
        self->flags &= 0b11011111;;         // set split flag to 0
    }
    self->score = tmp_score;
}

void display_last_card(hand *h){
    UBYTE tmp_rank;
    uint8_t i;
    UBYTE tmp_suit;

    uint8_t tmp_x = (h->cards[h->size - 1]).x+8;     // one tile right of the card sprite
    uint8_t tmp_y = (h->cards[h->size - 1]).y + 8;    //1 tile down from the card sprite 
    UBYTE s_r = h->cards[h->size - 1].suit_rank;

    UBYTE design_tiles[6] = {0x0,0x0,0x0,0x0,0x0,0x0};

    tmp_rank = (s_r & 0xF);
    tmp_suit = (s_r & 0xF0);

    if (1 <= tmp_rank && tmp_rank <= 10){
        if (tmp_rank % 2 !=0){
            design_tiles[2] = 0x1;
            design_tiles[3] = 0x2;
        }
        if (tmp_rank > 1){
            design_tiles[1] += 0x3;
            design_tiles[4] += 0x3;
        }
        if(tmp_rank > 3){
            design_tiles[0] += 0x3;
            design_tiles[5] += 0x3;
        }
        if(tmp_rank >5){
            design_tiles[2] += 0x3;
            design_tiles[3] += 0x3;
        }
        if(tmp_rank == 8){
            design_tiles[0] = 0x3; design_tiles[1] = 0x3;
            design_tiles[2] = 0x4; design_tiles[3] = 0x5;
            design_tiles[4] = 0x4; design_tiles[5] = 0x5;
        }
        if(tmp_rank == 9){
            design_tiles[0] = 0x4; design_tiles[1] = 0x5;
            design_tiles[2] = 0x4; design_tiles[3] = 0x5;
            design_tiles[4] = 0x4; design_tiles[5] = 0x5;
        }
        if(tmp_rank == 0xA){
            design_tiles[0] = 0x4; design_tiles[1] = 0x5;
            design_tiles[2] = 0x6; design_tiles[3] = 0x7;
            design_tiles[4] = 0x4; design_tiles[5] = 0x5;
        }
        for(i = 0; i < 6; i++){
            if (design_tiles[i] != 0x0){
                design_tiles[i] += tmp_suit;
            }
        }
        for(i = 0; i<6; i+=2){
            //place 2 tiles each iteration
            set_sprite_tile(h->design_spr_id[i], design_tiles[i]);
            set_sprite_tile(h->design_spr_id[i+1], design_tiles[i+1]);
            move_sprite(h->design_spr_id[i], tmp_x, tmp_y);
            move_sprite(h->design_spr_id[i+1], tmp_x+8, tmp_y);
            tmp_y +=8;
        }
    }
    else{
        if(tmp_rank == 11){
            design_tiles[0] = 0x50; design_tiles[1] = 0x51;
            design_tiles[2] = 0x52; design_tiles[3] = 0x53;
            design_tiles[4] = 0x54; design_tiles[5] = 0x55;
        }
        if(tmp_rank == 12){
            design_tiles[0] = 0x56; design_tiles[1] = 0x57;
            design_tiles[2] = 0x58; design_tiles[3] = 0x59;
            design_tiles[4] = 0x5A; design_tiles[5] = 0x5B;
        }
        if(tmp_rank == 13){
            design_tiles[0] = 0x5C; design_tiles[1] = 0x5D;
            design_tiles[2] = 0x5E; design_tiles[3] = 0x5F;
            design_tiles[4] = 0x60; design_tiles[5] = 0x61;

        }
        for(i = 0; i<6; i+=2){
            //place 2 tiles each iteration
            set_sprite_tile(h->design_spr_id[i], design_tiles[i]);
            set_sprite_tile(h->design_spr_id[i+1], design_tiles[i+1]);
            move_sprite(h->design_spr_id[i], tmp_x, tmp_y);
            move_sprite(h->design_spr_id[i+1], tmp_x+8, tmp_y);
            tmp_y +=8;
        }
    }
}

//================================( PLAYER )================================//
/*---------------------------------------------------------------------------*
before implementing this code 100% remember a few things:
>> You should still to fix the initialization issue with the hand (i.e. need to 
pass the sprite counter by reference into to the hand initialization function)
*---------------------------------------------------------------------------*/

typedef struct Player{
    uint16_t cash;
    uint16_t bet;
    bool half_dollar;
    hand hd;
}player;