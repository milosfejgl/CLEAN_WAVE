#include "VisualFFTComponent.h"

VisualFFTComponent::VisualFFTComponent()
    : fft(fftOrder), fftData(2 * fftSize, 0.0f), window(fftSize)
{
    // Initialize rectangle with default values
    rectangle.setBounds(10, 10, height, width);
    
    juce::dsp::WindowingFunction<float>::fillWindowingTables(window.data(), window.size(), juce::dsp::WindowingFunction<float>::hann);
}

void VisualFFTComponent::initializeSliders()
{
    for (auto& slider : sliders)
        {
            slider.setSliderStyle(juce::Slider::LinearVertical);
            slider.setRange(0.0, 1.0);
            slider.setValue(0.0);
            slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            slider.setEnabled(false); // Make the slider non-adjustable
            
            slider.setColour(juce::Slider::thumbColourId, juce::Colours::black);
            slider.setColour(juce::Slider::trackColourId, juce::Colours::whitesmoke);
            slider.setColour(juce::Slider::backgroundColourId, juce::Colours::whitesmoke);
            
            addAndMakeVisible(slider);
        }
}

VisualFFTComponent::~VisualFFTComponent() = default;

void VisualFFTComponent::paint(juce::Graphics& g)
{
    // Clear the background
    g.fillAll(juce::Colours::whitesmoke);

    //g.setColour(juce::Colours::black);
    //g.fillRect(rectangle);
    //g.setColour(juce::Colours::grey);
    //g.drawRect(rectangle);
}

void VisualFFTComponent::resized()
{
    // Position sliders within the rectangle
    auto sliderWidth = rectangle.getWidth() / numberOfSliders;
    for (int i = 0; i < numberOfSliders; ++i)
    {
        sliders[i].setBounds(rectangle.getX()  + i * sliderWidth, rectangle.getY(), sliderWidth, rectangle.getHeight());
    }
}

void VisualFFTComponent::setRectanglePosition(int x, int y)
{
    rectangle.setBounds(x, y, width, height);
    resized();
    repaint(); // Trigger a repaint to update the component
}

void VisualFFTComponent::setSliderValues(const juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumSamples() < fftSize)
        return; // Ensure the buffer has enough samples

    auto* channelData = buffer.getReadPointer(0);

    // Apply the window to the audio data
    for (int i = 0; i < fftSize; ++i)
    {
        fftData[i] = channelData[i] * window[i];
    }

    // Perform the FFT
    fft.performFrequencyOnlyForwardTransform(fftData.data());

    // Calculate the magnitude values
    for (int i = 0; i < numberOfSliders; ++i)
    {
        auto startBin = i * (fftSize / numberOfSliders);
        auto endBin = (i + 1) * (fftSize / numberOfSliders);
        float magnitude = 0.0f;
        
        for (int bin = startBin; bin < endBin; ++bin)
        {
            magnitude += fftData[bin];
        }
        
        magnitude /= (endBin - startBin); // Average magnitude
        sliders[i].setValue(magnitude / static_cast<float>(fftSize));
    }
}
