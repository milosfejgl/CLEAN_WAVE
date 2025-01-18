//
//  RetrainModelWindow1.hpp
//  CleanWavePlugin_v0
//
//  Created by Miloš Fejgl on 01.08.2024.
//

#ifndef RetrainModelWindow1_hpp
#define RetrainModelWindow1_hpp

#include <stdio.h>
#include <juce_gui_basics/juce_gui_basics.h>
#pragma once

class RetrainModelWindow : public juce::DocumentWindow, juce::Button::Listener
{
public:
    RetrainModelWindow();
    ~RetrainModelWindow() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;

    void closeButtonPressed() override;
    void buttonClicked(juce::Button* button) override;
    

private:
    juce::TextButton nextButton {"next"}; // Example component

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RetrainModelWindow)
};


#endif /* RetrainModelWindow1_hpp */
