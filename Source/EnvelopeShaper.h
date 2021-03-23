#pragma once

#include "AudioDevEffect.h"

#include <cmath>
#include <limits>
#include <JuceHeader.h>
#include "EQ.h"

class EnvelopeShaperFast : public AudioDevEffect
{
public:
    // callback for initializing with samplerate
    void prepareForPlayback(float samplerate) override
    {
        m_Samplerate = samplerate;

        update();
    }
    
    // callback for audio processing
    void processAudioSample(float & sample) override
    {
        //int nAttks = (m_Samplerate * (10 * 0.001));
        
        if (sample > m_Envelope)
        {
            m_Envelope += m_Attack * (sample - m_Envelope);
        }
        else if (sample <= m_Envelope)
        {
            m_Envelope += m_Release * (sample - m_Envelope);
        }
        sample = m_Envelope;
    }
    
    // setters for compression parameters
    void setAttack(float attack)
    {
        m_AttackInMilliseconds = attack;
        update();
    }
    
    void setRelease(float release)
    {
        m_ReleaseInMilliseconds = release;
        update();
    }
    
private:
    // envelope shaper private variables and functions
    float m_Envelope {0.0f};
    float m_Samplerate{ 44100 };
    float m_AttackInMilliseconds{ 0.0f };
    float m_ReleaseInMilliseconds{ 150.0f };
    float m_Attack{ 0.0f };
    float m_Release{ 0.0f };


    
    // update attack and release scaling factors
    void update()
    {
        m_Attack = calculate(m_AttackInMilliseconds);
        m_Release = calculateExp(m_ReleaseInMilliseconds);
    }
    
    // calculate scaling factor from a value in milliseconds
    float calculate(float time)
    {
        if (time <= 0.f || m_Samplerate <= 0.f)
        {
            return 1.f;
        }
        return 1.f - exp(-1.f / (time * 0.001f * m_Samplerate));
    }

    float calculateExp(float time)
    {
        if (time <= 0.f || m_Samplerate <= 0.f)
        {
            return 1.f;
        }
        return 1.f - exp(-1.f / (time * 0.001f * m_Samplerate));
        //return 1.f - pow(20.0, (-1.f / (time * 0.001f * m_Samplerate)));
    }
}; 

class EnvelopeShaperSlow : public AudioDevEffect
{
public:
    // callback for initializing with samplerate
    void prepareForPlayback(float samplerate) override
    {
        m_Samplerate = samplerate;

        update();
    }

    // callback for audio processing
    void processAudioSample(float& sample) override
    {
        //int nAttks = (m_Samplerate * (10 * 0.001));

        if (sample > m_Envelope)
        {
            m_Envelope += m_Attack * (sample - m_Envelope);
        }
        else if (sample <= m_Envelope)
        {
            m_Envelope += m_Release * (sample - m_Envelope);
        }
        sample = m_Envelope;
    }

    // setters for compression parameters
    void setAttack(float attack)
    {
        m_AttackInMilliseconds = attack;
        update();
    }

    void setRelease(float release)
    {
        m_ReleaseInMilliseconds = release;
        update();
    }

private:
    // envelope shaper private variables and functions
    float m_Envelope{ 0.0f };
    float m_Samplerate{ 44100 };
    float m_AttackInMilliseconds{ 20.0f };
    float m_ReleaseInMilliseconds{ 150.0f };
    float m_Attack{ 0.0f };
    float m_Release{ 0.0f };
    double holdcount{ 0.0 };

    // update attack and release scaling factors
    void update()
    {
        m_Attack = calculateExp(m_AttackInMilliseconds);
        m_Release = calculateExp(m_ReleaseInMilliseconds);
    }

    // calculate scaling factor from a value in milliseconds
    float calculate(float time)
    {
        if (time <= 0.f || m_Samplerate <= 0.f)
        {
            return 1.f;
        }
        return 1.f - exp(-1.f / (time * 0.001f * m_Samplerate));
        //return 1.f - pow(20.0, (-1.f / (time * 0.001f * m_Samplerate)));
        //return 1.f - log10l(-1.f / (time * 0.001f * m_Samplerate)); 

    }

    float calculateExp(float time)
    {
        if (time <= 0.f || m_Samplerate <= 0.f)
        {
            return 1.f;
        }
        return 1.f - exp(-1.f / (time * 0.001f * m_Samplerate));
        //return 1.f - pow(20.0, (-1.f / (time * 0.001f * m_Samplerate)));
    }
};

class EnvelopeEQProcess
{
public:
    void prepareEnvelope(double sampleRate, int numSamples, int numCh)
    {
        bufferNumSamples = numSamples;
        mEnvelopeFast.prepareForPlayback(sampleRate);
        mEnvelopeSlow.prepareForPlayback(sampleRate);
        
        totalNumInputChannels = numCh;

        mEq.initFilters(sampleRate, numSamples, numCh);
        
    }

    void setParams(float release, float attack, float snap)
    {
        mEnvelopeSlow.setAttack(attack);
        mEnvelopeSlow.setRelease(release);

        mEnvelopeFast.setAttack(0.f);
        mEnvelopeFast.setRelease(release);

        mSnap = snap;

    }
    void processBuffer(AudioSampleBuffer& buffer, float LSF, float HSF, float PeqFreq1, float PeqFreq2, float LSFGain, float HSFGain, float PeqGain1, float PeqGain2, int mScale)
    {
        mEq.updateCoeffs(LSF, HSF, PeqFreq1, PeqFreq2, LSFGain, HSFGain, PeqGain1, PeqGain2);
        bufferNumSamples = buffer.getNumSamples();
        mRectified.clear();
        mRectified.makeCopyOf(buffer);

        //Rectify copy of buffer for envelope
        {
            float* w = mRectified.getWritePointer(0);
            if (totalNumInputChannels > 1)
            {
                for (int i = 0; i < bufferNumSamples; i++)
                {
                    w[i] += buffer.getSample(1, i);
                    w[i] /= 2.f;
                }
            }

            for (int i = 0; i < bufferNumSamples; i++)
            {
                w[i] = fabsf(w[i]);
            }
        }

        mRectified.setSize(1, bufferNumSamples, true, true, true);
        //create 2 envelopes
        mRectified2.clear();
        mRectified2.makeCopyOf(mRectified);
        {
            float* wRect = mRectified.getWritePointer(0);
            float* wRect2 = mRectified2.getWritePointer(0);

            for (int i = 0; i < bufferNumSamples; i++)
            {
                //Envelope 1
                float sRect = mRectified.getSample(0, i);
                mEnvelopeFast.processAudioSample(sRect);
                wRect[i] = sRect;

                //Envelope 2
                float sRect2 = mRectified2.getSample(0, i);
                mEnvelopeSlow.processAudioSample(sRect2);
                wRect2[i] = sRect2;
            }

            //apply focus
            mRectified2.applyGain(1.5f);

            //avoid envelope to go below zero when inceasing snap and above 1 when decreasing
            for (int i = 0; i < bufferNumSamples; i++)
            {
                wRect[i] -= mRectified2.getSample(0, i);
                if (mRectified.getSample(0, i) < 0 && mSnap >= 0)
                {
                    wRect[i] = 0;
                }
                else if (mRectified.getSample(0, i) > 0 && mSnap < 0)
                {
                    wRect[i] = 1;
                }
            }
            //apply gain from envelope
            int scale;
            mScale == true ? scale = 2 : scale = 1;
            mRectified.applyGain(((mSnap * 0.5) * scale));
        }

        AudioSampleBuffer bufferEQ;
        bufferEQ.makeCopyOf(buffer);

        mEq.process(bufferEQ); //EQ
        

        //apply rectified envelope to bufferEQ
        for (int ch = 0; ch < totalNumInputChannels; ch++)
        {
            float* w = buffer.getWritePointer(ch);

            for (int i = 0; i < bufferNumSamples; i++)
            {
                float sClean = bufferEQ.getSample(ch, i);
                float sRect = mRectified.getSample(0, i);

                w[i] += (((sClean * powf(2.f, sRect)) - sClean) * 2);
            }
        }

    }


private:

    float mSnap{ 1.f };
    int totalNumInputChannels;
    int bufferNumSamples;
    AudioSampleBuffer mRectified, mRectified2;
    EnvelopeShaperFast mEnvelopeFast;
    EnvelopeShaperSlow mEnvelopeSlow;
    FourBandEQ mEq;
};