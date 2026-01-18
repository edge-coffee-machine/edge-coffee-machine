#include "esp_afe_sr_models.h"
#include "esp_mn_models.h"
#include "EdgeCoffeeMachine.h" // Tvoj header
#include <platforminterface/log.h>

// Task koji će raditi u pozadini
void speech_recognition_task(void *arg) {
    // 1. Inicijalizacija AFE (Audio Front-End)
    // Ovde koristiš isti setup kao u esp-skainet primeru koji ti je radio
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.wakenet_model_name = esp_wn_get_model_name_by_id(DET_MODE_90);

    const esp_afe_sr_iface_t *afe_handle = &esp_afe_sr_v1;
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);

    // 2. Glavna petlja
    while (true) {
        // Ovde bi išlo čitanje sa I2S (iz primera)
        // afe_handle->feed(afe_data, audio_buffer);

        // Provera komande preko MultiNet-a
        int command_id = 0; // Ovde dobijaš ID nakon detekcije (vidi skainet primer)

        // --- KLJUČNI DEO: POVEZIVANJE SA TVOJOM LOGIKOM ---
        if (command_id == 1) { // 1 je ID za "brew coffee"
            Qul::PlatformInterface::log("[Speech] Command 'Brew Coffee' detected!\n");

            // Pozivamo tvoju metodu direktno preko instance
            // Koristimo nullptr da bi on uzeo trenutno selektovani napitak
            // ili možeš specifično potražiti "Espresso" objekat
            Logic::EdgeCoffeeMachine::instance().makeDrink(nullptr);
        }
    }
}
