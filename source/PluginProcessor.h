#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
//#include <torch/torch.h>
#include <torch/script.h>

#if (MSVC)
#include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    bool getAudioBuffer(juce::AudioBuffer<float>& buffer);
    
    void loadModel(const void* modelData, size_t modelSize);
    
    //juce::ValueTree& getState() { return state; }
    
    //NN
    bool nnOn;
    float nnMix;
    
    //GATE
    bool gateOn;
    float gateThreshold;
    float gateAttack = 10.0f; // in milliseconds
    float gateRelease = 100.0f; // in milliseconds
    float gateHold = 50.0f; // in milliseconds
    float gateEnvelope = 0.0f; // Envelope value
    
    //GAIN
    float gain;
    
    //WAVES
    juce::AudioVisualiserComponent unprocessedWaveViewer;
    juce::AudioVisualiserComponent processedWaveViewer;
    
    //juce::AudioProcessorValueTreeState parameters;
    //static constexpr const char* paramNNOn = "nnOn";
    //static constexpr const char* paramNNMix = "nnMix";
    //static constexpr const char* paramChosenModel = "chosenModel";
    //static constexpr const char* paramGateOn = "gateOn";
    //static constexpr const char* paramThreshold = "gateThreshold";
    //static constexpr const char* paramAttack = "gateAttack";
    //static constexpr const char* paramRelease = "gateRelease";
    //static constexpr const char* paramGain = "gain";

private:
    
    //FFT VISU
    //static constexpr int fftOrder = 10;
    //static constexpr int fftSize = 1 << fftOrder;
    //static constexpr int bufferSize = fftSize*2;
    //std::array<float, bufferSize> fifoBuffer;
    //std::atomic<int> fifoIndex {0};
    
    //NN
    torch::jit::script::Module denoise_model;
    
    torch::Tensor inputTensor;
    float* inputTensorPtr = nullptr;
    
    torch::Tensor outputTensor;
    float* outputTensorPtr = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
    
    //PROCESSING
    juce::AudioBuffer<float> unprocessedInBuffer;
    juce::AudioBuffer<float> processedBuffer;
    
    int lastNumSamples = 1024;
    
    //WAVE VIEWER
    const int vwRepaintRate = 60;
    const int vwBufferSize = 512;
    
    //juce::ValueTree state;
    //juce::UndoManager undoManager;

};
