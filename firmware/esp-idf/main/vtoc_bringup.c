#include <inttypes.h>

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "nvs_flash.h"

static const char *TAG = "vtoc-bringup";

static const char *image_role(void) {
#if CONFIG_HUGINN_IMAGE_ROLE_RECOVERY
    return "recovery";
#else
    return "base";
#endif
}

static esp_err_t initialize_known_provisioning_state(void) {
#if CONFIG_HUGINN_IMAGE_ROLE_RECOVERY
    esp_err_t erase_result = nvs_flash_erase();
    if (erase_result != ESP_OK) {
        return erase_result;
    }
#endif
    return nvs_flash_init();
}

static bool psram_is_available(void) {
#if CONFIG_SPIRAM
    return esp_psram_is_initialized();
#else
    return false;
#endif
}

void app_main(void) {
    esp_chip_info_t chip = {0};
    esp_chip_info(&chip);

    uint32_t flash_bytes = 0;
    esp_err_t flash_result = esp_flash_get_size(NULL, &flash_bytes);
    esp_err_t provisioning_result = initialize_known_provisioning_state();

    ESP_LOGI(TAG, "HUGINN_VERSION=0.1.0-bringup");
    ESP_LOGI(TAG, "HUGINN_IMAGE_ROLE=%s", image_role());
    ESP_LOGI(TAG, "HUGINN_TARGET=esp32s3 revision=%u cores=%u", chip.revision, chip.cores);
    ESP_LOGI(TAG, "HUGINN_FLASH_BYTES=%" PRIu32 " status=%s", flash_bytes,
             esp_err_to_name(flash_result));
    ESP_LOGI(TAG, "HUGINN_PSRAM_INITIALIZED=%s", psram_is_available() ? "true" : "false");
    ESP_LOGI(TAG, "HUGINN_FREE_HEAP=%" PRIu32, esp_get_free_heap_size());
    ESP_LOGI(TAG, "HUGINN_PROVISIONING=%s", esp_err_to_name(provisioning_result));
    ESP_LOGI(TAG, "HUGINN_SELF_TEST=%s",
             (flash_result == ESP_OK && provisioning_result == ESP_OK) ? "PASS" : "FAIL");
}
