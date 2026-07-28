#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "heltec_v3_oled_console.h"

static const char *TAG = "heltec-v3-oled";

enum {
    OLED_SDA = 17,
    OLED_SCL = 18,
    OLED_RESET = 21,
    OLED_POWER = 36,
    OLED_WIDTH = 128,
    OLED_PAGES = 8,
    OLED_FIRST_ADDRESS = 0x08,
    OLED_LAST_ADDRESS = 0x77,
};

static const char FONT_CHARS[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-/";
static const uint8_t FONT[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, {0x7e, 0x11, 0x11, 0x11, 0x7e},
    {0x7f, 0x49, 0x49, 0x49, 0x36}, {0x3e, 0x41, 0x41, 0x41, 0x22},
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, {0x7f, 0x49, 0x49, 0x49, 0x41},
    {0x7f, 0x09, 0x09, 0x09, 0x01}, {0x3e, 0x41, 0x49, 0x49, 0x7a},
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, {0x00, 0x41, 0x7f, 0x41, 0x00},
    {0x20, 0x40, 0x41, 0x3f, 0x01}, {0x7f, 0x08, 0x14, 0x22, 0x41},
    {0x7f, 0x40, 0x40, 0x40, 0x40}, {0x7f, 0x02, 0x0c, 0x02, 0x7f},
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, {0x3e, 0x41, 0x41, 0x41, 0x3e},
    {0x7f, 0x09, 0x09, 0x09, 0x06}, {0x3e, 0x41, 0x51, 0x21, 0x5e},
    {0x7f, 0x09, 0x19, 0x29, 0x46}, {0x46, 0x49, 0x49, 0x49, 0x31},
    {0x01, 0x01, 0x7f, 0x01, 0x01}, {0x3f, 0x40, 0x40, 0x40, 0x3f},
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, {0x7f, 0x20, 0x18, 0x20, 0x7f},
    {0x63, 0x14, 0x08, 0x14, 0x63}, {0x03, 0x04, 0x78, 0x04, 0x03},
    {0x61, 0x51, 0x49, 0x45, 0x43}, {0x3e, 0x45, 0x49, 0x51, 0x3e},
    {0x00, 0x42, 0x7f, 0x40, 0x00}, {0x42, 0x61, 0x51, 0x49, 0x46},
    {0x21, 0x41, 0x45, 0x4b, 0x31}, {0x18, 0x14, 0x12, 0x7f, 0x10},
    {0x27, 0x45, 0x45, 0x45, 0x39}, {0x3c, 0x4a, 0x49, 0x49, 0x30},
    {0x01, 0x71, 0x09, 0x05, 0x03}, {0x36, 0x49, 0x49, 0x49, 0x36},
    {0x06, 0x49, 0x49, 0x29, 0x1e}, {0x00, 0x36, 0x36, 0x00, 0x00},
    {0x08, 0x08, 0x08, 0x08, 0x08}, {0x60, 0x18, 0x06, 0x01, 0x00},
};

static uint8_t frame[OLED_WIDTH * OLED_PAGES];

static const uint8_t *glyph(char character) {
    const char *entry = strchr(FONT_CHARS, character);
    if (entry == NULL) {
        return FONT[0];
    }
    return FONT[entry - FONT_CHARS];
}

static esp_err_t i2c_write(uint8_t address, uint8_t control, const uint8_t *data, size_t length) {
    i2c_cmd_handle_t command = i2c_cmd_link_create();
    if (command == NULL) {
        return ESP_ERR_NO_MEM;
    }
    i2c_master_start(command);
    i2c_master_write_byte(command, (address << 1U) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(command, control, true);
    i2c_master_write(command, (uint8_t *)data, length, true);
    i2c_master_stop(command);
    esp_err_t result = i2c_master_cmd_begin(I2C_NUM_0, command, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(command);
    return result;
}

static esp_err_t probe_address(uint8_t address) {
    i2c_cmd_handle_t command = i2c_cmd_link_create();
    if (command == NULL) {
        return ESP_ERR_NO_MEM;
    }
    i2c_master_start(command);
    i2c_master_write_byte(command, (address << 1U) | I2C_MASTER_WRITE, true);
    i2c_master_stop(command);
    esp_err_t result = i2c_master_cmd_begin(I2C_NUM_0, command, pdMS_TO_TICKS(25));
    i2c_cmd_link_delete(command);
    return result;
}

static void clear_frame(void) {
    memset(frame, 0, sizeof(frame));
}

static void draw_text(uint8_t page, uint8_t column, const char *text) {
    if (page >= OLED_PAGES || text == NULL) {
        return;
    }
    size_t offset = (size_t)page * OLED_WIDTH + column;
    while (*text != '\0' && offset + 6 <= (size_t)(page + 1) * OLED_WIDTH) {
        const uint8_t *pixels = glyph(*text++);
        for (size_t index = 0; index < 5; ++index) {
            frame[offset++] = pixels[index];
        }
        frame[offset++] = 0;
    }
}

static esp_err_t flush_frame(uint8_t address) {
    for (uint8_t page = 0; page < OLED_PAGES; ++page) {
        const uint8_t position[] = {0xb0U | page, 0x00, 0x10};
        esp_err_t result = i2c_write(address, 0x00, position, sizeof(position));
        if (result != ESP_OK) {
            return result;
        }
        result = i2c_write(address, 0x40, &frame[(size_t)page * OLED_WIDTH], OLED_WIDTH);
        if (result != ESP_OK) {
            return result;
        }
    }
    return ESP_OK;
}

static esp_err_t initialize_display(uint8_t address) {
    const uint8_t commands[] = {
        0xae, 0xd5, 0x80, 0xa8, 0x3f, 0xd3, 0x00, 0x40, 0x8d, 0x14,
        0x20, 0x00, 0xa1, 0xc8, 0xda, 0x12, 0x81, 0x7f, 0xd9, 0xf1,
        0xdb, 0x40, 0xa4, 0xa6, 0xaf,
    };
    return i2c_write(address, 0x00, commands, sizeof(commands));
}

esp_err_t huginn_heltec_v3_oled_console_start(const char *image_role, bool self_test_passed) {
    const i2c_config_t configuration = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = OLED_SDA,
        .scl_io_num = OLED_SCL,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master = {.clk_speed = 100000},
        .clk_flags = 0,
    };

    gpio_set_direction(OLED_POWER, GPIO_MODE_OUTPUT);
    gpio_set_level(OLED_POWER, 0);  // Verified V3 Vext enable level from Heltec examples.
    vTaskDelay(pdMS_TO_TICKS(25));
    gpio_set_direction(OLED_RESET, GPIO_MODE_OUTPUT);
    gpio_set_level(OLED_RESET, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(OLED_RESET, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    esp_err_t result = i2c_param_config(I2C_NUM_0, &configuration);
    if (result != ESP_OK) {
        return result;
    }
    result = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    if (result != ESP_OK) {
        return result;
    }

    uint8_t address = 0;
    for (uint8_t candidate = OLED_FIRST_ADDRESS; candidate <= OLED_LAST_ADDRESS; ++candidate) {
        if (probe_address(candidate) == ESP_OK) {
            address = candidate;
            break;
        }
    }
    if (address == 0) {
        ESP_LOGE(TAG, "HUGINN_OLED_CONSOLE=NO_DEVICE_ACK");
        return ESP_ERR_NOT_FOUND;
    }
    result = initialize_display(address);
    if (result == ESP_OK) {
        clear_frame();
        draw_text(0, 0, "HUG1NN");
        draw_text(2, 0, "LOCAL CONSOLE");
        draw_text(4, 0,
                  image_role != NULL && strcmp(image_role, "recovery") == 0
                      ? "RECOVERY IMAGE"
                      : "BASE IMAGE");
        draw_text(5, 0, "SERIAL READY");
        draw_text(7, 0, self_test_passed ? "STATUS: PASS" : "STATUS: CHECK");
        result = flush_frame(address);
    }
    if (result == ESP_OK) {
        ESP_LOGI(TAG, "HUGINN_OLED_CONSOLE=READY address=0x%02x", address);
    } else {
        ESP_LOGE(TAG, "HUGINN_OLED_CONSOLE=%s", esp_err_to_name(result));
    }
    return result;
}
