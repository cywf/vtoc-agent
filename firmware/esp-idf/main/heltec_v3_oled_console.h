#pragma once

#include <stdbool.h>

#include "esp_err.h"

// Starts the local OLED console. It is intentionally local-only: all displayed
// values come from the already established bring-up state.
esp_err_t huginn_heltec_v3_oled_console_start(const char *image_role, bool self_test_passed);
