#include <inttypes.h>

#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "heltec_v3_display_probe.h"

static const char *TAG = "heltec-v3-probe";

enum {
    HUGINN_HELTEC_V3_OLED_SDA = 17,
    HUGINN_HELTEC_V3_OLED_SCL = 18,
    HUGINN_HELTEC_V3_OLED_RESET = 21,
    HUGINN_HELTEC_V3_DISPLAY_POWER = 36,
    HUGINN_HELTEC_V3_STATUS_LED = 35,
    HUGINN_I2C_FIRST_7BIT_ADDRESS = 0x08,
    HUGINN_I2C_LAST_7BIT_ADDRESS = 0x77,
};

static esp_err_t probe_i2c_address(uint8_t address) {
    i2c_cmd_handle_t command = i2c_cmd_link_create();
    if (command == NULL) {
        return ESP_ERR_NO_MEM;
    }

    i2c_master_start(command);
    i2c_master_write_byte(command, (address << 1U) | I2C_MASTER_WRITE, true);
    i2c_master_stop(command);
    esp_err_t result = i2c_master_cmd_begin(I2C_NUM_0, command, pdMS_TO_TICKS(20));
    i2c_cmd_link_delete(command);
    return result;
}

esp_err_t huginn_heltec_v3_display_probe(void) {
    const i2c_config_t configuration = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = HUGINN_HELTEC_V3_OLED_SDA,
        .scl_io_num = HUGINN_HELTEC_V3_OLED_SCL,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master = {.clk_speed = 100000},
        .clk_flags = 0,
    };

    ESP_LOGI(TAG, "HUGINN_BOARD_PROFILE=heltec-wifi-lora-32-v3");
    ESP_LOGI(TAG, "HUGINN_SERIAL_DIAGNOSTICS=uart-default");
    ESP_LOGI(TAG,
             "HUGINN_DISPLAY_PROBE_BUS=i2c0 sda=%d scl=%d reset=%d power=%d led=%d",
             HUGINN_HELTEC_V3_OLED_SDA, HUGINN_HELTEC_V3_OLED_SCL,
             HUGINN_HELTEC_V3_OLED_RESET, HUGINN_HELTEC_V3_DISPLAY_POWER,
             HUGINN_HELTEC_V3_STATUS_LED);
    ESP_LOGI(TAG, "HUGINN_DISPLAY_PROBE_ACTUATION=none");

    esp_err_t result = i2c_param_config(I2C_NUM_0, &configuration);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "HUGINN_DISPLAY_PROBE_SETUP=%s", esp_err_to_name(result));
        return result;
    }

    result = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "HUGINN_DISPLAY_PROBE_SETUP=%s", esp_err_to_name(result));
        return result;
    }

    uint32_t acknowledgements = 0;
    for (uint8_t address = HUGINN_I2C_FIRST_7BIT_ADDRESS;
         address <= HUGINN_I2C_LAST_7BIT_ADDRESS; ++address) {
        result = probe_i2c_address(address);
        if (result == ESP_OK) {
            ++acknowledgements;
            ESP_LOGI(TAG, "HUGINN_DISPLAY_PROBE_ACK=0x%02" PRIx8, address);
        }
    }

    esp_err_t uninstall_result = i2c_driver_delete(I2C_NUM_0);
    if (uninstall_result != ESP_OK) {
        ESP_LOGE(TAG, "HUGINN_DISPLAY_PROBE_TEARDOWN=%s", esp_err_to_name(uninstall_result));
        return uninstall_result;
    }

    if (acknowledgements == 0) {
        ESP_LOGI(TAG, "HUGINN_DISPLAY_PROBE_RESULT=NO_DEVICE_ACK");
    } else {
        ESP_LOGI(TAG, "HUGINN_DISPLAY_PROBE_RESULT=ACKS=%" PRIu32, acknowledgements);
    }
    return ESP_OK;
}
