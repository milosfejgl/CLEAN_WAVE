//
//  RetrainModelWindow1.cpp
//  CleanWavePlugin_v0
//
//  Created by Miloš Fejgl on 01.08.2024.
//

#include "RetrainModelWindow1.hpp"

RetrainModelWindow::RetrainModelWindow()
    : DocumentWindow("Retrain Model",
                     juce::Colours::lightgrey,
                     DocumentWindow::allButtons),
    nextButton("next")
{
    // Set size and position
    setSize(300, 200); // Adjust size as needed

    // Add components
    addAndMakeVisible(&nextButton);

    // Center the window
    centreWithSize(getWidth(), getHeight());
    
    setVisible(true);
}

RetrainModelWindow::~RetrainModelWindow()
{
    nextButton.removeListener(this);
}


void RetrainModelWindow::closeButtonPressed()
{
    // Handle close button press
    setVisible(false);
}


void RetrainModelWindow::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::whitesmoke);
}


// Override the resized method to set the position of the button
void RetrainModelWindow::resized()
{
    auto buttonWidth = 100; // Width of the button
    auto buttonHeight = 30; // Height of the button
    auto xPos = getWidth() - buttonWidth - 10; // X position for the button
    auto yPos = getHeight() - buttonHeight - 10; // Y position for the button

    nextButton.setBounds(xPos, yPos, buttonWidth, buttonHeight); // Set the button's bounds
}


// Handle button click event
void RetrainModelWindow::buttonClicked(juce::Button* button)
{
    if (button == &nextButton)
    {
        juce::Logger::writeToLog("Next button clicked!");
    }
}

