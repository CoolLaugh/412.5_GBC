#pragma once
#include <SFML/Audio.hpp>
#include "CommonDefinitions.h"
#include "memory.h"

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
	std::vector<float> buffer;
	std::vector<float> lastBuffer;
};

class APU {

public:
	Memory* memory;
	Channel channel1;
	Channel channel2;
	int scale = 500;

	APU();

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

private:

	void channel1FrameSequencer(Channel& channel, word FrequencyHigh);
	void channel1Sweep(Channel& channel);
	void channel1Timer(Channel& channel, word FrequencyHigh, word FrequencyLow);
	void channel1Duty(Channel& channel, word dutyAddress);
	void channel1Envelope(Channel& channel, word volumeEnvelopeAddress);
	void channel1buffer(Channel& channel);
};

