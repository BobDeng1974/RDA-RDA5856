/* 
* Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#ifndef VPPP_NOISESUPPRESS_ASM_PREPROCESS_H
#define VPPP_NOISESUPPRESS_ASM_PREPROCESS_H

//speex_aec
#define SPEEX_NB_BANDS	24
#define SPEEX_NN	80


//#define SPEEX_NN	256
#define NOISE_SHIFT	7
#define NEG_NOISE_SHIFT	(-NOISE_SHIFT)
#define NEG_NOISE_SHIFT_1 (1-NOISE_SHIFT)
#define SNR_SHIFT	8
#define NEG_SNR_SHIFT	(-SNR_SHIFT)
#define EXPIN_SHIFT 11
#define NEG_EXPIN_SHIFT (-EXPIN_SHIFT)
#define EXPIN_SNR_SHIFT	(EXPIN_SHIFT-SNR_SHIFT)
#define SNR_EXPIN_SHIFT	(SNR_SHIFT-EXPIN_SHIFT)
#define NEG_EXPIN_SNR_SHIFT	(-EXPIN_SNR_SHIFT)

#define SPEEX_N_PLUS_M	(SPEEX_NN+SPEEX_NB_BANDS)

#define SPEECH_PROB_START_DEFAULT       11469
#define SPEECH_PROB_CONTINUE_DEFAULT    6554
#define NOISE_SUPPRESS_DEFAULT       -30
#define ECHO_SUPPRESS_DEFAULT        -60
#define ECHO_SUPPRESS_ACTIVE_DEFAULT -40

#define MAXFACTORS 32

//voc_short KFS_substate_nfft	,y
//voc_short KFS_substate_inverse	,y
//voc_short KFS_substate_factors	,(2*MAXFACTORS),y
//voc_short KFS_substate_twiddles	,2,y
//voc_short KFS_tmpbuf	,(2*(SPEEX_NN)),y
//voc_short KFS_super_twiddles	,(2*(SPEEX_NN)),y 
#define SUBSTATE_NFFT_OFFSET	0
#define SUBSTATE_INVERSE_OFFSET	(SUBSTATE_NFFT_OFFSET+1)
#define SUBSTATE_FACTORS_OFFSET	(SUBSTATE_INVERSE_OFFSET+1)
#define SUBSTATE_TWIDDLES_OFFSET	(SUBSTATE_FACTORS_OFFSET+2*MAXFACTORS)
#define TMPBUF_OFFSET	(SUBSTATE_TWIDDLES_OFFSET+2)
#define SUPER_TWIDDLES_OFFSET	(TMPBUF_OFFSET+2*SPEEX_NN)


#define SPEEX_NOISESUPPRESS_CONSTX_SIZE				960

// speex_aec functions
void speex_div32_16(void);
void spx_ilog2(void);
void spx_ilog4(void);
void spx_atan01(void);
void spx_atan(void);
void speex_toBARK(void);
void filterbank_new(void);
void _spx_cos_pi_2(void);
void spx_cos_norm(void);
void spx_sqrt(void);
void spx_fft(void);
void filterbank_compute_bank32(void);
void update_noise_prob(void);
void spx_DIV32_16_Q8(void);
void spx_DIV32_16_Q15(void);
void spx_fft_init(void);
void spx_kf_factor(void);
void spx_qcurve(void);
void compute_gain_floor(void);
void hypergeom_gain(void);
void spx_exp(void);
void spx_ifft(void);
void spx_kiss_fft(void);
void spx_kf_shuffle(void);
void spx_kf_bfly4(void);
void spx_kiss_fftri2(void);
void spx_kf_work(void);
void spx_kf_shuffle(void);
void spx_kf_bfly5(void);
void speex_preprocess_state_init(void);
void speex_preprocess_run(void);
void speex_div32_16_sign(void);

void preprocess_analysis(void);
void spx_mult16_32_q15(void);
void filterbank_compute_psd16(void);
void spx_mult16_32_p15(void);
void Rda_speex_preprocess_state_reset(void);
void spx_kiss_fftr2(void);



#endif

