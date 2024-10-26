#include "scene_setup.h"
#include "scene/main_scene.h"
#include "scene/bet_scene.h"
#include "scene/intro_scene.h"
#include "scene/play_scene.h"
#include "scene/split_screen.h"

/*
 * Loop:
 *            ___________________________________
 *           |                                  |
 *          \ /                                 |
 *  Main -> Bet -> Intro -> Play -> Dealer -> Result
 *                           / \
 *                            |
 *                           \ /
*                           Split
 */

Scene main_screen = (Scene) {
    .name="MAIN",
    .input=&main_input,
    .start=&main_start,  //load settings/score
    .update=&main_update,
    .render_ui=&main_render_ui,
    .render=&main_render,
};

Scene bet_screen = (Scene) {
    .name="BET",
    .input=&bet_input,
    .start=&bet_start,
    .update=NULL,
    .render_ui=&bet_render_ui,
    .render=&bet_render,
};

Scene intro_screen = (Scene) {
    .name="INTRO",
    .start=intro_start,
    .render=intro_render,
    .render_ui=intro_render_ui,
    .update=intro_update,
    .input=intro_input,
};

Scene play_screen = (Scene) {
    .name="PLAY",
    .start=play_start,
    .render=play_render,
    .render_ui=play_render_ui,
    .update=play_update,
    .input=play_input,
};

Scene split_screen = (Scene) {
    .name="SPLIT",
    .input=&split_input,
    .start=&split_start,
    .update=&split_update,
    .render_ui=NULL,
    .render=&split_render,
};

Scene dealer_screen = (Scene) {
    .name="DEALER",
    .input=NULL,
    .start=NULL,
    .update=NULL,
    .render_ui=NULL,
    .render=NULL,
};

Scene result_screen = (Scene) {
    .name="RESULT",
    .input=NULL,
    .start=NULL,
    .update=NULL,
    .render_ui=NULL,
    .render=NULL,
};
