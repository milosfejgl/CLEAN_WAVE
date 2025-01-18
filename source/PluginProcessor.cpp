#include "PluginProcessor.h"
#include "PluginEditor.h"
//#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>
#include <memory>

//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        unprocessedInBuffer (2, 1024),
        processedBuffer (2, 1024),
        processedWaveViewer(1),
        unprocessedWaveViewer(1)
{
    //LOADING NN
    try {
        std::stringstream modelStream;
        modelStream.write(BinaryData::dns48_12_11_pt, BinaryData::dns48_12_11_ptSize);
        denoise_model = torch::jit::load(modelStream);
        std::cout << "MODEL LOADED";
    } catch (const c10::Error& e) {
        std::cerr << "ERROR LOADING THE MODEL !\n";
    }
    
    //CREATING BUFFERS
    lastNumSamples = 1024;
    
    //INPUT OUTPUT
    inputTensor = torch::zeros({1, 1, lastNumSamples}, torch::kFloat);
    inputTensorPtr = inputTensor.data_ptr<float>();
    
    outputTensor = torch::zeros({1, 1, lastNumSamples}, torch::kFloat);
    outputTensorPtr = outputTensor.data_ptr<float>();
    
    //WAVE VIEWRS
    processedWaveViewer.setRepaintRate(vwRepaintRate);
    processedWaveViewer.setBufferSize(vwBufferSize);
    unprocessedWaveViewer.setRepaintRate(vwRepaintRate);
    unprocessedWaveViewer.setBufferSize(vwBufferSize);
    
    /*
    state.setProperty(paramNNOn, false, nullptr);
    state.setProperty(paramNNMix, 0.5f, nullptr);
    state.setProperty(paramChosenModel, 1, nullptr);
    state.setProperty(paramGateOn, false, nullptr);
    state.setProperty(paramThreshold, 0.0f, nullptr);
    state.setProperty(paramAttack, 10.0f, nullptr);
    state.setProperty(paramRelease, 200.0f, nullptr);
    state.setProperty(paramGain, 1.0f, nullptr);
    */
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate);
    if (samplesPerBlock != lastNumSamples) {
        unprocessedInBuffer.setSize(getNumInputChannels(), samplesPerBlock, false, true, true);
        processedBuffer.setSize(getNumInputChannels(), samplesPerBlock, false, true, true);
        inputTensor = torch::zeros({1, 1, samplesPerBlock}, torch::kFloat);
        inputTensorPtr = inputTensor.data_ptr<float>();
        outputTensor = torch::zeros({1, 1, samplesPerBlock}, torch::kFloat);
        outputTensorPtr = outputTensor.data_ptr<float>();
    }
    lastNumSamples = samplesPerBlock;
    
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                   juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don’t have input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // Ensure buffers are sized properly
    unprocessedInBuffer.setSize(totalNumInputChannels, buffer.getNumSamples(), false, true, true);
    processedBuffer.setSize(totalNumInputChannels, buffer.getNumSamples(), false, true, true);

    // Process each channel
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        const auto* readPointer = buffer.getReadPointer(channel);

        // Save input to unprocessed buffer
        unprocessedInBuffer.copyFrom(channel, 0, readPointer, buffer.getNumSamples());

        if (nnOn) {
            std::memcpy(inputTensorPtr, readPointer, buffer.getNumSamples() * sizeof(float));
            outputTensor = denoise_model.forward({inputTensor}).toTensor();
            processedBuffer.copyFrom(channel, 0, outputTensor.data_ptr<float>(), buffer.getNumSamples());
        } else {
            processedBuffer.copyFrom(channel, 0, unprocessedInBuffer.getReadPointer(channel), buffer.getNumSamples());
        }

        // Mix processed and unprocessed buffers using nnMix
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            channelData[sample] = nnMix * processedBuffer.getSample(channel, sample) +
                                  (1 - nnMix) * unprocessedInBuffer.getSample(channel, sample);
        }

        // Apply gate if gateOn is true, then apply gain
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            if (gateOn) {
                float inputLevel = juce::Decibels::gainToDecibels(channelData[sample]);

                // Gate processing
                float targetEnvelope = (inputLevel < gateThreshold) ? 0.0f : 1.0f;
                float attackCoef = std::exp(-1.0f / (gateAttack * 0.001f * getSampleRate()));
                float releaseCoef = std::exp(-1.0f / (gateRelease * 0.001f * getSampleRate()));
                float coef = (targetEnvelope > gateEnvelope) ? attackCoef : releaseCoef;
                gateEnvelope = coef * gateEnvelope + (1.0f - coef) * targetEnvelope;

                // Apply gate envelope
                channelData[sample] *= gateEnvelope;
            }

            // Apply gain
            channelData[sample] *= gain;
        }
    }

    // Push buffers to wave viewers
    processedWaveViewer.pushBuffer(buffer);
    unprocessedWaveViewer.pushBuffer(unprocessedInBuffer);
}

//==============================================================================
bool PluginProcessor::getAudioBuffer(juce::AudioBuffer<float>& buffer)
{
    //if (fifoIndex >= bufferSize)
    //{
    //    buffer.setSize(1, bufferSize);
    //    buffer.copyFrom(0, 0, fifoBuffer.data(), bufferSize);
    //    fifoIndex = 0;
    //    return true;
    //}
    //return false;
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    //juce::MemoryOutputStream stream(destData, true);
    //state.writeToStream(stream);
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    /*
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    auto  newState = juce::ValueTree::readFromStream(stream);

    if (newState.isValid())
    {
        parameters.state = newState;
        
        nnOn = state.getProperty(paramNNOn);
        nnMix = state.getProperty(paramNNMix);
        gateOn = state.getProperty(paramGateOn);
        gateThreshold = state.getProperty(paramThreshold);
        gateAttack = state.getProperty(paramAttack);
        gateRelease = state.getProperty(paramRelease);
        gain = state.getProperty(paramGain);
    }
     */
    juce::ignoreUnused (data, sizeInBytes);
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

void PluginProcessor::loadModel(const void* modelData, size_t modelSize) {
    try {
        std::stringstream modelStream;
        modelStream.write(static_cast<const char*>(modelData), modelSize);
        denoise_model = torch::jit::load(modelStream);
        std::cout << "MODEL LOADED SUCCESSFULLY" << std::endl;
    } catch (const c10::Error& e) {
        std::cerr << "ERROR LOADING MODEL: " << e.what() << std::endl;
    }
}
