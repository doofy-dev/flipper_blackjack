#include "asset.h"
#include "../util/list.h"
#include "../util/helpers.h"

typedef struct {
    const Icon *address;
    Buffer *buffer;
} AssetIcon;

static List *icons = NULL;


Buffer *asset_load_icon(const Icon *icon) {
    if (icons == NULL) icons = list_make();

    Buffer *data = buffer_decompress_icon(icon);
    AssetIcon *asset_icon = allocate(sizeof(AssetIcon));
    asset_icon->address = icon;
    asset_icon->buffer = data;
    list_push_back(asset_icon, icons);
    return data;
}

Buffer *asset_get_icon(const Icon *icon) {
    if (icons != NULL) {
        FOREACH(item, icons) {
            AssetIcon *asset_icon = item->data;
            if (asset_icon->address == icon) {
                return asset_icon->buffer;
            }
        }
    }
    return asset_load_icon(icon);
}

void asset_cleanup() {
    if (icons == NULL) return;
    ListItem *start = icons->head;
    while (start) {
        ListItem *next = start->next;
        AssetIcon *icon = start->data;
        release(icon->buffer->data);
        release(icon->buffer);
        release(start->data);
        release(start);
        start = next;
    }
    release(icons);
}