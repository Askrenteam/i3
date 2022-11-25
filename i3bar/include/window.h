/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3bar - an xcb-based status- and ws-bar for i3
 * © 2010 Axel Wagner and contributors (see also: LICENSE)
 *
 * mode.c: Handle "window" event and show current window name in the bar
 *
 */
#pragma once

#include <config.h>

#include <xcb/xproto.h>

#include "common.h"

/* Name of current binding mode and its render width */
struct bar_title {
    i3String *name;
    int name_width;
};

typedef struct bar_title bar_title;

/*
 * Start parsing the received JSON string
 *
 */
void parse_window_json(char *json);
