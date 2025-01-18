#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>

//constexpr int numberOfSliders = 15;

class VisualFFTComponent : public juce::Component
{
public:
    VisualFFTComponent();
    ~VisualFFTComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setRectanglePosition(int x, int y);
    void setSliderValues(const juce::AudioBuffer<float>& values);
    
    void getHeight(); //dodělat
    void getWidth(); //dodělat

private:
    juce::Rectangle<int> rectangle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualFFTComponent)
    
    static constexpr int fftOrder = 10;
    static constexpr int fftSize = 1 << fftOrder;
    static constexpr int numberOfSliders = 15;
    const int height = 200;
    const int width = 400;
    
    juce::Slider sliders[numberOfSliders];
    
    juce::dsp::FFT fft;
    std::vector<float> fftData;
    std::vector<float> window;
    
    void initializeSliders();
};



