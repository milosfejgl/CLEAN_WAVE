#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), visualFFTComponent()
{
    //juce::ignoreUnused (processorRef);
    //addAndMakeVisible (inspectButton);
    // this chunk of code instantiates and opens the melatonin inspector
    //inspectButton.onClick = [&] {
    //    if (!inspector)
    //    {
    //        inspector = std::make_unique<melatonin::Inspector> (*this);
    //        inspector->onClose = [this]() { inspector.reset(); };
    //    }

    //    inspector->setVisible (true);
    //};
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //setSize (400, 300);
    
    /*
    nnOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        p.parameters, "nnOn", nnButton);

    nnMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.parameters, "nnMix", nnMixSlider);

    gateOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        p.parameters, "gateOn", gateButton);

    gateThresAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.parameters, "gateThreshold", gateThresSlider);

    gateAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.parameters, "gateAttack", gateAttackSlider);

    gateReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.parameters, "gateRelease", gateReleaseSlider);

    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.parameters, "gain", gainSlider);
     */
    
    //BACKGROUND
    logo = juce::ImageCache::getFromMemory( BinaryData::malone_logo_png, BinaryData::malone_logo_pngSize );
    setSize (700, 500);
    
    
    //WAVE VIEWERS
    processorRef.unprocessedWaveViewer.setColours(juce::Colours::transparentWhite, juce::Colours::red.withAlpha(0.5f));
    processorRef.unprocessedWaveViewer.setOpaque(false);
    addAndMakeVisible(processorRef.unprocessedWaveViewer);
    //addAndMakeVisible(unprocessedWaveViewer);
    
    processorRef.processedWaveViewer.setColours(juce::Colours::transparentWhite, juce::Colours::blue.withAlpha(0.5f));
    processorRef.processedWaveViewer.setOpaque(false);
    addAndMakeVisible(processorRef.processedWaveViewer);
    //addAndMakeVisible(processedWaveViewer);
    
    
    //CONTROLS
    //nn slider
    nnMixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    nnMixSlider.setTitle("NN MIX");
    nnMixSlider.setRange(0.0, 1.0, 0.005);
    nnMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 90);
    nnMixSlider.setPopupDisplayEnabled(true, false, this);
    nnMixSlider.setTextValueSuffix(" mix of the neural netwok prediction");
    nnMixSlider.setValue(0.5);
    addAndMakeVisible(&nnMixSlider);
    nnMixSlider.addListener(this);
    
    
    //choose model
    chooseModelBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::black);
    chooseModelBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
    chooseModelBox.setColour(juce::ComboBox::arrowColourId, juce::Colours::white);
    chooseModelBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    //chooseModelBox.setColour(juce::ComboBox::buttonColourId, juce::Colours::black);

    chooseModelBox.addItem("model 1: click", 1);
    chooseModelBox.addItem("model 2: background", 2);
    chooseModelBox.addItem("model 3: static", 3);
    chooseModelBox.addItem("model 4: tones", 4);
    chooseModelBox.addItem("model 5: drive", 5);
    chooseModelBox.addItem("model 6: my own", 6);
    chooseModelBox.setSelectedId(1);
    chooseModelBox.addListener(this);
    addAndMakeVisible(chooseModelBox);
    addAndMakeVisible(selectedOptionLabel);
    
    
    //gate
    juce::Font courierFont("Courier New", 12.0f, juce::Font::plain);
    
    gateThresSlider.setSliderStyle(juce::Slider::Rotary);
    gateThresSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 20);
    gateThresSlider.setPopupDisplayEnabled(true, false, this);
    gateThresSlider.setTextValueSuffix(" dB");
    gateThresSlider.setRange(-60.0f, 10.0f, 0.1f);
    gateThresSlider.setValue(0.0f);
    addAndMakeVisible(gateThresSlider);
    gateThresSlider.addListener(this);
    
    gateThresLabel.setText("threshold", juce::dontSendNotification);
    gateThresLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    gateThresLabel.setFont(courierFont);
    gateThresLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gateThresLabel);
    
    gateAttackSlider.setSliderStyle(juce::Slider::Rotary);
    gateAttackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 20);
    gateAttackSlider.setPopupDisplayEnabled(true, false, this);
    gateAttackSlider.setTextValueSuffix(" ms");
    gateAttackSlider.setRange(1.0f, 1000.0f, 1.0f);
    gateAttackSlider.setValue(10.0f);
    addAndMakeVisible(gateAttackSlider);
    gateAttackSlider.addListener(this);
    
    gateAttackLabel.setText("attack", juce::dontSendNotification);
    gateAttackLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    gateAttackLabel.setFont(courierFont);
    gateAttackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gateAttackLabel);
    
    gateReleaseSlider.setSliderStyle(juce::Slider::Rotary);
    gateReleaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 20);
    gateReleaseSlider.setPopupDisplayEnabled(true, false, this);
    gateReleaseSlider.setTextValueSuffix(" ms");
    gateReleaseSlider.setRange(1.0f, 3000.0f, 1.0f);
    gateReleaseSlider.setValue(200.0f);
    addAndMakeVisible(gateReleaseSlider);
    gateReleaseSlider.addListener(this);
    
    gateReleaseLabel.setText("release", juce::dontSendNotification);
    gateReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    gateReleaseLabel.setFont(courierFont);
    gateReleaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gateReleaseLabel);
    
    
    //gain
    gainSlider.setSliderStyle(juce::Slider::Rotary);
    gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 20);
    gainSlider.setPopupDisplayEnabled(true, false, this);
    gainSlider.setTextValueSuffix(" dB");
    //gainSlider.setRange(0.0, 2.0, 0.05);
    gainSlider.setRange(-60.0, 6.0, 0.6);
    gainSlider.setValue(0.0);
    addAndMakeVisible(gainSlider);
    gainSlider.addListener(this);
    
    
    //buttons
    nnButton.setButtonText("OFF");
    nnOn = false;
    nnButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
    nnButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(&nnButton);
    nnButton.addListener(this);
    
    gateButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
    gateButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(&gateButton);
    gateButton.addListener(this);
    
    retrainModelButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
    retrainModelButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(&retrainModelButton);
    retrainModelButton.addListener(this);
    
    //AUDIO VISU
    //visualFFTComponent.setRectanglePosition(200, 150);
    
    
        
    
    
    //addAndMakeVisible(visualFFTComponent);
    //addAndMakeVisible(nnMixSlider);
    //nnMixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(processorRef.parameters, "nnMix", nnMixSlider));
       
    
    //startTimerHz(30);
}

PluginEditor::~PluginEditor()
{
    nnButton.removeListener(this);
    gateButton.removeListener(this);
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    //BACKGROUND
    //juce::Colour backgroundColour = juce::Colour::fromRGB(210, 225, 255);
    g.fillAll(juce::Colours::white);
    g.fillAll (backgroundColor.withAlpha(0.8f));
    
    
    //BACKGROUND IMAGE
    //g.drawImageAt(logo, (getWidth() - logo.getWidth()) / 2, (getHeight() - logo.getHeight()) / 2);
    

    
    //LINES
    g.setColour(controllLineColor);
    width = getWidth();
    height = getHeight();
    horizontalLineY = static_cast<int>(height * horizontalLineRatio);
    verticalLineX1 = static_cast<int>(width * verticalLineRatio1);
    verticalLineX2 = static_cast<int>(width * verticalLineRatio2);
    
    juce::Colour rectColor1 = juce::Colours::lightgrey;
    juce::Colour rectColor2 = rectColor1.darker(0.2f); // Slightly darker
    juce::Colour rectColor3 = rectColor1.darker(0.5f); // Even darker

    g.setColour(rectColor1);
    g.fillRect(0, horizontalLineY, verticalLineX1, height - horizontalLineY);

    g.setColour(rectColor2);
    g.fillRect(verticalLineX1, horizontalLineY, verticalLineX2 - verticalLineX1, height - horizontalLineY);

    g.setColour(rectColor3);
    g.fillRect(verticalLineX2, horizontalLineY, width - verticalLineX2, height - horizontalLineY);

    g.setColour(controllLineColor);
    g.drawLine(0, horizontalLineY, width, horizontalLineY, 1.0f);
    g.drawLine(verticalLineX1, horizontalLineY, verticalLineX1, height, 2.0f);
    g.drawLine(verticalLineX2, horizontalLineY, verticalLineX2, height, 2.0f);
    
    g.setColour(middleLineColor);
    g.drawLine(0, height * 0.5, width, height * 0.5, 1.0f);
    
    
    //TEXTs
    auto area = getLocalBounds();
    g.setColour (juce::Colours::blue);
    g.setFont (16.0f);
    //auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
    
    juce::Font pluginNameFont("Sans-serif Bold", 60.0f, juce::Font::bold);
    g.setFont(pluginNameFont);
    g.setColour(textColor);
    g.drawText ("CLEAN WAVE", area.removeFromTop (80), juce::Justification::centred, false);
    
    juce::Font aboutPluginFont("Courier New", 15.f, juce::Font::bold);
    g.setFont(aboutPluginFont);
    g.setColour(juce::Colours::darkgrey.withAlpha(0.9f));
    g.drawText("a plugin which makes your waves clean.", area.removeFromTop(10), juce::Justification::centred, false);
    g.drawText("created by milos fejgl, 2024.", area.removeFromTop(20), juce::Justification::centred, false);
    
    juce::Font sectionTitleFont("Courier New", 18.0f, juce::Font::bold);
    g.setFont(sectionTitleFont);
    g.setColour(textColor);
    g.drawText("denoising", 10, horizontalLineY - 20, verticalLineX1 - 20, 20, juce::Justification::centredLeft);
    g.drawText("gate", verticalLineX1 + 10, horizontalLineY - 20, width - verticalLineX1 - 20, 20, juce::Justification::centredLeft);
    g.drawText("gain", verticalLineX2 + 10, horizontalLineY - 20, width - verticalLineX2 - 20, 20, juce::Justification::centredLeft);
    

    
}

void PluginEditor::resized()
{
    
    auto area = getLocalBounds();
    area.removeFromBottom(50);
    width = getWidth();
    height = getHeight();
    horizontalLineY = static_cast<int>(height * horizontalLineRatio);
    verticalLineX1 = static_cast<int>(width * verticalLineRatio1);
    verticalLineX2 = static_cast<int>(width * verticalLineRatio2);
    
    
    //WAVE VIEWERS
    auto waveViewerBounds = getLocalBounds().withSizeKeepingCentre(getWidth(), getHeight() * 0.55);
    processorRef.unprocessedWaveViewer.setBounds(waveViewerBounds);
    processorRef.processedWaveViewer.setBounds(waveViewerBounds);
    //unprocessedWaveViewer.setBounds(waveViewerBounds);
    //processedWaveViewer.setBounds(waveViewerBounds);

    
    
    //NN CONTROLS
    nnButton.setBounds(10, horizontalLineY + 10, nnButtWidth, nnButtHeight);
    nnMixSlider.setBounds(nnButton.getRight() + 10, horizontalLineY + 7, verticalLineX1 - nnButton.getRight() - 20, 15);
    chooseModelBox.setBounds(nnMixSlider.getX(), nnMixSlider.getBottom() + 5, nnMixSlider.getWidth(), 20);
    retrainModelButton.setBounds(chooseModelBox.getX(), chooseModelBox.getBottom() + 5, nnMixSlider.getWidth(), 20);
        
    
    //GATE CONTROLS
    int gateSliderWidth = 60;
    int gateSliderHeight = gateSliderWidth;
    int componentSpacing = (verticalLineX2 - verticalLineX1 - 3 * gateSliderWidth - gateSliderWidth) / 5; // Adjusted forgateButton
    int componentY = horizontalLineY + 10;
        
    gateButton.setBounds(verticalLineX1 + componentSpacing, componentY, gateSliderWidth, gateSliderHeight);
    gateThresSlider.setBounds(gateButton.getRight() + componentSpacing, componentY - 5, gateSliderWidth, gateSliderHeight);
    gateAttackSlider.setBounds(gateThresSlider.getRight() + componentSpacing, componentY - 5, gateSliderWidth, gateSliderHeight);
    gateReleaseSlider.setBounds(gateAttackSlider.getRight() + componentSpacing, componentY - 5, gateSliderWidth,gateSliderHeight);
        
    int labelY = gateThresSlider.getBottom() - 10;
        
    gateThresLabel.setBounds(gateThresSlider.getX(), labelY, gateSliderWidth, 20);
    gateAttackLabel.setBounds(gateAttackSlider.getX(), labelY, gateSliderWidth, 20);
    gateReleaseLabel.setBounds(gateReleaseSlider.getX(), labelY, gateSliderWidth, 20);
        

    
    //GAIN CONTROLS
    int rectCenterX = verticalLineX2 + (width - verticalLineX2) / 2;
    int rectCenterY = horizontalLineY + (height - horizontalLineY) / 2;
    int gainSliderWidth = 80;
    int gainSliderHeight = 80;
    gainSlider.setBounds(rectCenterX - gainSliderWidth / 2, rectCenterY + 5 - gainSliderHeight / 2, gainSliderWidth, gainSliderHeight);

    
    //inspectButton.setBounds (getLocalBounds().withSizeKeepingCentre(100, 50));
    //visualFFTComponent.setBounds(getLocalBounds());
}

void PluginEditor::timerCallback()
{
    //juce::AudioBuffer<float> buffer;
    //if( processorRef.getAudioBuffer(buffer) )
    //    visualFFTComponent.setSliderValues(buffer);
    
}

void PluginEditor::sliderValueChanged(juce::Slider* slider)
{
    //NN SLIDER
    if (slider == &gainSlider)
        processorRef.nnMix = nnMixSlider.getValue();
    
    //GATE SLIDERS
    if (slider == &gateThresSlider)
        processorRef.gateThreshold = gateThresSlider.getValue();
    
    if (slider == &gateAttackSlider)
        processorRef.gateAttack = gateAttackSlider.getValue();
    
    if (slider == &gateReleaseSlider)
        processorRef.gateRelease = gateReleaseSlider.getValue();
    
    //GAIN SLIDER
    if (slider == &gainSlider)
        //processorRef.gain = gainSlider.getValue();
        processorRef.gain = juce::Decibels::decibelsToGain(gainSlider.getValue());
}

void PluginEditor::buttonClicked(juce::Button* button) {
    
    //NN ON / OFF BUTTON
    if (button == &nnButton) {
        nnOn = !nnOn;
        processorRef.nnOn = nnOn;
        nnButton.setButtonText(nnOn ? "ON" : "OFF");
        nnButton.setColour(juce::TextButton::buttonColourId, nnOn ? juce::Colours::white : juce::Colours::black);
        nnButton.setColour(juce::TextButton::textColourOffId, nnOn ? juce::Colours::black : juce::Colours::white);
    }
    
    //NN RE-TRAIN MODEL BUTTON
    if (button == &retrainModelButton) {
        //if (retrainModelWindow == nullptr) {
        //    retrainModelWindow = std::make_unique<RetrainModelWindow>();
        //}
        //retrainModelWindow->setVisible(true);
        //retrainModelWindow->toFront(true);
    }
    
    //GATE BUTTON
    if (button == &gateButton) {
        gateOn = !gateOn;
        processorRef.gateOn = gateOn;
        gateButton.setButtonText(gateOn ? "ON" : "OFF");
        gateButton.setColour(juce::TextButton::buttonColourId, gateOn ? juce::Colours::white : juce::Colours::black);
        gateButton.setColour(juce::TextButton::textColourOffId, gateOn ? juce::Colours::black : juce::Colours::white);
    }
}

void PluginEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &chooseModelBox)
    {
        // Turn off the neural network
        nnOn = false;
        processorRef.nnOn = nnOn;
        nnButton.setButtonText("OFF");
        nnButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
        nnButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

        // Load the new model based on the selected option
        switch (chooseModelBox.getSelectedId())
        {
            case 1: // Model 1
                processorRef.loadModel(BinaryData::dns48_12_11_pt, BinaryData::dns48_12_11_ptSize);
                break;
            case 2: // Model 2
                processorRef.loadModel(BinaryData::dns48_12_12_pt, BinaryData::dns48_12_12_ptSize);
                break;
            case 3: // Model 3
                processorRef.loadModel(BinaryData::dns48_12_13_pt, BinaryData::dns48_12_13_ptSize);
                break;
            case 4: // Model 4
                processorRef.loadModel(BinaryData::dns48_12_14_pt, BinaryData::dns48_12_14_ptSize);
                break;
            case 5: // Model 5
                processorRef.loadModel(BinaryData::dns48_12_15_pt, BinaryData::dns48_12_15_ptSize);
                break;
            case 6: // Custom Model
                processorRef.loadModel(BinaryData::dns48_12_11_pt, BinaryData::dns48_12_11_ptSize);
                break;
            default:
                juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                                       "Model Error",
                                                       "Selected model not found!");
                return;
        }

        // Restart the neural network
        nnOn = true;
        processorRef.nnOn = nnOn;
        nnButton.setButtonText("ON");
        nnButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
        nnButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        // Update selected model label
        selectedOptionLabel.setText("Selected: " + chooseModelBox.getText(), juce::dontSendNotification);
    }
}

    

