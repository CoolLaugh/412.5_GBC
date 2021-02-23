#pragma once
#include <SFML/Audio.hpp>
#include "CommonDefinitions.h"
#include "memory.h"
#include "BufferedSoundStream.h"
#include <deque>


struct Channel {

	int frameSequencer = 0;
	int frameSequencerStep = 0;
	bool enabled = true;
	int length = 0;
	int sweepPeriod = 0;
	bool sweepEnabled = true;
	int timer = 0;
	int dutyIndex = 0;
	bool duty = true;
	int envelope = 0;
	bool envelopeEnabled = true;
	int amp = 0;
	int pcc = 95;
	int waveIndex = 0;
	word lfsr = 0;
	std::vector<int> buffer;
	std::vector<float> displayBuffer;
	int number = 0;
	bool mute = false;
};

class APU {

public:
	Memory* memory;
	Channel channel1;
	Channel channel2;
	Channel channel3;
	Channel channel4;

	bool muteAll = false;

	int scale = 500;
	int BufferAmount = (44100 / 10);

	APU();
	~APU();

private:

	std::vector<sf::Int16> samples;
	sf::SoundBuffer buffer;
	sf::Sound sound;

	word frequencyChange(word lastFrequency);
	int clockCounter;
	float volume = 0.5f;


	const bool duties[4][8] = { {true, false, false, false, false, false, false, false},
								{true, true, false, false, false, false, false, false},
								{true, true, true, true, false, false, false, false},
								{true, true, true, true, true, true, false, false} };

public:

	void step(int clocks);

	void resetSC1Length(byte val);
	void resetSC2Length(byte val);
	void resetSC3Length(byte val);
	void resetSC4Length(byte val);

private:

	void frameSequencer(Channel& channel, word FrequencyHigh);
	void sweep(Channel& channel);
	void SquareTimer(Channel& channel, word FrequencyHigh, word FrequencyLow);
	void squareDuty(Channel& channel, word dutyAddress);
	void envelope(Channel& channel, word volumeEnvelopeAddress);
	void squareBuffer(Channel& channel);
	void waveTimer(Channel& channel, word FrequencyHigh, word FrequencyLow);
	void wave(Channel& channel);
	void noiseTimer(Channel& channel);
	void noiseBuffer(Channel& channel);

	void fillDisplayBuffer(Channel& channel);
};

