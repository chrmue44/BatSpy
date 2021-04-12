/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "input_i2s.h"
#include "output_i2s.h"

#if !defined(KINETISL)

DMAMEM __attribute__((aligned(32))) static uint32_t i2s_rx_buffer[AUDIO_BLOCK_SAMPLES];
audio_block_t * AudioInputI2S::block_left = NULL;
audio_block_t * AudioInputI2S::block_right = NULL;
bool AudioInputI2S::_irqRuns = false;
int16_t AudioInputI2S::wRight = 0;
int16_t AudioInputI2S::wLeft = 0;
uint16_t AudioInputI2S::block_offset = 0;
bool AudioInputI2S::update_responsibility = false;
DMAChannel AudioInputI2S::dma(false);


void AudioInputI2S::begin(void)
{
	dma.begin(true); // Allocate the DMA channel first

	//block_left_1st = NULL;
	//block_right_1st = NULL;

	// TODO: should we set & clear the I2S_RCSR_SR bit here?
	AudioOutputI2S::config_i2s();

#if defined(KINETISK)
	CORE_PIN13_CONFIG = PORT_PCR_MUX(4); // pin 13, PTC5, I2S0_RXD0
	dma.TCD->SADDR = (void *)((uint32_t)&I2S0_RDR0 + 2);
	dma.TCD->SOFF = 0;
	dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
	dma.TCD->NBYTES_MLNO = 2;
	dma.TCD->SLAST = 0;
	dma.TCD->DADDR = i2s_rx_buffer;
	dma.TCD->DOFF = 2;
	dma.TCD->CITER_ELINKNO = sizeof(i2s_rx_buffer) / 2;
	dma.TCD->DLASTSGA = -sizeof(i2s_rx_buffer);
	dma.TCD->BITER_ELINKNO = sizeof(i2s_rx_buffer) / 2;
	dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
	dma.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_RX);

	I2S0_RCSR |= I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE | I2S_RCSR_FR;
	I2S0_TCSR |= I2S_TCSR_TE | I2S_TCSR_BCE; // TX clock enable, because sync'd to TX

#elif defined(__IMXRT1062__)
	CORE_PIN8_CONFIG  = 3;  //1:RX_DATA0
	IOMUXC_SAI1_RX_DATA0_SELECT_INPUT = 2;

//	dma.TCD->SADDR = (void *)((uint32_t)&I2S1_RDR0 + 2);
	dma.TCD->SADDR = (void *)((uint32_t)&I2S1_RDR0);
	dma.TCD->SOFF = 0;
	dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
	dma.TCD->NBYTES_MLNO = 2;
	dma.TCD->SLAST = 0;
	dma.TCD->DADDR = i2s_rx_buffer;
	dma.TCD->DOFF = 2;
	dma.TCD->CITER_ELINKNO = sizeof(i2s_rx_buffer) / 2;
	dma.TCD->DLASTSGA = -sizeof(i2s_rx_buffer);
	dma.TCD->BITER_ELINKNO = sizeof(i2s_rx_buffer) / 2;
	dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
	dma.triggerAtHardwareEvent(DMAMUX_SOURCE_SAI1_RX);

/*
working setup with 32 bit words
                     31    24 23    16 15     8 7      0          
I2S1RCSR 0x90100001  10010000 00010000 00000000 00000001
31: RE = 1
28: BCE = 1
20: WSF = 1
 0: FRDE = 1
I2S1RCR1 0x00000001  00000000 00000000 00000000 00000001 
0..4: RFW = 1
I2S1RCR2 0x07000001  00000111 00000000 00000000 00000001  
26, 27: MSEL = 1
25: BCP = 1
24: BCD = 1
0..7: DIV = 1
I2S1RCR3 0x00010000  00000000 00000001 00000000 00000000
nix
I2S1RCR4 0x00011f1b  00000000 00000001 00011111 00011011
16..20: FRSZ = 1
8..12: SYWD = 31
4: MF = 1
3: FSE = 1
1: FSP = 1
0: FSD = 1
I2S1RCR5 0x1f1f1f00  00011111 00011111 00011111 00000000
24..28: WNW = 31
16...20: WOW = 31
8...12: FBT = 31
*/

uint16_t ww = 15;
   I2S1_RCSR = 0;
//   I2S1_TCSR = 0;
   I2S1_RCR2 |= I2S_RCR2_BCP;  //polarity bit clock: 0 falling, 1: rising edge<

   I2S1_RCR4 &= ~I2S_RCR4_SYWD(31);
   I2S1_RCR4 |= I2S_RCR4_SYWD(ww);

//   I2S1_RCR4 &= ~I2S_RCR4_FSE;
  // I2S1_RCR4 |= I2S_RCR4_FSE;
//   I2S1_RCR4 &= ~I2S_RCR4_FSP;  // frame sync polarity

   I2S1_RCR5 = I2S_RCR5_WNW(ww) | I2S_RCR5_W0W(ww) | I2S_RCR5_FBT(ww);  
//   I2S1_TCR5 = I2S_TCR5_WNW(ww) | I2S_TCR5_W0W(ww) | I2S_TCR5_FBT(ww);  

	I2S1_RCSR = I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE | I2S_RCSR_FR;

#endif
	update_responsibility = update_setup();
	dma.enable();
	dma.attachInterrupt(isr);
}

void AudioInputI2S::isr(void)
{
	uint32_t daddr, offset;
	const int16_t *src, *end;
	int16_t *dest_left, *dest_right;
	audio_block_t *left, *right;

#if defined(KINETISK) || defined(__IMXRT1062__)
	daddr = (uint32_t)(dma.TCD->DADDR);
	dma.clearInterrupt();
	//Serial.println("isr");
    _irqRuns = true;
	if (daddr < (uint32_t)i2s_rx_buffer + sizeof(i2s_rx_buffer) / 2) {
		// DMA is receiving to the first half of the buffer
		// need to remove data from the second half
		src = (int16_t *)&i2s_rx_buffer[AUDIO_BLOCK_SAMPLES/2];
		end = (int16_t *)&i2s_rx_buffer[AUDIO_BLOCK_SAMPLES];
		if (AudioInputI2S::update_responsibility) AudioStream::update_all();
	} else {
		// DMA is receiving to the second half of the buffer
		// need to remove data from the first half
		src = (int16_t *)&i2s_rx_buffer[0];
		end = (int16_t *)&i2s_rx_buffer[AUDIO_BLOCK_SAMPLES/2];
	}
	left = AudioInputI2S::block_left;
	right = AudioInputI2S::block_right;
	if (left != NULL && right != NULL) {
		offset = AudioInputI2S::block_offset;
		if (offset <= AUDIO_BLOCK_SAMPLES/2) {
			dest_left = &(left->data[offset]);
			dest_right = &(right->data[offset]);
			AudioInputI2S::block_offset = offset + AUDIO_BLOCK_SAMPLES/2;
			arm_dcache_delete((void*)src, sizeof(i2s_rx_buffer) / 2);
			do {
      			wLeft = *src++;
				wLeft = (wRight << 15) | (wLeft >> 1);
				wRight = *src++;
				*dest_left++ = wLeft;
				*dest_right++ = wLeft;
			} while (src < end);
		}
	}
    _irqRuns = false;
#endif
}



void AudioInputI2S::update(void)
{
	audio_block_t *new_left=NULL, *new_right=NULL, *out_left=NULL, *out_right=NULL;

	// allocate 2 new blocks, but if one fails, allocate neither
	new_left = allocate();
	if (new_left != NULL) {
		new_right = allocate();
		if (new_right == NULL) {
			release(new_left);
			new_left = NULL;
		}
	}
	__disable_irq();
	if (block_offset >= AUDIO_BLOCK_SAMPLES) {
		// the DMA filled 2 blocks, so grab them and get the
		// 2 new blocks to the DMA, as quickly as possible
		out_left = block_left;
		block_left = new_left;
		out_right = block_right;
		block_right = new_right;
		block_offset = 0;
		__enable_irq();
		// then transmit the DMA's former blocks
		transmit(out_left, 0);
		release(out_left);
		transmit(out_right, 1);
		release(out_right);
		//Serial.print(".");
	} else if (new_left != NULL) {
		// the DMA didn't fill blocks, but we allocated blocks
		if (block_left == NULL) {
			// the DMA doesn't have any blocks to fill, so
			// give it the ones we just allocated
			block_left = new_left;
			block_right = new_right;
			block_offset = 0;
			__enable_irq();
		} else {
			// the DMA already has blocks, doesn't need these
			__enable_irq();
			release(new_left);
			release(new_right);
		}
	} else {
		// The DMA didn't fill blocks, and we could not allocate
		// memory... the system is likely starving for memory!
		// Sadly, there's nothing we can do.
		__enable_irq();
	}
}


/******************************************************************/


void AudioInputI2Sslave::begin(void)
{
	dma.begin(true); // Allocate the DMA channel first

	AudioOutputI2Sslave::config_i2s();
#if defined(KINETISK)
	CORE_PIN13_CONFIG = PORT_PCR_MUX(4); // pin 13, PTC5, I2S0_RXD0

	dma.TCD->SADDR = (void *)((uint32_t)&I2S0_RDR0 + 2);
	dma.TCD->SOFF = 0;
	dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
	dma.TCD->NBYTES_MLNO = 2;
	dma.TCD->SLAST = 0;
	dma.TCD->DADDR = i2s_rx_buffer;
	dma.TCD->DOFF = 2;
	dma.TCD->CITER_ELINKNO = sizeof(i2s_rx_buffer) / 2;
	dma.TCD->DLASTSGA = -sizeof(i2s_rx_buffer);
	dma.TCD->BITER_ELINKNO = sizeof(i2s_rx_buffer) / 2;
	dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;

	dma.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_RX);
	update_responsibility = update_setup();
	dma.enable();

	I2S0_RCSR |= I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE | I2S_RCSR_FR;
	I2S0_TCSR |= I2S_TCSR_TE | I2S_TCSR_BCE; // TX clock enable, because sync'd to TX
	dma.attachInterrupt(isr);

#elif defined(__IMXRT1062__)
	CORE_PIN8_CONFIG  = 3;  //1:RX_DATA0
	IOMUXC_SAI1_RX_DATA0_SELECT_INPUT = 2;

	dma.TCD->SADDR = (void *)((uint32_t)&I2S1_RDR0 + 2);
	dma.TCD->SOFF = 0;
	dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
	dma.TCD->NBYTES_MLNO = 2;
	dma.TCD->SLAST = 0;
	dma.TCD->DADDR = i2s_rx_buffer;
	dma.TCD->DOFF = 2;
	dma.TCD->CITER_ELINKNO = sizeof(i2s_rx_buffer) / 2;
	dma.TCD->DLASTSGA = -sizeof(i2s_rx_buffer);
	dma.TCD->BITER_ELINKNO = sizeof(i2s_rx_buffer) / 2;
	dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
	dma.triggerAtHardwareEvent(DMAMUX_SOURCE_SAI1_RX);
	dma.enable();

	I2S1_RCSR = 0;
	I2S1_RCSR = I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE | I2S_RCSR_FR;
	I2S1_RCR5 = 0;
	

	update_responsibility = update_setup();
	dma.attachInterrupt(isr);
#endif
}

#elif defined(KINETISL)

/**************************************************************************************
*       Teensy LC
***************************************************************************************/
#define NUM_SAMPLES (AUDIO_BLOCK_SAMPLES / 2)

DMAMEM static int16_t i2s_rx_buffer1[NUM_SAMPLES * 2];
DMAMEM static int16_t i2s_rx_buffer2[NUM_SAMPLES * 2];
audio_block_t * AudioInputI2S::block_left = NULL;
audio_block_t * AudioInputI2S::block_right = NULL;
DMAChannel AudioInputI2S::dma1(false);
DMAChannel AudioInputI2S::dma2(false);
bool AudioInputI2S::update_responsibility = false;

void AudioInputI2S::begin(void)
{
	memset(i2s_rx_buffer1, 0, sizeof( i2s_rx_buffer1 ) );
	memset(i2s_rx_buffer2, 0, sizeof( i2s_rx_buffer2 ) );

	dma1.begin(true);
	dma2.begin(true);

	AudioOutputI2S::config_i2s();
	CORE_PIN13_CONFIG = PORT_PCR_MUX(4); // pin 13, PTC5, I2S0_RXD0

	//configure both DMA channels
	dma1.CFG->SAR = (void *)((uint32_t)&I2S0_RDR0 + 2);
	dma1.CFG->DCR = (dma1.CFG->DCR & 0xF08E0FFF) | DMA_DCR_SSIZE(2);
	dma1.destinationBuffer(i2s_rx_buffer1, sizeof(i2s_rx_buffer1));
	dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_RX);
	dma1.interruptAtCompletion();
	dma1.disableOnCompletion();
	dma1.attachInterrupt(isr1);

	dma2.CFG->SAR = dma1.CFG->SAR;
	dma2.CFG->DCR = dma1.CFG->DCR;
	dma2.destinationBuffer(i2s_rx_buffer2, sizeof(i2s_rx_buffer2));
	dma2.interruptAtCompletion();
	dma2.disableOnCompletion();
	dma2.attachInterrupt(isr2);

	I2S0_RCSR = 0;
	I2S0_RCSR = I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FWDE | I2S_RCSR_FR;
	I2S0_TCSR |= I2S_TCSR_TE | I2S_TCSR_BCE; // TX clock enable, because sync'd to TX

	update_responsibility = update_setup();
	dma1.enable();
}

void AudioInputI2S::update(void)
{

        //Keep it simple
	//If we have a block, transmit and release it.
	if (block_left) {
		transmit(block_left, 0);
		release(block_left);
		block_left = nullptr;
	}

	if (block_right) {
		transmit(block_right, 1);
		release(block_right);
		block_right = nullptr;
	}

	// allocate 2 new blocks, but if one fails, allocate neither
	block_left = allocate();
	if (block_left != nullptr) {
		block_right = allocate();
		if (block_right == nullptr) {
			release(block_left);
			block_left = nullptr;
		}
	}

}

//todo : ("unroll-loops") or optimize better
inline __attribute__((always_inline, hot, optimize("O2") ))
static void deinterleave(const int16_t *src,audio_block_t *block_left, audio_block_t *block_right, const unsigned offset)
{
	//we can assume that we have either two blocks or none

	if (!block_left) return;

	for (unsigned i=0; i < NUM_SAMPLES; i++) {
		block_left->data[i + offset] = src[i*2];
		block_right->data[i + offset] = src[i*2+1];
	}

}

void AudioInputI2S::isr1(void)
{
	//DMA Channel 1 Interrupt

	//Start Channel 2:
	dma2.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_RX);
	dma2.enable();

	//Reset & Copy Data Channel 1
	dma1.clearInterrupt();
	dma1.destinationBuffer(i2s_rx_buffer1, sizeof(i2s_rx_buffer1));
	deinterleave(&i2s_rx_buffer1[0], AudioInputI2S::block_left, AudioInputI2S::block_right, 0);
}

void AudioInputI2S::isr2(void)
{
	//DMA Channel 2 Interrupt

	//Start Channel 1:
	dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_RX);
	dma1.enable();

	//Reset & Copy Data Channel 2
	dma2.clearInterrupt();
	dma2.destinationBuffer(i2s_rx_buffer2, sizeof(i2s_rx_buffer2));
	deinterleave(&i2s_rx_buffer2[0], AudioInputI2S::block_left, AudioInputI2S::block_right, NUM_SAMPLES);
	if (AudioInputI2S::update_responsibility) AudioStream::update_all();
}

void AudioInputI2Sslave::begin(void)
{
	memset(i2s_rx_buffer1, 0, sizeof( i2s_rx_buffer1 ) );
	memset(i2s_rx_buffer2, 0, sizeof( i2s_rx_buffer2 ) );

	dma1.begin(true);
	dma2.begin(true);

	AudioOutputI2Sslave::config_i2s();
	CORE_PIN13_CONFIG = PORT_PCR_MUX(4); // pin 13, PTC5, I2S0_RXD0

	//configure both DMA channels
	dma1.CFG->SAR = (void *)((uint32_t)&I2S0_RDR0 + 2);
	dma1.CFG->DCR = (dma1.CFG->DCR & 0xF08E0FFF) | DMA_DCR_SSIZE(2);
	dma1.destinationBuffer(i2s_rx_buffer1, sizeof(i2s_rx_buffer1));
	dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_RX);
	dma1.interruptAtCompletion();
	dma1.disableOnCompletion();
	dma1.attachInterrupt(isr1);

	dma2.CFG->SAR = dma1.CFG->SAR;
	dma2.CFG->DCR = dma1.CFG->DCR;
	dma2.destinationBuffer(i2s_rx_buffer2, sizeof(i2s_rx_buffer2));
	dma2.interruptAtCompletion();
	dma2.disableOnCompletion();
	dma2.attachInterrupt(isr2);


	I2S0_RCSR = 0;
	I2S0_RCSR = I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FWDE | I2S_RCSR_FR;

	update_responsibility = update_setup();
	dma1.enable();

}

#endif