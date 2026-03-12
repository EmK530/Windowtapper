#include "window_manager.h"

extern WindowModule Drums_module;
extern WindowModule Chord_module;
extern WindowModule FAFront_module;
extern WindowModule FABack_module;
extern WindowModule WWDTMHigh_module;
extern WindowModule WWDTMLow_module;
extern WindowModule SFRoll_module;
extern WindowModule SFTap_module;
extern WindowModule PM_module;
extern WindowModule Arp_module;
extern WindowModule Noise_module;
extern WindowModule DTMF_module;
extern WindowModule Bass_module;
extern WindowModule Spreader_module;
extern WindowModule Radiolab_module;

WindowModule* modules[] = {
    &Drums_module,
    &Chord_module,
    &FAFront_module,
    &FABack_module,
    &WWDTMHigh_module,
    &WWDTMLow_module,
    &SFRoll_module,
    &SFTap_module,
    &PM_module,
    &Arp_module,
    &Noise_module,
    &DTMF_module,
    &Bass_module,
    &Spreader_module,
    &Radiolab_module
};