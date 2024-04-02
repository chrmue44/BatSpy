
#ifndef Audio_sim_h_
#define Audio_sim_h_


//#include "DMAChannel.h"
#
// When changing multiple audio object settings that must update at
// the same time, these functions allow the audio library interrupt
// to be disabled.  For example, you may wish to begin playing a note
// in response to reading an analog sensor.  If you have "velocity"
// information, you might start the sample playing and also adjust
// the gain of a mixer channel.  Use AudioNoInterrupts() first, then
// make both changes to the 2 separate objects.  Then allow the audio
// library to update with AudioInterrupts().  Both changes will happen
// at the same time, because AudioNoInterrupts() prevents any updates
// while you make changes.
//
#define AudioNoInterrupts() { /* TODO */ }  // (NVIC_DISABLE_IRQ(IRQ_SOFTWARE))
#define AudioInterrupts()   { /* TODO */ }  //{(NVIC_ENABLE_IRQ(IRQ_SOFTWARE))

// include all the library headers, so a sketch can use a single
// #include <Audio.h> to get the whole library
//

#define __IMXRT1062__ 1
#define __ARM_ARCH_7EM__ 1
#include "analyze_fft1024.h"
#include "analyze_print.h"
#include "analyze_peak.h"
#include "filter_biquad.h"
#include "input_spi_mono.h"
#include "output_mqs.h"
#include "mixer.h"
#include "output_mqs.h"
#include "play_queue.h"
#include "effect_delay.h"
#include "effect_multiply.h"
//#include "play_sd_raw.h"
//#include "play_sd_wav.h"
#include "record_queue.h"
#include "synth_sine.h"
#include "spi_interrupt.h"

#endif
