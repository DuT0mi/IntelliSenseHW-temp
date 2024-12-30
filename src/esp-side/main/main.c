#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

// Define default constants for scanning
#define DEFAULT_SCAN_LIST_SIZE 20
#define NUMBER_OF_PROPER_CHANNELS 3
#define TOP_N 3

// Tag for logging messages
static const char *TAG = "SCANNING |";

// Array to store valid channels
static int8_t proper_channels[NUMBER_OF_PROPER_CHANNELS];

// Struct to hold Wi-Fi scan configuration
typedef struct _wifi_scan_configuration {
    wifi_mode_t wifi_mode;           // Wi-Fi mode (e.g., STA, AP)
    wifi_scan_config_t scanning_parameters; // Scanning parameters
    /**
     * Determines if scan should block, if block is true, 
     * this API will block the caller until the scan is 
     * done, otherwise it will return immediately
     */
    bool should_block;
} wifi_scan_configuration;

// Initialize the network stack
static void initialize_stack() {
    // Initialize network interface
    ESP_ERROR_CHECK(esp_netif_init());

    // Create default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create default Wi-Fi station network interface
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif); // Ensure network interface was created successfully
}

// Initialize the Wi-Fi module
static void initialize_wifi() {
    // Set default Wi-Fi initialization configuration
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));

    // Configure Wi-Fi country settings
    wifi_country_t country = {
        .cc = "HU ",              // Country code (Hungary)
        .schan = 1,               // Start channel
        .nchan = 14,              // Number of channels
        .max_tx_power = 20,       // Max transmission power (dBm)
        .policy = WIFI_COUNTRY_POLICY_MANUAL // Manual country policy
    };

    // Apply the country settings
    ESP_ERROR_CHECK(esp_wifi_set_country(&country));
}

// Perform both stack and Wi-Fi initialization
static void initialize() {
    initialize_stack(); // Initialize network stack
    initialize_wifi();  // Initialize Wi-Fi
}

// Find the top N RSSI values from a list
void find_top_n_rssi(int8_t *rssi_values, uint16_t count, int top_n) {
    int8_t top_values[top_n]; // Array to store top N RSSI values
    memset(top_values, -128, sizeof(top_values)); // Initialize with lowest RSSI value (int8_t lowest value)

    for (int i = 0; i < count; i++) {
        // Compare each RSSI value to find the top N
        for (int j = 0; j < top_n; j++) {
            if (rssi_values[i] > top_values[j]) {
                // Shift lower ranked values down
                for (int k = top_n - 1; k > j; k--) {
                    top_values[k] = top_values[k - 1];
                }
                top_values[j] = rssi_values[i]; // Insert new value
                break;
            }
        }
    }

    // Log the top N RSSI values
    ESP_LOGI(TAG, "Top %d RSSI values:", top_n);
    for (int i = 0; i < top_n; i++) {
        ESP_LOGI(TAG, "RSSI #%d: %d", i + 1, top_values[i]);
    }
}

// Perform a Wi-Fi scan and log the results
static void scan_wifi(const wifi_scan_configuration *configuration) {
    memset(proper_channels, 0, sizeof(proper_channels)); // Reset channel list

    // Set Wi-Fi mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(configuration->wifi_mode));
    ESP_ERROR_CHECK(esp_wifi_start()); // Start Wi-Fi module

    // Start scanning
    ESP_ERROR_CHECK(esp_wifi_scan_start(&configuration->scanning_parameters, configuration->should_block));

    uint16_t ap_count = 0; // Count of access points
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count)); // Get number of APs found
    ESP_LOGI(TAG, "Total APs scanned: %u", ap_count);

    if (ap_count > 0) {
        // Array to store AP information
        wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
        memset(ap_info, 0, sizeof(ap_info));

        // Limit the number of APs to process
        uint16_t number_of_aps = (ap_count > DEFAULT_SCAN_LIST_SIZE) ? DEFAULT_SCAN_LIST_SIZE : ap_count;
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number_of_aps, ap_info));

        // Extract and log RSSI values
        int8_t rssi_values[DEFAULT_SCAN_LIST_SIZE];
        for (int i = 0; i < number_of_aps; i++) {
            rssi_values[i] = ap_info[i].rssi; // Store RSSI value
            ESP_LOGI(TAG, "SSID: %s, RSSI: %d, Channel: %d", ap_info[i].ssid, ap_info[i].rssi, ap_info[i].primary);
        }

        // Find and log the top N RSSI values
        find_top_n_rssi(rssi_values, number_of_aps, TOP_N);
    } else {
        ESP_LOGI(TAG, "No APs found."); // Log if no APs are found
    }

    ESP_ERROR_CHECK(esp_wifi_stop()); // Stop Wi-Fi module
}

// Main application entry point
void app_main(void) {
    // Initialize flash memory
    esp_err_t flash_init = nvs_flash_init();
    if (flash_init == ESP_ERR_NVS_NO_FREE_PAGES || flash_init == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase()); // Erase flash if needed
        flash_init = nvs_flash_init(); // Reinitialize flash
    }
    ESP_ERROR_CHECK(flash_init);

    // Define Wi-Fi scan parameters
    wifi_scan_config_t scan_params = {
        .ssid = NULL,            // No specific SSID
        .bssid = NULL,           // No specific BSSID
        .channel = 0,            // Scan all channels
        .show_hidden = false,    // Ignore hidden SSIDs
        .scan_type = WIFI_SCAN_TYPE_ACTIVE, // Active scanning
        .scan_time = {
            .active = { .min = 100, .max = 300 }, // Active scan time range (ms)
            .passive = 300       // Passive scan time (ms)
        }
    };

    // Define Wi-Fi configuration
    wifi_scan_configuration wifi_config = {
        .wifi_mode = WIFI_MODE_STA, // Set mode to station
        .scanning_parameters = scan_params, // Apply scan parameters
        .should_block = true,     // Block until scan completes
    };

    initialize(); // Initialize stack and Wi-Fi
    scan_wifi(&wifi_config); // Perform Wi-Fi scan
}
