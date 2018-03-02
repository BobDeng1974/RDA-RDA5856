/* 
 * Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file calibp_audio_defaults.h                                             //
/// This file defines the default calibration parameters for audio            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef  _CALIBP_AUDIO_DEFAULTS_H_
#define  _CALIBP_AUDIO_DEFAULTS_H_

#include "hal_rda_abb.h"


// --------------------------------------------------
// DEFAULT_AUDIO_CALIB_PARAM_USED indicates that the file contains the
// default audio calibration parameters, which are NOT generated by
// the audio calibration tool.
// On the other hand, the file generated by the audio calibration tool
// should NOT contain this macro.

#define DEFAULT_AUDIO_CALIB_PARAM_USED


// --------------------------------------------------
// Common calibration parameters

#define DEFAULT_CALIB_CHIP_ID CHIP_ASIC_ID
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
#if defined(GALLITE_IS_8805)
#define DEFAULT_CALIB_CHIP_DIE 8805
#elif defined(GALLITE_IS_8806)
#define DEFAULT_CALIB_CHIP_DIE 8806
#endif
#endif // Gallite

#define DEFAULT_CALIB_VERSION CALIB_VERSION_NUMBER


//HandSet Mode


///  Audio params, MDF profile.
#define DEFAULT_CALIB_MDF_PROFILE_HS     {0x4000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000}

///  Audio params, SDF profile. 
#define DEFAULT_CALIB_SDF_PROFILE_HS     {0x4000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000    }



///  Audio params, MDF profile.
#define DEFAULT_CALIB_MDF_PROFILE_LS     {0x4000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000}

///  Audio params, SDF profile. 
#define DEFAULT_CALIB_SDF_PROFILE_LS     {0x4000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000    }

///  Audio params, MDF profile.
#define DEFAULT_CALIB_MDF_PROFILE_FM     {0x4000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000}

///  Audio params, SDF profile. 
#define DEFAULT_CALIB_SDF_PROFILE_FM     {0x0400, 0x0400, 0x0400, 0x0400, \
                0x0400, 0x0400, 0x0400, 0x0400, \
                0x0400, 0x0400, 0x0400, 0x0400, \
                0x0400, 0x0400, 0x0400, 0x0400, \
                0x0400, 0x0400, 0x0400, 0x0400, \
                0x0400, 0x0400, 0x0400, 0x0400, \
                0x0400, 0x0400, 0x0400, 0x0400, \
                0x0400, 0x0400, 0x0400, 0x0400, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000    }




///  Audio params, MDF profile.
#define DEFAULT_CALIB_MDF_PROFILE { 0x4000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000}

///  Audio params, SDF profile. 
#define DEFAULT_CALIB_SDF_PROFILE { 0x4000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000, \
                0x0000, 0x0000, 0x0000, 0x0000}


#define DEFAULT_CALIB_AUDIO_HS_PARAMS          { 0,1,0,0,0,50,0,8,127,1,1,1,1,{0,0,0,0}}
#define DEFAULT_CALIB_AUDIO_EP_PARAMS          { 0,1,0,0,0,50,0,8,127,1,1,1,1,{0,0,0,0}}
#define DEFAULT_CALIB_AUDIO_LS_PARAMS          { 0,1,0,1,0,40,1,0,127,1,1,1,1,{0,0,0,0}}
#define DEFAULT_CALIB_AUDIO_BT_PARAMS          { 0,0,0,0,0,50,0,8,127,1,1,1,1,{0,0,0,0}}
#define DEFAULT_CALIB_AUDIO_FM_PARAMS          { 0,0,0,0,0,50,0,8,127,1,1,1,1,{0,0,0,0}}
#define DEFAULT_CALIB_AUDIO_TV_PARAMS          { 0,0,0,0,0,50,0,8,127,1,1,1,1,{0,0,0,0}}

// Echo Cancel params. EC Alg is for HS. In LS mode, EC will NOT be enabled.
//HS
#define DEFAULT_EC_HS_MU    3
#define DEFAULT_EC_HS_REL   1
#define DEFAULT_EC_HS_MIN   7
#define DEFAULT_ES_HS_ON    0
#define DEFAULT_ES_HS_DTD   0
#define DEFAULT_ES_HS_VAD   0

//EP
#define DEFAULT_EC_EP_MU    0
#define DEFAULT_EC_EP_REL   0
#define DEFAULT_EC_EP_MIN   0
#define DEFAULT_ES_EP_ON    0
#define DEFAULT_ES_EP_DTD   0
#define DEFAULT_ES_EP_VAD   0

//LS
#define DEFAULT_EC_LS_MU    0
#define DEFAULT_EC_LS_REL   0
#define DEFAULT_EC_LS_MIN   0
#define DEFAULT_ES_LS_ON    1
#define DEFAULT_ES_LS_DTD   0
#define DEFAULT_ES_LS_VAD   8


//BT
#define DEFAULT_EC_BT_MU    3
#define DEFAULT_EC_BT_REL   2
#define DEFAULT_EC_BT_MIN   3
#define DEFAULT_ES_BT_ON    0
#define DEFAULT_ES_BT_DTD   0
#define DEFAULT_ES_BT_VAD   0

//FM
#define DEFAULT_EC_FM_MU    0
#define DEFAULT_EC_FM_REL   0
#define DEFAULT_EC_FM_MIN   0
#define DEFAULT_ES_FM_ON    1
#define DEFAULT_ES_FM_DTD   0
#define DEFAULT_ES_FM_VAD   8


//TV
#define DEFAULT_EC_TV_MU    0
#define DEFAULT_EC_TV_REL   0
#define DEFAULT_EC_TV_MIN   0
#define DEFAULT_ES_TV_ON    1
#define DEFAULT_ES_TV_DTD   0
#define DEFAULT_ES_TV_VAD   8




#define DEFAULT_NA          0

///  Audio Enable params, for the handset
#define DEFAULT_ENABLE_HANDSET              CALIB_SDF_ON | CALIB_MDF_ON
///  Audio Enable params, for the earpiece
#define DEFAULT_ENABLE_EARPIECE             CALIB_SDF_ON | CALIB_MDF_ON
///  Audio Enable params, for the loudspeaker
#define DEFAULT_ENABLE_LOUDSPEAKER          CALIB_SDF_ON | CALIB_MDF_ON | CALIB_EC_ON

/// Audio VoC parameters.
#define DEFAULT_CALIB_AUDIO_VOC_HS_EC       { DEFAULT_EC_HS_MU, DEFAULT_EC_HS_REL, DEFAULT_EC_HS_MIN, DEFAULT_ES_HS_ON, DEFAULT_ES_HS_DTD, DEFAULT_ES_HS_VAD, DEFAULT_ENABLE_HANDSET}

#define DEFAULT_CALIB_AUDIO_VOC_HS_FILTERS  { DEFAULT_CALIB_MDF_PROFILE_HS, DEFAULT_CALIB_SDF_PROFILE_HS}

#define DEFAULT_CALIB_AUDIO_VOC_EP_EC       { DEFAULT_EC_EP_MU, DEFAULT_EC_EP_REL, DEFAULT_EC_EP_MIN, DEFAULT_ES_EP_ON, DEFAULT_ES_EP_DTD, DEFAULT_ES_EP_VAD, DEFAULT_ENABLE_EARPIECE}

#define DEFAULT_CALIB_AUDIO_VOC_EP_FILTERS  { DEFAULT_CALIB_MDF_PROFILE, DEFAULT_CALIB_SDF_PROFILE }

#define DEFAULT_CALIB_AUDIO_VOC_LS_EC       { DEFAULT_EC_LS_MU, DEFAULT_EC_LS_REL, DEFAULT_EC_LS_MIN, DEFAULT_ES_LS_ON, DEFAULT_ES_LS_DTD, DEFAULT_ES_LS_VAD, DEFAULT_ENABLE_LOUDSPEAKER}

#define DEFAULT_CALIB_AUDIO_VOC_LS_FILTERS  { DEFAULT_CALIB_MDF_PROFILE_LS, DEFAULT_CALIB_SDF_PROFILE_LS }


/// Audio VoC parameters.
#define DEFAULT_CALIB_AUDIO_VOC_BT_EC       { DEFAULT_EC_BT_MU, DEFAULT_EC_BT_REL, DEFAULT_EC_BT_MIN, DEFAULT_ES_BT_ON, DEFAULT_ES_BT_DTD, DEFAULT_ES_BT_VAD, DEFAULT_ENABLE_HANDSET}

#define DEFAULT_CALIB_AUDIO_VOC_BT_FILTERS  { DEFAULT_CALIB_MDF_PROFILE, DEFAULT_CALIB_SDF_PROFILE }

#define DEFAULT_CALIB_AUDIO_VOC_FM_EC       { DEFAULT_EC_FM_MU, DEFAULT_EC_FM_REL, DEFAULT_EC_FM_MIN, DEFAULT_ES_FM_ON, DEFAULT_ES_FM_DTD, DEFAULT_ES_FM_VAD, DEFAULT_ENABLE_LOUDSPEAKER}

#define DEFAULT_CALIB_AUDIO_VOC_FM_FILTERS  { DEFAULT_CALIB_MDF_PROFILE_FM, DEFAULT_CALIB_SDF_PROFILE_FM }

#define DEFAULT_CALIB_AUDIO_VOC_TV_EC       { DEFAULT_EC_TV_MU, DEFAULT_EC_TV_REL, DEFAULT_EC_TV_MIN, DEFAULT_ES_TV_ON, DEFAULT_ES_TV_DTD, DEFAULT_ES_TV_VAD, DEFAULT_ENABLE_LOUDSPEAKER}

#define DEFAULT_CALIB_AUDIO_VOC_TV_FILTERS  { DEFAULT_CALIB_MDF_PROFILE, DEFAULT_CALIB_SDF_PROFILE }


#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE || \
     CHIP_ASIC_ID == CHIP_ASIC_ID_8808)

/// Default audio output gains for handset.
#define DEFAULT_CALIB_AUDIO_OUT_HS  \
 {\
  {  0, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  {  3, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  3, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  {  6, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  6, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  {  9, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  9, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0, 12, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  { 15, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0, 15, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  { 21, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0, 21, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
 }

/// Default audio output gains for ear-piece.
#define DEFAULT_CALIB_AUDIO_OUT_EP  \
 {\
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0,  0, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD-3, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_HEAD-3, 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0,  0, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0 }, \
  {  3, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0,  3, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0 }, \
  {  6, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0,  6, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0 }, \
  {  9, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0,  9, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0, 12, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0 }, \
  { 15, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0, 15, DEFAULT_ABB_DAC_GAIN_HEAD  , 0, 0 }, \
 }

/// Default audio output gains for loud-speaker
#define DEFAULT_CALIB_AUDIO_OUT_LS \
  {\
  {  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  3, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0,  3, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  6, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0,  6, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  9, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0,  9, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0, 12, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_SPEAKER, 3, 0, 15, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_SPEAKER, 6, 0, 21, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
 }

/// Default audio output gains for BT.
#define DEFAULT_CALIB_AUDIO_OUT_BT  \
 {\
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
 }

/// Default audio output gains for FM
#define DEFAULT_CALIB_AUDIO_OUT_FM \
 {\
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  3, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  3, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  6, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  6, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  9, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  9, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 12, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 15, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 15, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 21, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 21, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
 }

/// Default audio output gains for analog TV
#define DEFAULT_CALIB_AUDIO_OUT_TV \
 {\
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  3, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  3, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  6, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  6, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  9, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  9, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 12, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 15, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 15, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 21, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 21, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
 }

#else // 8809 or later

/// Default audio output gains for handset.
#define DEFAULT_CALIB_AUDIO_OUT_HS  \
 {\
  {  0, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  {  3, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  3, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  {  6, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  6, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  {  9, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0,  9, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0, 12, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  { 15, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0, 15, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
  { 18, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0, 21, DEFAULT_ABB_DAC_GAIN_RECEIVER, 0, 0 }, \
 }

/// Default audio output gains for ear-piece.
#define DEFAULT_CALIB_AUDIO_OUT_EP  \
 {\
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD   , 0, 0,  0, DEFAULT_ABB_DAC_GAIN_HEAD   , 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD-12, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_HEAD-12, 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD-9 , 0, 0,  0, DEFAULT_ABB_DAC_GAIN_HEAD-9 , 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD-6 , 0, 0,  0, DEFAULT_ABB_DAC_GAIN_HEAD-6 , 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD-3 , 0, 0,  0, DEFAULT_ABB_DAC_GAIN_HEAD-3 , 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD   , 0, 0,  0, DEFAULT_ABB_DAC_GAIN_HEAD   , 0, 0 }, \
  {  3, DEFAULT_ABB_DAC_GAIN_HEAD   , 0, 0,  3, DEFAULT_ABB_DAC_GAIN_HEAD   , 0, 0 }, \
  {  6, DEFAULT_ABB_DAC_GAIN_HEAD   , 0, 0,  6, DEFAULT_ABB_DAC_GAIN_HEAD   , 0, 0 }, \
 }

/// Default audio output gains for loud-speaker
#define DEFAULT_CALIB_AUDIO_OUT_LS \
  {\
  {  0, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0,  0, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0 }, \
  { -2, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0, -2, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0 }, \
  { 10, DEFAULT_ABB_DAC_GAIN_SPEAKER-9, 0, 0, 10, DEFAULT_ABB_DAC_GAIN_SPEAKER-6, 0, 0 }, \
  { 10, DEFAULT_ABB_DAC_GAIN_SPEAKER-6, 0, 0, 10, DEFAULT_ABB_DAC_GAIN_SPEAKER-3, 0, 0 }, \
  { 10, DEFAULT_ABB_DAC_GAIN_SPEAKER-3, 0, 0, 10, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0 }, \
  { 10, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0, 16, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0 }, \
  { 16, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0, 19, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0 }, \
  { 19, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0, 21, DEFAULT_ABB_DAC_GAIN_SPEAKER,   0, 0 }, \
 }

/// Default audio output gains for BT.
#define DEFAULT_CALIB_AUDIO_OUT_BT  \
 {\
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  { 0, 0, 0, 0, 0, 0, 0, 0 }, \
 }

/// Default audio output gains for FM
#define DEFAULT_CALIB_AUDIO_OUT_FM \
 {\
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, -2, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  3, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 10, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  6, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 16, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  9, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 19, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 21, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 15, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 22, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 21, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 23, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
 }

/// Default audio output gains for analog TV
#define DEFAULT_CALIB_AUDIO_OUT_TV \
 {\
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0,  0, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  0, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, -2, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  3, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 10, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  6, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 16, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  {  9, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 19, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 12, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 21, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 15, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 22, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
  { 21, DEFAULT_ABB_DAC_GAIN_HEAD, 0, 0, 23, DEFAULT_ABB_DAC_GAIN_SPEAKER, 0, 0 }, \
 }

#endif // 8809 or later


#define SIDE_GAIN CALIB_AUDIO_GAIN_VALUE_MUTE

///  Audio gains, side loop.
#define DEFAULT_CALIB_AUDIO_HS_SIDE   { SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN }
#define DEFAULT_CALIB_AUDIO_EP_SIDE   { SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN }
#define DEFAULT_CALIB_AUDIO_LS_SIDE   { SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN }
#define DEFAULT_CALIB_AUDIO_BT_SIDE   { SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN }
#define DEFAULT_CALIB_AUDIO_FM_SIDE   { SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN }
#define DEFAULT_CALIB_AUDIO_TV_SIDE   { SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN, SIDE_GAIN }



///  Microphone gains parameters
///  { analog gain, ADC gain, algorithm gain }
#define MIC_GAINS_CALL_HS           { 18, DEFAULT_ABB_ADC_GAIN, 6 }

#define MIC_GAINS_CALL_EP           { 18, DEFAULT_ABB_ADC_GAIN, 6 }

#define MIC_GAINS_CALL_LS           { 18, DEFAULT_ABB_ADC_GAIN, 6 }

#define MIC_GAINS_CALL_BT           {  6, DEFAULT_ABB_ADC_GAIN, 3 }

#define MIC_GAINS_CALL_FM           {  6, DEFAULT_ABB_ADC_GAIN, 3 }

#define MIC_GAINS_CALL_TV           {  6, DEFAULT_ABB_ADC_GAIN, 3 }

#define MIC_GAINS_RECORD_HS         { 18, DEFAULT_ABB_ADC_GAIN, 6 }

#define MIC_GAINS_RECORD_EP         { 18, DEFAULT_ABB_ADC_GAIN, 6 }

#define MIC_GAINS_RECORD_LS         { 18, DEFAULT_ABB_ADC_GAIN, 6 }

#define MIC_GAINS_RECORD_BT         {  6, DEFAULT_ABB_ADC_GAIN, 3 }

#define MIC_GAINS_RECORD_FM         {  6, DEFAULT_ABB_ADC_GAIN, 3 }

#define MIC_GAINS_RECORD_TV         {  6, DEFAULT_ABB_ADC_GAIN, 3 }



///  Audio gains parameters, HS
#define DEFAULT_CALIB_AUDIO_GAINS_HS  { MIC_GAINS_CALL_HS, DEFAULT_CALIB_AUDIO_OUT_HS, DEFAULT_CALIB_AUDIO_HS_SIDE, MIC_GAINS_RECORD_HS, }

///  Audio gains parameters, EP
#define DEFAULT_CALIB_AUDIO_GAINS_EP  { MIC_GAINS_CALL_EP, DEFAULT_CALIB_AUDIO_OUT_EP, DEFAULT_CALIB_AUDIO_EP_SIDE, MIC_GAINS_RECORD_EP, }

///  Audio gains parameters, LS
#define DEFAULT_CALIB_AUDIO_GAINS_LS  { MIC_GAINS_CALL_LS, DEFAULT_CALIB_AUDIO_OUT_LS, DEFAULT_CALIB_AUDIO_LS_SIDE, MIC_GAINS_RECORD_LS, }

///  Audio gains parameters, BT
#define DEFAULT_CALIB_AUDIO_GAINS_BT  { MIC_GAINS_CALL_BT, DEFAULT_CALIB_AUDIO_OUT_BT, DEFAULT_CALIB_AUDIO_BT_SIDE, MIC_GAINS_RECORD_BT, }

///  Audio gains parameters, FM
#define DEFAULT_CALIB_AUDIO_GAINS_FM  { MIC_GAINS_CALL_FM, DEFAULT_CALIB_AUDIO_OUT_FM, DEFAULT_CALIB_AUDIO_FM_SIDE, MIC_GAINS_RECORD_FM, }

///  Audio gains parameters, TV
#define DEFAULT_CALIB_AUDIO_GAINS_TV  { MIC_GAINS_CALL_TV, DEFAULT_CALIB_AUDIO_OUT_TV, DEFAULT_CALIB_AUDIO_TV_SIDE, MIC_GAINS_RECORD_TV, }




#endif //  _CALIBP_AUDIO_DEFAULTS_H_