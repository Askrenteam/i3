/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3bar - an xcb-based status- and ws-bar for i3
 * © 2010 Axel Wagner and contributors (see also: LICENSE)
 *
 * mode.c: Handle mode event and show current binding mode in the bar
 *
 */
#include "common.h"

#include <stdlib.h>
#include <string.h>

#include <yajl/yajl_parse.h>

/* A datatype to pass through the callbacks to save the state */
struct window_json_params {
    char *json;
    char *cur_key;
    char *name;
    bar_title_t *bar_title;
};

/*
 * Parse a string (change)
 *
 */
static int window_string_cb(void *params_, const unsigned char *val, size_t len) {
    struct window_json_params *params = (struct window_json_params *)params_;

    if (!strcmp(params->cur_key, "name")) {
        sasprintf(&(params->name), "%.*s", len, val);
        FREE(params->cur_key);
        return 1;
    }

    FREE(params->cur_key);
    return 0;
}

/*
 * Parse a boolean.
 *
 */
static int window_boolean_cb(void *params_, int val) {
    return 0;
}

/*
 * Parse a key.
 *
 * Essentially we just save it in the parsing state
 *
 */
static int window_map_key_cb(void *params_, const unsigned char *keyVal, size_t keyLen) {
    struct window_json_params *params = (struct window_json_params *)params_;
    FREE(params->cur_key);
    sasprintf(&(params->cur_key), "%.*s", keyLen, keyVal);
    return 1;
}

static int window_end_map_cb(void *params_) {
    struct window_json_params *params = (struct window_json_params *)params_;

    /* Save the name */
    params->bar_title->name = i3string_from_utf8(params->name);
    /* Save its rendered width */
    params->bar_title->name_width = predict_text_width(params->bar_title->name);

    DLOG("Got window change: %s\n", i3string_as_utf8(params->bar_title->name));
    FREE(params->cur_key);

    return 1;
}

/* A datastructure to pass all these callbacks to yajl */
static yajl_callbacks window_callbacks = {
    .yajl_string = window_string_cb,
    .yajl_boolean = window_boolean_cb,
    .yajl_map_key = window_map_key_cb,
    .yajl_end_map = window_end_map_cb,
};

/*
 * Start parsing the received JSON string
 *
 */
void parse_window_json(char *json) {
    /* FIXME: Fasciliate stream processing, i.e. allow starting to interpret
     * JSON in chunks */
    struct window_json_params params;

    bar_title bar_title;

    params.cur_key = NULL;
    params.json = json;
    params.bar_title = &bar_title;

    yajl_handle handle;
    yajl_status state;

    handle = yajl_alloc(&window_callbacks, NULL, (void *)&params);

    state = yajl_parse(handle, (const unsigned char *)json, strlen(json));

    /* FIXME: Proper error handling for JSON parsing */
    switch (state) {
        case yajl_status_ok:
            break;
        case yajl_status_client_canceled:
        case yajl_status_error:
            ELOG("Could not parse window event!\n");
            exit(EXIT_FAILURE);
            break;
    }

    /* We don't want to indicate default binding mode */
//    if (strcmp("default", i3string_as_utf8(params.mode->name)) == 0)
//        I3STRING_FREE(params.mode->name);

//    /* Set the new binding mode */
//    set_current_mode(&binding);
//

    set_bar_name(bar_title.name)
    yajl_free(handle);

    FREE(params.cur_key);
}
