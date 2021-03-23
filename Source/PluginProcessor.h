/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EnvelopeShaper.h"
#include "EnvelopeShaper2.h"
#include "EQ.h"

#define AMOUNT_ID "amount"
#define AMOUNT_NAME "Amount"

#define X2_ID "2x"
#define X2_NAME "2X"

#define FOCUS_ID "focus"
#define FOCUS_NAME "Focus"

#define RELEASE_ID "release (ms)"
#define RELEASE_NAME "Release (ms)"

#define ATTACK_ID "attack (ms)"
#define ATTACK_NAME "Attack (ms)"

#define OUTPUT_ID "output"
#define OUTPUT_NAME "Output"

#define LSF_ID "low-shelf freq"
#define LSF_NAME "Low-Shelf Freq"

#define HSF_ID "high-shelf freq"
#define HSF_NAME "High-Shelf Freq"

#define PEQ1F_ID "low mid freq"
#define PEQ1F_NAME "Low Mid Freq"

#define PEQ2F_ID "high mid freq"
#define PEQ2F_NAME "High Mid Freq"

#define LSFGain_ID "low-shelf gain"
#define LSFGain_NAME "Low-Shelf Gain"

#define HSFGain_ID "high-shelf gain"
#define HSFGain_NAME "High-Shelf Gain"

#define PEQ1Gain_ID "low mid gain"
#define PEQ1Gain_NAME "Low Mid Gain"

#define PEQ2Gain_ID "high mid gain"
#define PEQ2Gain_NAME "High Mid Gain"

#define CLIP_ID "clip"
#define CLIP_NAME "Clip"

#define DW_ID "drywet"
#define DW_NAME "Dry/Wet"

#define OS_ID "oversample"
#define OS_NAME "Oversample"
//==============================================================================
/**
*/
class SpikeQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SpikeQAudioProcessor();
    ~SpikeQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState parameters;
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    int mScale;
    float mAmount, mAttack, mRelease, mOutput, mLsf, mHsf, mPeq1F, mPeq2F, mLsfG, mHsfG, mPeq1G, mPeq2G, mDryWet, mLatency;
    bool mClip, mOs;

private:
    void setParams();
    float clipper(float sample);

    dsp::ProcessSpec spec;
    EnvelopeEQProcess mEnvelopeEQ;
    std::unique_ptr<juce::dsp::Oversampling<float>> mOversample;
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::None> delay{ 44100 };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpikeQAudioProcessor)
};
