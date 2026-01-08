#include "VoiceEngine.h"
#include "EdgeCoffeeMachine.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bsp/esp-bsp.h"
#include "esp_codec_dev.h"
#include "bsp/esp32_p4_function_ev_board.h"
extern "C" {
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "esp_board_init.h"
#include "model_path.h"
}
//#include "speech_commands_action.h"

//#include "esp_process_sdkconfig.h"

// DEFINICIJE GLOBALNIH VARIJABLI (Ovo je nedostajalo)
static int wakeup_flag = 0;
static const esp_afe_sr_iface_t *afe_handle = NULL;
static volatile int task_flag = 0;
static srmodel_list_t *models = NULL;

// Funkcija za snimanje zvuka
void feed_Task(void *arg)
{
    esp_afe_sr_data_t *afe_data = (esp_afe_sr_data_t *)arg; // FIX: Eksplicitni cast
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int nch = afe_handle->get_feed_channel_num(afe_data);
    int feed_channel = esp_get_feed_channel();
    assert(nch == feed_channel);
    int16_t *i2s_buff = (int16_t *)malloc(audio_chunksize * sizeof(int16_t) * feed_channel);
    assert(i2s_buff);

    while (task_flag) {

        esp_get_feed_data(true, i2s_buff, audio_chunksize * sizeof(int16_t) * feed_channel);

        // DODAJ OVO:
        static int debug_count = 0;
        if (debug_count++ % 100 == 0) {
            printf("Audio Sample: %d\n", i2s_buff[0]);
        }
        afe_handle->feed(afe_data, i2s_buff);
    }

    free(i2s_buff);
    vTaskDelete(NULL);
}
void detect_Task(void *arg)
{
esp_afe_sr_data_t *afe_data = (esp_afe_sr_data_t *)arg;
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    char *mn_name = esp_srmodel_filter(models, ESP_MN_PREFIX, ESP_MN_ENGLISH);
    printf("multinet:%s\n", mn_name);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(mn_name);
    model_iface_data_t *model_data = multinet->create(mn_name, 6000);
    int mu_chunksize = multinet->get_samp_chunksize(model_data);
    //esp_mn_commands_update_from_sdkconfig(multinet, model_data); // Add speech commands from sdkconfig
    assert(mu_chunksize == afe_chunksize);
    //print active speech commands
    multinet->print_active_speech_commands(model_data);

    printf("------------detect start------------\n");
    while (task_flag) {

        afe_fetch_result_t* res = afe_handle->fetch(afe_data);
        if (!res || res->ret_value == ESP_FAIL) continue;

        if (res->wakeup_state == WAKENET_DETECTED) {
            printf("WAKEWORD DETECTED! (Score: %d)\n", res->wakeup_state); // Dodaj ispis
            multinet->clean(model_data);
            wakeup_flag = 1; // Osiguraj da je setovano
        }

        if (wakeup_flag == 1) {
            esp_mn_state_t mn_state = multinet->detect(model_data, res->data);

            if (mn_state == ESP_MN_STATE_DETECTED) {
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                // ... ostatak tvoje logike ...
                wakeup_flag = 0; // Resetuj nakon detekcije
                afe_handle->enable_wakenet(afe_data);
            } else if (mn_state == ESP_MN_STATE_TIMEOUT) {
                printf("MultiNet Timeout\n");
                wakeup_flag = 0;
                afe_handle->enable_wakenet(afe_data);
            }
        }
    }
    if (model_data) {
        multinet->destroy(model_data);
        model_data = NULL;
    }
    printf("detect exit\n");
    vTaskDelete(NULL);

    // ... (exit logic) ...
}

// ... tvoji taskovi (feed_Task, detect_Task) ...

extern "C" {


void start_speech_recognition() {
    models = esp_srmodel_init("model"); // partition label defined in partitions.csv
    ESP_ERROR_CHECK(esp_board_init(16000, 2, 16));
    // ESP_ERROR_CHECK(esp_sdcard_init("/sdcard", 10));

#if CONFIG_IDF_TARGET_ESP32
    printf("This demo only support ESP32S3\n");
    return;
#else
    afe_config_t *afe_config = afe_config_init(esp_get_input_format(), models, AFE_TYPE_SR, AFE_MODE_LOW_COST);
    afe_handle = esp_afe_handle_from_config(afe_config);
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(afe_config);
    afe_config_free(afe_config);
#endif

    task_flag = 1;
    xTaskCreatePinnedToCore(&detect_Task, "detect", 8 * 1024, (void*)afe_data, 5, NULL, 1);
    xTaskCreatePinnedToCore(&feed_Task, "feed", 8 * 1024, (void*)afe_data, 5, NULL, 0);
}
}
