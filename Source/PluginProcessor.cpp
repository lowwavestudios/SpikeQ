/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpikeQAudioProcessor::SpikeQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, "PARAMETER", createParameterLayout())
#endif
{
}

SpikeQAudioProcessor::~SpikeQAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout SpikeQAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr <RangedAudioParameter>> params;

    auto amountParam = std::make_unique <AudioParameterFloat>(AMOUNT_ID, AMOUNT_NAME, 0.0f, 5.f, 0.0f);
    auto scaleParam = std::make_unique <AudioParameterBool>(X2_ID, X2_NAME, false);
    auto attackParam = std::make_unique <AudioParameterInt>(ATTACK_ID, ATTACK_NAME, 0, 150, 20);
    auto releaseParam = std::make_unique <AudioParameterInt>(RELEASE_ID, RELEASE_NAME, 5, 1000, 150);
    auto outputParam = std::make_unique <AudioParameterFloat>(OUTPUT_ID, OUTPUT_NAME, -24.0f, 12.0f, 0.0f);
    auto lsfParam = std::make_unique <AudioParameterInt>(LSF_ID, LSF_NAME, 20, 400, 50);
    auto hsfParam = std::make_unique <AudioParameterInt>(HSF_ID, HSF_NAME, 1500, 20000, 10000);
    auto peq1fParam = std::make_unique <AudioParameterInt>(PEQ1F_ID, PEQ1F_NAME, 20, 1500, 150);
    auto peq2fParam = std::make_unique <AudioParameterInt>(PEQ2F_ID, PEQ2F_NAME, 650, 10000, 3000);
    auto lsfGainParam = std::make_unique <AudioParameterFloat>(LSFGain_ID, LSFGain_NAME, -24.f, 24.f, 0.f);
    auto hsfGainParam = std::make_unique <AudioParameterFloat>(HSFGain_ID, HSFGain_NAME, -24.f, 24.f, 0.f);
    auto peq1GainParam = std::make_unique <AudioParameterFloat>(PEQ1Gain_ID, PEQ1Gain_NAME, -24.f, 24.f, 0.f);
    auto peq2GainParam = std::make_unique <AudioParameterFloat>(PEQ2Gain_ID, PEQ2Gain_NAME, -24.f, 24.f, 0.f);
    auto clipParam = std::make_unique <AudioParameterBool>(CLIP_ID, CLIP_NAME, true);
    auto osParam = std::make_unique <AudioParameterBool>(OS_ID, OS_NAME, false);
    auto dryWetParam = std::make_unique <AudioParameterInt>(DW_ID, DW_NAME, 0, 100, 100);
    auto uiSizeParam = std::make_unique <AudioParameterInt>(UISIZE_ID, UISIZE_NAME, 200, 1200, 400);
    auto invertParam = std::make_unique <AudioParameterBool>(INVERT_ID, INVERT_NAME, false);

    params.push_back(std::move(amountParam));
    params.push_back(std::move(scaleParam));
    params.push_back(std::move(attackParam));
    params.push_back(std::move(releaseParam));
    params.push_back(std::move(outputParam));
    params.push_back(std::move(lsfParam));
    params.push_back(std::move(hsfParam));
    params.push_back(std::move(peq1fParam));
    params.push_back(std::move(peq2fParam));
    params.push_back(std::move(lsfGainParam));
    params.push_back(std::move(hsfGainParam));
    params.push_back(std::move(peq1GainParam));
    params.push_back(std::move(peq2GainParam));
    params.push_back(std::move(clipParam));
    params.push_back(std::move(osParam));
    params.push_back(std::move(dryWetParam));
    params.push_back(std::move(uiSizeParam));
    params.push_back(std::move(invertParam));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String SpikeQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpikeQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpikeQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpikeQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpikeQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpikeQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SpikeQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpikeQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpikeQAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpikeQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void SpikeQAudioProcessor::setParams()
{
    mAmount = (*parameters.getRawParameterValue(AMOUNT_ID));
    mScale = (*parameters.getRawParameterValue(X2_ID));
    mAttack = (*parameters.getRawParameterValue(ATTACK_ID));
    mRelease = (*parameters.getRawParameterValue(RELEASE_ID));
    mOutput = (*parameters.getRawParameterValue(OUTPUT_ID));
    mDryWet = (*parameters.getRawParameterValue(DW_ID));

    mLsf = (*parameters.getRawParameterValue(LSF_ID));
    mHsf = (*parameters.getRawParameterValue(HSF_ID));
    mPeq1F = (*parameters.getRawParameterValue(PEQ1F_ID));
    mPeq2F = (*parameters.getRawParameterValue(PEQ2F_ID));

    mLsfG = (*parameters.getRawParameterValue(LSFGain_ID));
    mHsfG = (*parameters.getRawParameterValue(HSFGain_ID));
    mPeq1G = (*parameters.getRawParameterValue(PEQ1Gain_ID));
    mPeq2G = (*parameters.getRawParameterValue(PEQ2Gain_ID));
    mClip = (*parameters.getRawParameterValue(CLIP_ID));
    mOs = (*parameters.getRawParameterValue(OS_ID));
    mInvert = (*parameters.getRawParameterValue(INVERT_ID));

    mEnvelopeEQ.setParams(mRelease, mAttack, mAmount, mInvert);
}
//==============================================================================
void SpikeQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    spec.sampleRate = sampleRate;

    mEnvelopeEQ.prepareEnvelope(sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    mOversample.reset(new juce::dsp::Oversampling<float>(getTotalNumOutputChannels(), 2, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple, true, true));
    mOversample->initProcessing(static_cast<size_t>(samplesPerBlock));

    delay.reset();
    delay.prepare(spec);

    setParams();
}

void SpikeQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpikeQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SpikeQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    setParams();

    //prep latency
    mOversample->setUsingIntegerLatency(true);
    mLatency = static_cast <int>(mOversample->getLatencyInSamples());

    if (mOs == true && mClip == true)
    {
        setLatencySamples(mLatency);
    }
    else
    {
        setLatencySamples(0);
    }

    //prepare dry buffer
    delay.setDelay(mLatency);

    AudioSampleBuffer dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    mEnvelopeEQ.processBuffer(buffer, mLsf, mHsf, mPeq1F, mPeq2F, mLsfG, mHsfG, mPeq1G, mPeq2G, mScale);

    buffer.applyGain(Decibels::decibelsToGain(mOutput));

    if (mOs == true && mClip == true)
    {
        //prepare for oversampling
        dsp::AudioBlock<float> processBlock(buffer);
        dsp::AudioBlock<float> highSampleRateBlock = mOversample->processSamplesUp(processBlock);

        //Clipper
        for (int ch = 0; ch < highSampleRateBlock.getNumChannels(); ch++)
        {
            float* w = highSampleRateBlock.getChannelPointer(ch);
            for (int i = 0; i < highSampleRateBlock.getNumSamples(); i++)
            {
                w[i] = clipper(highSampleRateBlock.getSample(ch, i));
            }
        }

        mOversample->processSamplesDown(processBlock);

        //delay dry buffer
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* data = dryBuffer.getWritePointer(channel);

            for (int i = 0; i < buffer.getNumSamples(); i++)
            {
                delay.pushSample(channel, data[i]);
                data[i] = delay.popSample(channel, mLatency);
            }
        }
    }

    //clipper
    if (mClip == true && mOs == false)
    {
        for (int ch = 0; ch < totalNumOutputChannels; ch++)
        {
            float* w = buffer.getWritePointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); i++)
            {
                float s = buffer.getSample(ch, i);
                w[i] = clipper(s);
            }
        }
    }
    
    //Dry Wet
    for (int ch = 0; ch < buffer.getNumChannels(); ch++)
    {
        float* w = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            float dryS = dryBuffer.getSample(ch, i);
            float wetS = buffer.getSample(ch, i);

            w[i] = (wetS * (mDryWet / 100)) + (dryS * (-(mDryWet / 100) + 1));
        }
    }
}

float SpikeQAudioProcessor::clipper(float sample)
{
    float x = sample;
    x = jlimit<float>(-0.99f, 0.99f, tanhf(powf(x, 5) + x) * 0.99f);
    
    return x;
}

//==============================================================================
bool SpikeQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpikeQAudioProcessor::createEditor()
{
    return new SpikeQAudioProcessorEditor (*this);
}

//==============================================================================
void SpikeQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    MemoryOutputStream stream(destData, false);
    parameters.state.writeToStream(stream);
}

void SpikeQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ValueTree tree = ValueTree::readFromData(data, size_t(sizeInBytes));
    if (tree.isValid())
        parameters.state = tree;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpikeQAudioProcessor();
}
