#pragma once

/*
HPF, LPF and Peak Filters for sidechain of processor

by Low Wave Studios

*/

#include <JuceHeader.h>

class FourBandEQ 
{
public:

	void initFilters(float samplerate, int numSamples, int numCh)
	{
		mSampleRate = samplerate;
		spec.sampleRate = mSampleRate;
		spec.maximumBlockSize = numSamples;
		spec.numChannels = numCh;

		lowShelfFilter.reset();
		lowShelfFilter.prepare(spec);

		highShelfFilter.reset();
		highShelfFilter.prepare(spec);

		peakFilter1.reset();
		peakFilter1.prepare(spec);

		peakFilter2.reset();
		peakFilter2.prepare(spec);



	}
	void updateCoeffs(float LSF, float HSF, float PeqFreq1, float PeqFreq2, float LSFGain, float HSFGain, float PeqGain1, float PeqGain2)
	{
		mLSF = LSF;
		mHSF = HSF;
		mPeqFreq1 = PeqFreq1;
		mPeqFreq2 = PeqFreq2;
		mLSFGain = LSFGain;
		mHSFGain = HSFGain;
		mPeqGain1 = PeqGain1;
		mPeqGain2 = PeqGain2;

		*lowShelfFilter.state = *dsp::IIR::Coefficients<float>::makeLowShelf(mSampleRate, mLSF, 1.f, Decibels::decibelsToGain(mLSFGain));
		*highShelfFilter.state = *dsp::IIR::Coefficients<float>::makeHighShelf(mSampleRate, mHSF, 1.f, Decibels::decibelsToGain(mHSFGain));
		*peakFilter1.state = *dsp::IIR::Coefficients<float>::makePeakFilter(mSampleRate, mPeqFreq1, 0.5f, Decibels::decibelsToGain(mPeqGain1));
		*peakFilter2.state = *dsp::IIR::Coefficients<float>::makePeakFilter(mSampleRate, mPeqFreq2, 0.5f, Decibels::decibelsToGain(mPeqGain2));

	}

	void process(juce::AudioSampleBuffer& buffer)
	{

		dsp::AudioBlock<float> mBlock(buffer);

		if (mLSFGain != 0)
		{
			lowShelfFilter.process(dsp::ProcessContextReplacing<float>(mBlock));
		}
		if (mHSFGain != 0)
		{
			highShelfFilter.process(dsp::ProcessContextReplacing<float>(mBlock));
		}
		if (mPeqGain1 != 0.f)
		{
			peakFilter1.process(dsp::ProcessContextReplacing<float>(mBlock));
		}
		if (mPeqGain2 != 0.f)
		{
			peakFilter2.process(dsp::ProcessContextReplacing<float>(mBlock));
		}
	}

private:

	float mSampleRate{ 44100 };

	float mLSF{ 50.f };
	float mHSF{ 10000.f };
	float mPeqFreq1{ 250.f };
	float mPeqFreq2{ 2500.f };
	float mLSFGain{ 0.0f };
	float mHSFGain{ 0.0f };
	float mPeqGain1{ 0.0f };
	float mPeqGain2{ 0.0f };

	dsp::ProcessSpec spec;

	dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> lowShelfFilter;
	dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> highShelfFilter;
	dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> peakFilter1;
	dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> peakFilter2;

	//JUCE_LEAK_DETECTOR(SidechainEQ)
};
