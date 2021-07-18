/************************** BEGIN JuceParameterUI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
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
 ************************************************************************/

#ifndef JuceParameterUI_H
#define JuceParameterUI_H

#include "../JuceLibraryCode/JuceHeader.h"

#include "faust/gui/GUI.h"
#include "faust/gui/PathBuilder.h"

// Link AudioParameterBool with on/off parameter

struct FaustPlugInAudioParameterBool : public juce::AudioParameterBool, public uiOwnedItem {
    
    FaustPlugInAudioParameterBool(GUI* gui, FAUSTFLOAT* zone, const std::string& path, const std::string& label)
    :juce::AudioParameterBool(path, label, false), uiOwnedItem(gui, zone)
    {}
    
    virtual ~FaustPlugInAudioParameterBool() {}
    
    void reflectZone() override
    {
        FAUSTFLOAT v = *fZone;
        fCache = v;
        setValueNotifyingHost(float(v));
    }
    
    virtual void setValue (float newValue) override
    {
        modifyZone(FAUSTFLOAT(newValue));
    }
    
};

// Link AudioParameterFloat with range parameters

struct FaustPlugInAudioParameterFloat : public juce::AudioParameterFloat, public uiOwnedItem {
    
    FaustPlugInAudioParameterFloat(GUI* gui, FAUSTFLOAT* zone, const std::string& path, const std::string& label, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    :juce::AudioParameterFloat(path, label, float(min), float(max), float(init)), uiOwnedItem(gui, zone)
    {}
    
    virtual ~FaustPlugInAudioParameterFloat() {}
    
    void reflectZone() override
    {
        FAUSTFLOAT v = *fZone;
        fCache = v;
        if (v >= range.start && v <= range.end) {
            setValueNotifyingHost(range.convertTo0to1(float(v)));
        }
    }
    
    virtual void setValue (float newValue) override
    {
        modifyZone(FAUSTFLOAT(range.convertFrom0to1(newValue)));
    }
    
};

// included as a link to Foley's GUI Magic Combo Boxes from slider[style:menu]
struct FaustPlugInAudioParameterChoice : public juce::AudioParameterChoice, public uiOwnedItem {
    // @param parameterID         The parameter ID to use
    //    @param parameterName       The parameter name to use
    //    @param choices             The set of choices to use
    //    @param defaultItemIndex    The index of the default choice
    FaustPlugInAudioParameterChoice(GUI* gui, FAUSTFLOAT* zone, const std::string& path, const std::string& label, juce::StringArray& options, int init)
        :juce::AudioParameterChoice(path, label, options, int(init)), uiOwnedItem(gui, zone)
    {}

    virtual ~FaustPlugInAudioParameterChoice() {}

    void reflectZone() override
    {
        FAUSTFLOAT v = *fZone;
   //     fCache = v;
   //     if (v >= range.start && v <= range.end) {
   //         setValueNotifyingHost(range.convertTo0to1(float(v)));
   //     }
    }

    virtual void setValue(float newValue) override
    {
  //      modifyZone(FAUSTFLOAT(range.convertFrom0to1(newValue)));
    }

};

// A class to create AudioProcessorParameter objects for each zone
#ifdef MAGIC_COMBO_BOX
class JuceParameterUI : public GUI, public MetaDataUI, public PathBuilder {
#else
class JuceParameterUI : public GUI, public PathBuilder {
#endif


    
    private:
        
        juce::AudioProcessor* fProcessor;
        
    public:
        
        JuceParameterUI(juce::AudioProcessor* processor):fProcessor(processor)
        {}
    
        virtual ~JuceParameterUI() {}
        
        // -- widget's layouts
        
        virtual void openTabBox(const char* label)
        {
            pushLabel(label);
        }
        virtual void openHorizontalBox(const char* label)
        {
            pushLabel(label);
        }
        virtual void openVerticalBox(const char* label)
        {
            pushLabel(label);
        }
        virtual void closeBox()
        {
            popLabel();
        }
        
        // -- active widgets
        
        virtual void addButton(const char* label, FAUSTFLOAT* zone)
        {
            fProcessor->addParameter(new FaustPlugInAudioParameterBool(this, zone, buildPath(label), label));
        }
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            fProcessor->addParameter(new FaustPlugInAudioParameterBool(this, zone, buildPath(label), label));
        }
        
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            if (isMenu(zone)) { 
#ifdef MAGIC_COMBO_BOX
                // get the Faust style:menu string and pull off the labels into a string array.
                // the values will be ignored by PGM.  In use it will generate
				// values 0 to n - 1 corresponding to the keys in order.
                std::string poobert = fMenuDescription[zone];
                std::size_t beginQuote = poobert.find("\'");
                std::size_t endQuote = poobert.find("\'", beginQuote + 1);
                std::string menuOption = poobert.substr(beginQuote + 1, endQuote - beginQuote - 1);
                juce::StringArray selectionList(menuOption);

                while (TRUE)
                {
                    beginQuote = poobert.find("\'", endQuote + 1);
                    if (beginQuote == std::string::npos)
                        break;
                    endQuote = poobert.find("\'", beginQuote + 1);
                    if (endQuote == std::string::npos)
                        break;
                    menuOption = poobert.substr(beginQuote + 1, endQuote - beginQuote - 1);
                    selectionList.add(menuOption);
                }

                fProcessor->addParameter(new FaustPlugInAudioParameterChoice(this, zone, buildPath(label), label, selectionList, init));
//                fProcessor->addParameterListener(label, this);
#else
                fProcessor->addParameter(new FaustPlugInAudioParameterFloat(this, zone, buildPath(label), label, init, min, max, step));
#endif
            }
            else
            {
                fProcessor->addParameter(new FaustPlugInAudioParameterFloat(this, zone, buildPath(label), label, init, min, max, step));
            };
        }
        
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            fProcessor->addParameter(new FaustPlugInAudioParameterFloat(this, zone, buildPath(label), label, init, min, max, step));
        }
        
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            fProcessor->addParameter(new FaustPlugInAudioParameterFloat(this, zone, buildPath(label), label, init, min, max, step));
        }
        
        // -- passive widgets
        
        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
        {
            fProcessor->addParameter(new FaustPlugInAudioParameterFloat(this, zone, buildPath(label), label, 0, min, max, 0));
        }
        
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
        {
            fProcessor->addParameter(new FaustPlugInAudioParameterFloat(this, zone, buildPath(label), label, 0, min, max, 0));
        }
		
#ifdef MAGIC_COMBO_BOX
        /** Declare a metadata. */
        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* value) override
        {
            MetaDataUI::declare(zone, key, value);
        }
#endif     		
    
};

#endif
/**************************  END  JuceParameterUI.h **************************/
