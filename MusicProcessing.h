#pragma once
#ifndef MusicProcessing_H
#define MusicProcessing_H

#include <fmod.hpp>
#include <fmod_errors.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class MusicProcessing
{
public:
	MusicProcessing();
	~MusicProcessing();

	void loadMusic(string filePath);
	void playMusic();
	void update();

	int initSpectrum(int maxBars);
	void getFrequency(float* spectrum);

private:
	FMOD::System* fmodSystem;
	FMOD::Channel* channel;
	FMOD::Sound* sound;
	FMOD::DSP* dsp;

	vector<int> numSamplesPerBar;
	int windowSize;
	void init();
};

#endif
