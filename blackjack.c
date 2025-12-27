#include <furi.h>
#include <notification/notification_messages.h>
#include "blackjack_icons.h"
#include "src/util/scene.h"
#include "src/game_state.h"
#include "src/util/helpers.h"
#include "src/scene_setup.h"
#include "src/settings.h"
#include "src/graphics/asset.h"
#include "src/graphics/render.h"

static FuriMutex *update_mutex;
static const Icon *suits[4] = {&I_hearths, &I_spades, &I_diamonds, &I_clubs};
static const Icon *pattern = &I_pattern_big;
static const Icon *letters[13] = {
    &I_2,
    &I_3,
    &I_4,
    &I_5,
    &I_6,
    &I_7,
    &I_8,
    &I_9,
    &I_10,
    &I_J,
    &I_Q,
    &I_K,
    &I_A
};

static void gui_input_events_callback(const void *value, void *ctx) {
    furi_mutex_acquire(update_mutex, FuriWaitForever);
    GameState *instance = ctx;
    const InputEvent *event = value;

    if (event->key == InputKeyBack && event->type == InputTypeLong) {
        instance->exit = true;
    }

    input_scene(instance, event->key, event->type);

    furi_mutex_release(update_mutex);
}

GameState *prepare() {
    GameState *gameState = malloc(sizeof(GameState));
    update_mutex = (FuriMutex *) furi_mutex_alloc(FuriMutexTypeNormal);

    set_cull_mode(CULL_NONE);

    //set main data
    gameState->exit = false;
    gameState->sound_enabled = true;
    gameState->input = furi_record_open(RECORD_INPUT_EVENTS);
    gameState->gui = furi_record_open(RECORD_GUI);
    gameState->canvas = gui_direct_draw_acquire(gameState->gui);
    gameState->input_subscription = furi_pubsub_subscribe(gameState->input, gui_input_events_callback, gameState);

    // card graphics
    card_load_suit_assets(suits);
    card_load_letter_assets(letters);
    card_load_background(pattern);


    //set game data
    gameState->state = malloc(sizeof(PlayData));
    gameState->state->dealer = list_make();
    gameState->state->deck = list_make();

    for (uint8_t i = 0; i < 4; i++)
        gameState->state->hand[i] = list_make();
    gameState->state->balance = INITIAL_BALANCE;
    gameState->state->balance_mul = 0;
    gameState->state->bet = INITIAL_BET;

    load_settings(gameState);

    //set scenes
    prepare_scenes(gameState->canvas);
    add_scene(&main_screen);    //0
    add_scene(&bet_screen);     //1
    add_scene(&intro_screen);   //2
    add_scene(&play_screen);    //3
    add_scene(&split_screen);   //4
    add_scene(&dealer_screen);  //5
    add_scene(&result_screen);  //6

    //Load main image
    set_scene(gameState, 0);

    return gameState;
}

static void direct_draw_run(GameState *instance) {
    if (!check_pointer(instance)) return;
    furi_thread_set_current_priority(FuriThreadPriorityIdle);

    do {
        FuriStatus status = furi_mutex_acquire(update_mutex, 20);
        if (!status) continue;
//        FURI_LOG_W("STATE", "UPDATE");
        update_scene(instance);

        render_scene(instance);

        furi_mutex_release(update_mutex);
        furi_thread_yield();

    } while (!instance->exit);
}

static void cleanup(GameState *instance) {
    furi_pubsub_unsubscribe(instance->input, instance->input_subscription);

    //free game data

    list_free(instance->state->deck);
    list_free(instance->state->dealer);
    for (uint8_t i = 0; i < 4; i++)
        list_free(instance->state->hand[i]);

    free(instance->state);

    //free the rest
    instance->canvas = NULL;
    gui_direct_draw_release(instance->gui);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_INPUT_EVENTS);
    free_scenes();
    asset_cleanup();

    free(instance);
    furi_mutex_free(update_mutex);
}

int32_t blackjack_app(void *p) {
    UNUSED(p);
    CHECK_HEAP();
    GameState *instance = prepare();
    direct_draw_run(instance);

    cleanup(instance);
    check_leak();
    CHECK_HEAP();
    return 0;
}