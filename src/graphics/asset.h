#pragma once
#include <gui/icon.h>
#include "buffer.h"

Buffer* asset_load_icon(const Icon *icon);
Buffer* asset_get_icon(const Icon *icon);
void asset_cleanup();