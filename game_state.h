#pragma once


#include <gui/canvas.h>
#include <gui/gui.h>
#include <core/pubsub.h>
#include <notification/notification.h>
#include "util/scene.h"
#include "util/card.h"

/*
 * RULES
 *
 * Lose: full bet lost
 * Win: wins bet + bet
 * Blackjack: 1.5x win of the bet + bet
 * Push (draw): gets back the bet
 *
 * if anyone has blackjack (21 from 2 cards), it is the winner
 * even if others have the same amount with mire cards
 *
 * Stand: next turn without adding bet
 * Hit: Get one more card
 * Bust: when you over 21 after hit
 *
 * dealer:
 * must hit if hand is lower than 17
 * must hit with soft 17 (17 with ace) is depending on the casino
 *
 * Insurance
 * if dealers has a face up card of Ace, player can get insurance before any other action.
 * Insurance costs 0.5X of your bet.
 * Dealer checks the hand after insurance taken.
 * Dealer has blackjack: you lose your bet, but get back 2x your insurance (in this case you are even)
 * Dealer doesn't have blackjack: you lose your insurance amount, game continues
 *
 *
 * Split:
 * Only can split if you have 2 cards in that hand and with the same value (even 10 and K)
 * After split, the dealer gives a new card to both hands.
 * Now if you got another pair in one or both hands, you can split again. (but only this time, max split is 3)
 * Each hand dealt separately.
 * In short, allow split menu when you have 2 cards, same value, and have less than 4 hands.
 *
 * [IMPORTANT] if you split aces, you can not do anything else with that hand, unless you get another ace, which allows to split again.
 *
 * No blackjack bonus for split hands, but still wins against hands with more than 2 cards 21. If dealer has blackjack, it is a push (draw).
 *
 * If you split, you add your initial bet to the pot for that hand
 *
 *
 * Doubling:
 * only get one more card. Only allowed for the first 2 cards, not allowed after you split
 *
 */

#define INITIAL_BET 10
#define INITIAL_BALANCE 1000

//to avoid overflow, after reached increase the balance multiplier and set balance to the overflown value
//1.000.000.000
#define MAX_SCORE 1000000000
#define MAX_BET 200

typedef struct {
    Card *card;
    Vector start;
    Vector target;
    Vector current;
    float t;
} AnimatedCard;

typedef struct {
    List *deck;
    List *dealer;
    List *hand[4];
    uint8_t hand_count;
    uint8_t current_hand;

    bool doubled[4];
    bool insurance;
    /*
     * only the initial bet
     * for next round, keep it as initial bet
     * double and split calculated on the fly
     * double: 2 * bet
     * split: hand_count * bet
     */
    uint8_t bet;    //store

    //subtract from balance after betting/splitting/doubling
    uint32_t balance; //store
    uint8_t balance_mul; //store (to allow way bigger hands)

} PlayData;

typedef struct {
    // Main data
    Canvas *canvas;
    Gui *gui;
    FuriPubSub *input;
    FuriPubSubSubscription *input_subscription;
    NotificationApp *notification_app;
    bool exit;

    bool sound_enabled; //store, on main menu display a speaker image, left arrow toggles it

    // Game data
    PlayData *state;

} GameState;