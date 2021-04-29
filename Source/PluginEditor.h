/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LWSLookAndFeel.h"
#include "UbuntuFont.h"

//==============================================================================
/**
*/

class SpikeQAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer, public Slider::Listener, public Button::Listener
{
public:
    SpikeQAudioProcessorEditor (SpikeQAudioProcessor&);
    ~SpikeQAudioProcessorEditor() override;

    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void paintOverChildren(Graphics& g) override;
    void sliderValueChanged(Slider* sliderThatHasChanged) override;
    void buttonClicked(Button* buttonThatHasChanged) override;

    void newButtonStyle(Colour textColour, Colour tickColour, Colour disableColour);
    void newSliderStyle(Colour textColour, float trackTransp, Slider::TextEntryBoxPosition textBoxStyle, bool readonly, int width, int height);

    static const char* logoSmall_png;
    static const int logoSmall_pngSize;


    /*Typeface::Ptr getTypefaceForFont(const Font& f)
    {
        static Typeface::Ptr ubuntuReg = Typeface::createSystemTypefaceFor(UbuntuFont::UbuntuRegular_ttf,
            UbuntuFont::UbuntuRegular_ttfSize);
        return ubuntuReg;
    }*/

    Typeface::Ptr customTypeface; 

    Image cachedImage_logo2020_png2_1;

    void paramTextStyle(Graphics& g, String name, float x, float y, float width, float fontHeight, bool drawBox, Justification justification, Colour textColour = Colour(245, 245, 245));

    void defaultSliderBehaviour(Slider& slider);

    int uiSize{ 400 };

    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpikeQAudioProcessor& audioProcessor;

    Font LWSFont;

    LWSLookAndFeel::buttonLookAndFeel buttonLAF;
    LWSLookAndFeel::RotaryLookAndFeel rotaryLAF;

    Slider amountSlider, lowFreqSlider, lowGainSlider, lmFreqSilder, lmGainSlider, hmFreqSlider, hmGainSlider, highFreqSlider, highGainSlider, outputSlider, attackSlider, releaseSlider, dryWetSlider;
    
    
    ToggleButton x2Button, clipButton, osButton, sustainButton;
    std::unique_ptr<HyperlinkButton> help;
    Label amountText;
    //Label amountText, lowGainText, lmGainText, hmGainText, highGainText;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>
        amountSliderValue, lowFreqSliderValue, lowGainSliderValue, lmFreqSilderValue, lmGainSliderValue, hmFreqSliderValue, hmGainSliderValue, highFreqSliderValue, highGainSliderValue, outputSliderValue, attackSliderValue, releaseSliderValue, dryWetSliderValue;

    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> x2ButtonValue, clipButtonValue, osButtonValue, sustainButtonValue;


    Slider uiSizeSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>
        uiSizeSliderValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpikeQAudioProcessorEditor)
};
