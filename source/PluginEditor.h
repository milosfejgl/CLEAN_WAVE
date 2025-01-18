#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"
#include "VisualFFTComponent.h"
#include "RetrainModelWindow1.hpp"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer, private juce::Slider::Listener, private juce::Button::Listener, private juce::ComboBox::Listener
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    /*
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> nnOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> nnMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> gateOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateThresAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
     */

private:
    
    void sliderValueChanged (juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    
    void timerCallback() override;
    
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    
    //LINES
    int height;
    int width;
    int horizontalLineY;
    int verticalLineX1;
    int verticalLineX2;
    double horizontalLineRatio = 5.0 / 6.0;
    double verticalLineRatio1 = 0.45; //2.0 / 5.0;
    double verticalLineRatio2 = 0.85; //4.0 / 5.0;
    
    //PICTURES
    juce::Image logo = juce::ImageCache::getFromMemory( BinaryData::malone_logo_png, BinaryData::malone_logo_pngSize );
    
    //FFT COMPONENT
    VisualFFTComponent visualFFTComponent;
    
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
    
    //NN CONTROL VARIABLES
    juce::Slider nnMixSlider;
    const int nnButtWidth = 60;
    const int nnButtHeight = nnButtWidth;
    juce::TextButton nnButton {"OFF"};
    bool nnOn;
    
    //NN CHOOSE MODEL
    juce::ComboBox chooseModelBox;
    juce::Label selectedOptionLabel;
    
    //NN RETRAIN
    juce::TextButton retrainModelButton {"retrain model"};
    bool startRetraining = false;
    bool modelRetrained = false;
    
    //GATE VARIABLES
    const int gateButtWidth = 40;
    const int gateButtHeight = 60;
    juce::TextButton gateButton {"OFF"};
    bool gateOn = true;
    juce::Slider gateThresSlider;
    juce::Slider gateAttackSlider;
    juce::Slider gateReleaseSlider;
    juce::Label gateThresLabel;
    juce::Label gateAttackLabel;
    juce::Label gateReleaseLabel;
    
    //GAIN VARIABLES
    juce::Slider gainSlider;
    
    //COLORS
    juce::Colour backgroundColor = juce::Colours::whitesmoke;
    juce::Colour controllLineColor = juce::Colours::white;
    juce::Colour middleLineColor = juce::Colours::white;
    juce::Colour textColor = juce::Colours::black;
    
    //WINDOWS
    std::unique_ptr<RetrainModelWindow> retrainModelWindow;
    
    //WAVES
    //juce::AudioVisualiserComponent unprocessedWaveViewer{1}; // 1 channel
    //juce::AudioVisualiserComponent processedWaveViewer{1};   // 1 channel
    
    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> nnMixAttachment;
    
    
};
