#/************************************************************************

  IMPORTANT NOTE : this file contains two clearly delimited sections :
  the ARCHITECTURE section (in two parts) and the USER section. Each section
  is governed by its own copyright and license. Please check individually
  each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************

  Bela FAUST Architecture file
  Oli Larkin 2016
  www.olilarkin.co.uk
  Based on owl.cpp
  

  FAUST Architecture File
  Copyright (C) 2003-2014 GRAME, Centre National de Creation Musicale
  ---------------------------------------------------------------------
  This Architecture section is free software; you can redistribute it
  and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 3 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; If not, see <http://www.gnu.org/licenses/>.

  EXCEPTION : As a special exception, you may create a larger work
  that contains this FAUST architecture section and distribute
  that work under terms of your choice, so long as this FAUST
  architecture section is not modified.


 ************************************************************************
 ************************************************************************/

#ifndef __FaustBela_H__
#define __FaustBela_H__

#include <cstddef>
#include <string>
#include <math.h>
#include <strings.h>
#include <assert.h>
#include <cstdlib>
#include <BeagleRT.h>
#include <Utilities.h>

using namespace std;

#ifndef __FaustCommonInfrastructure__
#define __FaustCommonInfrastructure__

#include "faust/dsp/dsp.h"
#include "faust/gui/UI.h"

const char * const pinNamesStrings[] =
{
  "ANALOG_0",
  "ANALOG_1",
  "ANALOG_2",
  "ANALOG_3",
  "ANALOG_4",
  "ANALOG_5",
  "ANALOG_6",
  "ANALOG_7",
  "ANALOG_8",
  "DIGITAL_0",
  "DIGITAL_1",
  "DIGITAL_2",
  "DIGITAL_3",
  "DIGITAL_4",
  "DIGITAL_5",
  "DIGITAL_6",
  "DIGITAL_7",
  "DIGITAL_8",
  "DIGITAL_9",
  "DIGITAL_10",
  "DIGITAL_11",
  "DIGITAL_12",
  "DIGITAL_13",
  "DIGITAL_14",
  "DIGITAL_15"
 };
 
enum EInputPin
{
  kNoPin = -1,
  kANALOG_0 = 0,
  kANALOG_1,
  kANALOG_2,
  kANALOG_3,
  kANALOG_4,
  kANALOG_5,
  kANALOG_6,
  kANALOG_7,
  kANALOG_8,
  kDIGITAL_0,
  kDIGITAL_1,
  kDIGITAL_2,
  kDIGITAL_3,
  kDIGITAL_4,
  kDIGITAL_5,
  kDIGITAL_6,
  kDIGITAL_7,
  kDIGITAL_8,
  kDIGITAL_9,
  kDIGITAL_10,
  kDIGITAL_11,
  kDIGITAL_12,
  kDIGITAL_13,
  kDIGITAL_14,
  kDIGITAL_15,
  kNumInputPins
 };

struct Meta
{
  virtual void declare(const char* key, const char* value) = 0;
};

/**************************************************************************************

  BelaWidget : object used by BelaUI to ensures the connection between an Bela parameter 
  and a faust widget
  
***************************************************************************************/

class BelaWidget
{
protected:
  EInputPin fBelaPin; 
  FAUSTFLOAT* fZone;  // Faust widget zone
  const char* fLabel; // Faust widget label 
  float fMin;         // Faust widget minimal value
  float fRange;       // Faust widget value range (max-min)
  
  public:
  BelaWidget() 
  : fBelaPin(kNoPin)
  , fZone(0)
  , fLabel("")
  , fMin(0)
  , fRange(1)
  {}
    
  BelaWidget(const BelaWidget& w)
  : fBelaPin(w.fBelaPin)
  , fZone(w.fZone)
  , fLabel(w.fLabel)
  , fMin(w.fMin)
  , fRange(w.fRange)
  {}
    
  BelaWidget(EInputPin pin, FAUSTFLOAT* z, const char* l, float lo, float hi)
  : fBelaPin(pin)
  , fZone(z)
  , fLabel(l)
  , fMin(lo)
  , fRange(hi-lo) 
  {}
    
  void update(BeagleRTContext *context) 
  { 
    switch(fBelaPin)
    {
      case kANALOG_0:
      case kANALOG_1:
      case kANALOG_2:
      case kANALOG_3:
      case kANALOG_4:
      case kANALOG_5:
      case kANALOG_6:
      case kANALOG_7:
        *fZone = fMin + fRange * analogReadFrame(context, 0 /* TODO: average frame?*/, (int) fBelaPin);
                  rt_printf("pin %f\n", *fZone );

        break;
      case kDIGITAL_0:
      case kDIGITAL_1:
      case kDIGITAL_2:
      case kDIGITAL_3:
      case kDIGITAL_4:
      case kDIGITAL_5:
      case kDIGITAL_6:
      case kDIGITAL_7:
      case kDIGITAL_8:
      case kDIGITAL_9:
      case kDIGITAL_10:
      case kDIGITAL_11:
      case kDIGITAL_12:
      case kDIGITAL_13:
      case kDIGITAL_14:
      case kDIGITAL_15:
        *fZone = digitalReadFrame(context, 0 /* TODO: average frame?*/, ((int) fBelaPin - kDIGITAL_0)) > 0 ? fMin : fMin+fRange;
        break;
      default:
      break;
    };
  }
  
};

/**************************************************************************************

  BelaUI : Faust User Interface builder. Passed to buildUserInterface BelaUI allows
  the mapping between BELA pins and faust widgets. It relies on specific
  metadata "...[BELA:DIGITAL_0]..." in the widget's label for that. For example any 
  faust widget with metadata [BELA:DIGITAL_0] will be controlled by DIGITAL_0 
  (the second knob).
  
***************************************************************************************/

// The maximun number of mappings between Bela parameters and faust widgets 
#define MAXBELAWIDGETS 8

class BelaUI : public UI
{
  int fIndex; // number of BelaWidgets collected so far
  EInputPin fBelaPin; // current pin id
  BelaWidget fTable[MAXBELAWIDGETS]; // kind of static list of BelaWidgets
  
  // check if the widget is linked to a Bela parameter and, if so, add the corresponding BelaWidget
  void addBelaWidget(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) 
  {
    if (fBelaPin != kNoPin && (fIndex < MAXBELAWIDGETS))
    {
      fTable[fIndex] = BelaWidget(fBelaPin, zone, label, lo, hi);
      fIndex++;
    }
    fBelaPin = kNoPin;
  }

  // we dont want to create a widget but we clear fBelaPin just in case
  void skip() {
    fBelaPin = kNoPin;
  }

 public:

  BelaUI() 
  : fIndex(0) 
  , fBelaPin(kNoPin)
  {}
  
  virtual ~BelaUI() {}
  
  // should be called before compute() to update widget's zones registered as Bela parameters
  void update(BeagleRTContext *context)
  {
    for (int i=0; i<fIndex; i++)
      fTable[i].update(context);
  }

  //---------------------- virtual methods called by buildUserInterface ----------------
  
  // -- widget's layouts
  virtual void openTabBox(const char* label) {}
  virtual void openHorizontalBox(const char* label) {}
  virtual void openVerticalBox(const char* label) {}
  virtual void closeBox() {}

  // -- active widgets
  virtual void addButton(const char* label, FAUSTFLOAT* zone) { skip(); }
  virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) { skip(); }
  virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) { addBelaWidget(label, zone, lo, hi); }
  virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) { addBelaWidget(label, zone, lo, hi); }
  virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) { addBelaWidget(label, zone, lo, hi); }

  // -- passive widgets
  virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) { skip(); }
  virtual void addVerticalBargraph  (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) { skip(); }

// -- metadata declarations
  virtual void declare(FAUSTFLOAT* z, const char* k, const char* id) {
    if (strcasecmp(k,"BELA") == 0) {
      for(int i=0;i<kNumInputPins;i++) {
        if (strcasecmp(id, pinNamesStrings[i]) == 0) {
          fBelaPin = (EInputPin) i;
        }
      }
    }
  }
};

#endif // __FaustCommonInfrastructure__

/**************************BEGIN USER SECTION **************************/

<<includeIntrinsic>>

<<includeclass>>

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/


/**************************************************************************************

  Bela render.cpp that calls FAUST generated code
  
***************************************************************************************/

unsigned int gNumBuffers = 0; // the number of de-interleaved buffers for audio and analog i/o
float *gInputBuffers = NULL, *gOutputBuffers = NULL; //de-interleaved input/output buffers for the audio and analog inputs
mydsp fDSP;
BelaUI fUI;

//render.cpp
bool setup(BeagleRTContext *context, void *userData)
{
  gNumBuffers = context->audioChannels + context->analogChannels;
//   rt_printf("context->audioFrames %i\n", context->audioFrames);
//   rt_printf("context->audioChannels %i\n", context->audioChannels);
//   rt_printf("context->analogChannels %i\n", context->analogChannels);
//   rt_printf("context->analogFrames %i\n", context->analogFrames);

  fDSP.init(context->audioSampleRate);
  fDSP.buildUserInterface(&fUI); // Maps Bela Analog/Digital IO and faust widgets 

  gInputBuffers = (float *) malloc(gNumBuffers * context->audioFrames * sizeof(float));
  gOutputBuffers = (float *) malloc(gNumBuffers * context->audioFrames * sizeof(float));
  
  return true;
}

void render(BeagleRTContext *context, void *userData)
{

  // De-interleave the input data
  if(gInputBuffers != NULL)
  {
    for(unsigned int frame = 0; frame < context->audioFrames; frame++)
    {
      for(unsigned int ch = 0; ch < gNumBuffers; ch++)
      {
        if(ch >= context->audioChannels+context->analogChannels)
          break;
        if(ch >= context->audioChannels) // handle analogChannels
        {
          unsigned int m = frame/2;
          float mIn = (float) context->audioIn[m * context->analogChannels + (ch-context->audioChannels)];
          gInputBuffers[ch * context->audioFrames + frame] = mIn;
        }
        else // handle audioChannels
        {
          gInputBuffers[ch * context->audioFrames + frame] = context->audioIn[frame * context->audioChannels + ch];
        }
      }
    }
  }

  float*  ins[32];
  float*  outs[32];

  assert(fDSP.getNumInputs() < 32);
  assert(fDSP.getNumOutputs() < 32);

  // create the table of input channels
  for(int ch=0; ch<fDSP.getNumInputs(); ++ch)
    ins[ch] = gInputBuffers + (ch * context->audioFrames);
  
  // create the table of output channels
  for(int ch=0; ch<fDSP.getNumOutputs(); ++ch)
    outs[ch] = gOutputBuffers + (ch * context->audioFrames);
    
  // reads Bela pins and updates corresponding Faust Widgets zones
  fUI.update(context); 
  // Process FAUST DSP
  fDSP.compute(context->audioFrames, ins, outs);
  
  // Interleave the output data
  if(gOutputBuffers != NULL)
  {
    for(unsigned int frame = 0; frame < context->audioFrames; frame++)
    {
      for(unsigned int ch = 0; ch < gNumBuffers; ch++)
      {
        if(ch >= context->audioChannels+context->analogChannels) 
          break;
        else 
        {
          if(ch >= context->audioChannels) // handle analogChannels
          {
            unsigned int m = frame/2;
            context->analogOut[m * context->analogFrames + (ch-context->audioChannels)] = constrain(gOutputBuffers[ch*context->audioFrames + frame], 0.0f, 1.0f );
          } 
          else // handle audioChannels
          {
            context->audioOut[frame * context->audioChannels + ch] = gOutputBuffers[ch * context->audioFrames + frame];
          }
        }
      }
    }
  }
}

void cleanup(BeagleRTContext *context, void *userData)
{
}



#endif // __FaustBela_H__


////////////////////////////////////////////////////////////////////////////////////////////////////
