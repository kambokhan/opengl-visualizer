#include "MusicProcessing.h"

MusicProcessing::MusicProcessing() : windowSize(1024)
{
	init();
}

MusicProcessing::~MusicProcessing() {
	if (sound) {
		channel->removeDSP(dsp);
		sound->release();
	}

	if (fmodSystem) {
		dsp->release();
		fmodSystem->close();
		fmodSystem->release();
	}
}


void MusicProcessing::init() {
	FMOD_RESULT result = FMOD::System_Create(&fmodSystem);
	if (result != FMOD_OK) {
		printf("FMOD Error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
	else {
		fmodSystem->init(1, FMOD_INIT_NORMAL, NULL);

		fmodSystem->createDSPByType(FMOD_DSP_TYPE_FFT, &dsp);
		dsp->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_TRIANGLE);
		dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, windowSize);
	}
}

void MusicProcessing::loadMusic(std::string filePath) {
	fmodSystem->createSound(filePath.c_str(), FMOD_LOOP_NORMAL | FMOD_CREATESAMPLE, 0, &sound);
}

void MusicProcessing::playMusic() {
	fmodSystem->playSound(sound, 0, false, &channel);
	channel->addDSP(0, dsp);
	dsp->setActive(true);
}

void MusicProcessing::update() {
	fmodSystem->update();
}


int MusicProcessing::initSpectrum(int maxBars) {
	// Calculates frequency points 
	std::vector<int> frequencyOctaves;
	frequencyOctaves.push_back(0);
	for (int i = 1; i < 13; ++i)
		frequencyOctaves.push_back((int)((44100 / 2) / (float)pow(2, 12 - i)));
	// 10,20,40,80,160 etc... until 20kHz-> adjusted to start from 20hz and finish at 20khz

	int bandWidth = (44100 / windowSize); //window size -> audio buffer size
	int bandsPerOctave = maxBars / 12; // octaves per bar

	// Calculates num fft samples that get to be represented per bar
	numSamplesPerBar.clear();
	for (int octave = 0; octave < 12; ++octave)
	{
		int indexLow = frequencyOctaves[octave] / bandWidth;
		int indexHigh = (frequencyOctaves[octave + 1]) / bandWidth;
		int octavaIndexes = (indexHigh - indexLow);

		if (octavaIndexes > 0)
		{
			if (octavaIndexes <= bandsPerOctave)
			{
				for (int count = 0; count < octavaIndexes; ++count)
				{
					numSamplesPerBar.push_back(1);
				}
			}
			else
			{
				for (int count = 0; count < bandsPerOctave; ++count)
				{
					numSamplesPerBar.push_back(octavaIndexes / bandsPerOctave);
				}
			}
		}
	}
	return numSamplesPerBar.size();
}

void MusicProcessing::getFrequency(float* spectrum) {
	void* data;
	dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, &data, 0, 0, 0);
	FMOD_DSP_PARAMETER_FFT* fft = (FMOD_DSP_PARAMETER_FFT*)data;

	if (fft)
	{
		int length = fft->length / 2;
		int channels = fft->numchannels;

		if (length > 0) {
			int indexFFT = 0;
			for (int index = 0; index < numSamplesPerBar.size(); ++index) //check how many samples per bar
			{
				for (int frec = 0; frec < numSamplesPerBar[index]; ++frec) // for each frequency that gets represented by the bar
				{
					for (int channel = 0; channel < channels; ++channel) // for each channel (stereo is 2 channels)
					{
						spectrum[index] += fft->spectrum[channel][indexFFT];  //add the amplitude to the spectrum array
					}
					++indexFFT;
				}
				spectrum[index] /= (float)(numSamplesPerBar[index] * channels);
			}
		}
	}
}
