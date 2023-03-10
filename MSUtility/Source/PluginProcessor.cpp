/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MSUtilityAudioProcessor::MSUtilityAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                       
                        treeState(*this, nullptr, juce::Identifier("PARAMETERS"),
{
    std::make_unique<juce::AudioParameterFloat>("width", "Image width", 0.0f, 2.0f, 1.0f),
   
    
    std::make_unique<juce::AudioParameterChoice>("inChoice", "Input", juce::StringArray("Stereo", "Mid-Side"), 0),
    
    std::make_unique<juce::AudioParameterInt>("midDb", "MidGain", -96, 24, 0),
    std::make_unique<juce::AudioParameterInt>("sidesDb", "SidesGain", -96, 24, 0),
    
    //add std::make_unique<juce::AudioParameterFloat> for Input/Output level
    
    //perhaps mid or side level individually?
    //it would be good to pan mid or side individually to L/R channels
    
    std::make_unique<juce::AudioParameterChoice>("outChoice", "Output", juce::StringArray("Stereo", "Mid-Side"), 0)
    
    
   
    
})


#endif
{
    const juce::StringArray params = {"width", "inChoice", "midDb", "sidesDb", "outChoice"};
    for (int i = 0; i <= 4; ++i)
    {
        // adds a listener to each parameter in the array.
        treeState.addParameterListener(params[i], this);
    }
        
    treeState.addParameterListener("width", this);
    treeState.addParameterListener("InChoice", this);
    treeState.addParameterListener("OutChoice", this);
    treeState.addParameterListener("midDb", this);
    treeState.addParameterListener("sidesDb", this);
    
}

MSUtilityAudioProcessor::~MSUtilityAudioProcessor()
{
    
}
juce::AudioProcessorValueTreeState::ParameterLayout MSUtilityAudioProcessor::createParameterLayout(){
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    //update number of reservation for each new added parameter
    params.reserve(4); ////???
    
    return { params.begin(), params.end()};
}

//==============================================================================
const juce::String MSUtilityAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MSUtilityAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MSUtilityAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MSUtilityAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MSUtilityAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MSUtilityAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MSUtilityAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MSUtilityAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MSUtilityAudioProcessor::getProgramName (int index)
{
    return {};
}

void MSUtilityAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MSUtilityAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
   
    juce::dsp::ProcessSpec spec;
    
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    midGainModule.prepare(spec);
    sidesGainModule.prepare(spec); 
    
    width.reset(sampleRate,  0.02); //set ramp time
    
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MSUtilityAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MSUtilityAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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
#endif

void MSUtilityAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // get width position
//    float widthVal = (juce::AudioProcessorValueTreeState::getParameterAsValue("width", parameterID    )    const
// + 1.0f) / 2.0f;
//
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    if(totalNumInputChannels == 2){
        
    
    juce::dsp::AudioBlock<float>audioBlock {buffer};
    
  
    
    for (int channel = 0; channel < audioBlock.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // get reference of right and left channel into variables
        auto* left = audioBlock.getChannelPointer (0);
        auto* right = buffer.getWritePointer (1);
        
        for (int sample = 0; sample < audioBlock.getNumSamples(); ++sample)
        {
            const auto mid = 0.5 * (left[sample] + right[sample]);
            const auto sides = 0.5 * (left[sample] - right[sample]);
//
//            midGainModule.setGainDecibels(midGain);
//            sidesGainModule.setGainDecibels(sidesGain);
            
            const auto newMid = (2.0 - width.getNextValue()) * (mid);
            const auto newSides = (width.getNextValue()) * (sides);
            
//            const auto OutLeft = midGainModule.processSample(mid) + sidesGainModule.processSample(sides);
            const auto OutLeft =  newMid + newSides;
//            const auto OutRight = midGainModule.processSample(mid) - sidesGainModule.processSample(sides);
            const auto OutRight = newMid - newSides; //and get rid of Modules
            
            if (width.getNextValue() >= 1.0f)
            {
                
                left[sample] = OutLeft;
                right[sample] = OutRight;
            }
           else
           {
               const auto volumeScaler = juce::jmap(width.getNextValue(), 1.0f, 0.0f, 0.0f, -8.0f);
               
               left[sample] = OutLeft * juce::Decibels::decibelsToGain(volumeScaler);
               right[sample] = OutRight * juce::Decibels::decibelsToGain(volumeScaler);
           }
           
            
            }
        }
    }
}

//==============================================================================
bool MSUtilityAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MSUtilityAudioProcessor::createEditor()
{
    return new MSUtilityAudioProcessorEditor (*this, treeState);
}

//==============================================================================
void MSUtilityAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MSUtilityAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MSUtilityAudioProcessor();
}

void MSUtilityAudioProcessor::parameterChanged(const juce::String& parameterID, float
                                                  newValue)
{
    
  // if (parameterID == "width")
    //  (newValue);
//    else if (parameterID == "inChoice")
//        ladderFilter.setResonance(newValue);
//    else if (parameterID == "midDb")
//        ladderFilter.setDrive(newValue);
//    else if (parameterID == "sideDb")
//
//    else if (parameterID == "outChoice")
//    {
//        switch ((int)newValue)
//        {
//            case 0: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::LPF12);
//                break;
//            case 1: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::LPF24);
//                break;
//            case 2: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::HPF12);
//                break;
//            case 3: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::HPF24);
//                break;
//            case 4: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::BPF12);
//
//                break;
//            case 5: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::BPF24);
//                break;
//        }
  //  }
    
    
}
