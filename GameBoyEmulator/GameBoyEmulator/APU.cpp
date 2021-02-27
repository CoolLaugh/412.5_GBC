#include "APU.h"

APU::APU() {
	channel1.number = 1;
	channel2.number = 2;
	channel3.number = 3;
	channel4.number = 4;
}

APU::~APU() {

}

word APU::frequencyChange(word lastFrequency) {

	byte sweep = memory->Read(Address::Channel1Sweep);

	bool increase = true;
	if (BitTest(sweep, 3) == true) {
		increase = false;
	}

	byte NumberOfSweepShift = sweep & 0x07;

	word changeInFrequency = lastFrequency / pow(2, NumberOfSweepShift);

	word frequency = 0;

	if (increase == true) {
		frequency = lastFrequency + changeInFrequency;
	}
	else {
		frequency = lastFrequency - changeInFrequency;
	}

	return frequency;
}

void APU::step(int clocks) {

	if (enabled == false) {
		channel1.buffer.clear();
		channel2.buffer.clear();
		channel3.buffer.clear();
		channel4.buffer.clear();
		return;
	}

	if (memory->resetSC1Length == true) {
		memory->resetSC1Length = false;
		resetSC1Length(memory->Read(Address::Channel1SoundLengthWavePatternDuty) & 0x3F);
	}
	if (memory->resetSC2Length == true) {
		memory->resetSC2Length = false;
		resetSC2Length(memory->Read(Address::Channel2Tone) & 0x3F);
	}
	if (memory->resetSC3Length == true) {
		memory->resetSC3Length = false;
		resetSC3Length(memory->Read(Address::Channel3SoundLength));
	}
	if (memory->resetSC4Length == true) {
		memory->resetSC4Length = false;
		resetSC4Length(memory->Read(Address::Channel4SoundLength) & 0x3F);
	}

	for (size_t i = 0; i < clocks; i++) {

		frameSequencer(channel1, Address::Channel1FrequencyHigh);
		sweep(channel1);
		SquareTimer(channel1, Address::Channel1FrequencyHigh, Address::Channel1FrequencyLow);
		squareDuty(channel1, Address::Channel1SoundLengthWavePatternDuty);
		envelope(channel1, Address::Channel1VolumeEnvlope);
		squareBuffer(channel1);

		frameSequencer(channel2, Address::Channel2FrequencyHigh);
		SquareTimer(channel2, Address::Channel2FrequencyHigh, Address::Channel2FrequencyLow);
		squareDuty(channel2, Address::Channel2Tone);
		envelope(channel2, Address::Channel2VolumeEnvlope);
		squareBuffer(channel2);

		waveTimer(channel3, Address::Channel3FrequencyHigh, Address::Channel3FrequencyLow);
		wave(channel3);
		frameSequencer(channel3, Address::Channel3FrequencyHigh);

		frameSequencer(channel4, Address::Channel4Counter);
		envelope(channel4, Address::Channel4PolynomialCounter);
		noiseTimer(channel4);
		noiseBuffer(channel4);

	}

	if (channel1.buffer.size() >= BufferAmount) {

		samples.clear();
		for (size_t i = 0; i < channel1.buffer.size(); i++) {

			int sample = 0;
			if (channel1.mute == false) {
				sample += channel1.buffer[i];
			}
			if (channel2.mute == false) {
				sample += channel2.buffer[i];
			}
			if (channel3.mute == false) {
				sample += channel3.buffer[i];
			}
			if (channel4.mute == false) {
				sample += channel4.buffer[i];
			}

			if (muteAll == true) {
				sample = 0;
			}

			samples.push_back(sample * scale);
		}

		buffer.loadFromSamples(&samples[0], samples.size(), 1, kAudioFrequency);

		sound.setBuffer(buffer);
		sound.play();

		fillDisplayBuffer(channel1);
		fillDisplayBuffer(channel2);
		fillDisplayBuffer(channel3);
		fillDisplayBuffer(channel4);
	}
}

void APU::resetSC1Length(byte val) {

	if (channel1.length == 0) {
		channel1.length = 64 - val;
	}

	channel1.enabled = true;
	channel1.amp = memory->Read(Address::Channel1VolumeEnvlope) >> 4;
	channel1.envelope = memory->Read(Address::Channel1VolumeEnvlope) & 0x7;
	channel1.envelopeEnabled = true; 
	
	word freq = (memory->Read(Address::Channel1FrequencyHigh) & 0x7) << 8 | memory->Read((Address::Channel1FrequencyLow));
	int hz = 131072 / (2048 - freq); // hz, number of times per second the wave duty will repeat
	channel1.timer = ((kClocksPerFrame * kFrameRate) / hz) / 8; // 70224 clocks per frame, 60 frames per second, 8 parts per wave duty

	channel1.sweepPeriod = (memory->Read(Address::Channel1Sweep) >> 4) & 7;
	int SC1sweepShift = memory->Read(Address::Channel1Sweep) & 7;
	int SC1sweepNegate = 1;
	if (BitTest(memory->Read(Address::Channel1Sweep), 3) == true) {
		SC1sweepNegate = -1;
	}

	if (channel1.sweepPeriod != 0 &&
		SC1sweepShift != 0) {

		channel1.sweepEnabled = false;
	}
	else {
		channel1.sweepEnabled = true;
	}

	if (memory->Read(Address::Channel1VolumeEnvlope) >> 3 == 0x0) {
		channel1.enabled = false;
	}
}

void APU::resetSC2Length(byte val) {

	if (channel2.length == 0) {
		channel2.length = 64 - val;
	}
	channel2.enabled = true;
	channel2.amp = memory->Read(Address::Channel2VolumeEnvlope) >> 4;
	channel2.envelope = memory->Read(Address::Channel2VolumeEnvlope) & 7;
	channel2.envelopeEnabled = true;

	if (memory->Read(Address::Channel2VolumeEnvlope) >> 3 == 0x0) {
		channel2.enabled = false;
	}
}

void APU::resetSC3Length(byte val) {

	if (channel3.length == 0) {
		channel3.length = 256 - val;
	}

	channel3.enabled = true;
	channel3.waveIndex = 0;

	word freq = (memory->Read(Address::Channel3FrequencyHigh) & 0x7) << 8 | memory->Read((Address::Channel3FrequencyLow));
	int hz = 131072 / (2048 - freq); // hz, number of times per second the wave duty will repeat
	channel3.timer = ((kClocksPerFrame * kFrameRate) / hz) / 16; // 70224 clocks per frame, 60 frames per second, 16 parts of wave data

	if (memory->Read(Address::Channel3SoundOnOFF) >> 6 == 0x0) {
		channel3.enabled = false;
	}
}

void APU::resetSC4Length(byte val) {

	if (channel4.length == 0) {
		channel4.length = 64 - val;
	}

	channel3.enabled = true;
	//channel4.timer = SC4divisor[readFromMem(0xff22) & 0x7] << (readFromMem(0xff22) >> 4);
	//SC4lfsr = 0x7fff;
	channel4.amp = memory->Read(Address::Channel4VolumeEnvlope) >> 4;
	channel4.envelope = memory->Read(Address::Channel4VolumeEnvlope) & 7;
	channel4.envelopeEnabled = true;

	if (memory->Read(Address::Channel4VolumeEnvlope) >> 3 == 0x0) {
		channel4.enabled = false;
	}

}

void APU::frameSequencer(Channel & channel, word FrequencyHigh) {

	channel.frameSequencer++;
	if (channel.frameSequencer >= 8230) {// 8230 is 512 hz at 60fps, 512 hz at 59.72fps would be 8192 clocks

		channel.frameSequencer = 0;

		channel.frameSequencerStep++;
		channel.frameSequencerStep %= 8;

		if (channel.frameSequencerStep % 2 == 0 &&
			channel.length > 0 &&
			BitTest(memory->Read(FrequencyHigh), 6) == true) {

			channel.length--;
			if (channel.length <= 0) {
				channel.enabled = false;
			}
		}
	}
}

void APU::sweep(Channel & channel) {

	// sweep occurs at 128 hz or step 2 and 6 of the frame sequencer
	if ((channel.frameSequencerStep == 2 || channel.frameSequencerStep == 6) == false) {
		return;
	}

	byte sweep = memory->Read(Address::Channel1Sweep);

	if (((sweep >> 4) & 0x7) == 0) { // sweep off - no frequency change
		return;
	}

	if ((sweep & 0x7) == 0) { // no sweep shifts
		return;
	}

	channel.sweepPeriod--;
	if (channel.sweepPeriod <= 0) {

		channel.sweepPeriod = (sweep >> 4) & 7;

		if (channel.sweepEnabled == true) {

			int shift = sweep & 7;
			byte frequencyHi = memory->Read(Address::Channel1FrequencyHigh);
			word frequency = memory->Read(Address::Channel1FrequencyLow) | ((frequencyHi & 0x7) << 8);
			int frequencyChange = frequency / pow(2, shift);
			word newFrequency = frequency;

			if (BitTest(sweep, 3) == true) {
				frequencyChange *= -1;
			}
			newFrequency += frequencyChange;
			

			if (newFrequency < 2048 &&
				shift != 0) {

				memory->Write(Address::Channel1FrequencyLow, newFrequency & 0xFF);
				frequencyHi &= 0xC0;
				frequencyHi |= (newFrequency >> 8) & 0x07;
				memory->Write(Address::Channel1FrequencyHigh, frequencyHi);
			}

			if (newFrequency >= 2048 ||
				newFrequency + frequencyChange >= 2048) {

				channel.enabled = false;
				channel.sweepEnabled = false;
			}
		}
	}
}

void APU::SquareTimer(Channel & channel, word FrequencyHigh, word FrequencyLow) {

	if (channel.timer <= 0) {

		word freq = (memory->Read(FrequencyHigh) & 0x7) << 8 | memory->Read((FrequencyLow));
		int hz = 131072 / (2048 - freq); // hz, number of times per second the wave duty will repeat
		channel.timer = ((kClocksPerFrame * kFrameRate) / hz) / 8; // 70224 clocks per frame, 60 frames per second, 8 parts per wave duty
		
		channel.dutyIndex++;
		channel.dutyIndex %= 8;

	}
	else {
		channel.timer--;
	}
}

void APU::squareDuty(Channel & channel, word dutyAddress) {

	int duty = (memory->Read(dutyAddress) >> 6) & 0x3;

	channel.duty = duties[duty][channel.dutyIndex];
}

void APU::envelope(Channel & channel, word volumeEnvelopeAddress) {

	if (channel.frameSequencer != 0 ||
		channel.frameSequencerStep != 7 ||
		(memory->Read(volumeEnvelopeAddress) & 0x7) == 0 ||
		channel.envelopeEnabled == false) {

		return;
	}

	channel.envelope--;

	if (channel.envelope <= 0) {

		channel.envelope = memory->Read(volumeEnvelopeAddress) & 7;

		int direction = -1;
		if (BitTest(memory->Read(volumeEnvelopeAddress), 3) == true) {
			direction = 1;
		}

		byte newAmp = channel.amp + direction;

		if (newAmp >= 0 && newAmp <= 15) {
			channel.amp = newAmp;
		}
		else {
			channel.envelopeEnabled = false;
		}
	}
}

void APU::squareBuffer(Channel & channel) {

	channel.pcc--;
	if (channel.pcc <= 0) {
		channel.pcc = kClocksPerSample;

		if (channel.enabled == false ||
			channel.duty == false/* ||
			BitTest(memory->Read(0xFF26), 1) == true ||
			BitTest(memory->Read(0xFF25), 0) == true ||
			BitTest(memory->Read(0xFF25), 4) == true*/) {

			channel.buffer.push_back(0);
		}
		else {

			channel.buffer.push_back(channel.amp);
		}
	}
}

void APU::waveTimer(Channel & channel, word FrequencyHigh, word FrequencyLow) {

	if (channel.timer <= 0) {

		word freq = (memory->Read(FrequencyHigh) & 0x7) << 8 | memory->Read((FrequencyLow));
		int hz = 131072 / (2048 - freq); // hz, number of times per second the wave duty will repeat
		channel.timer = ((kClocksPerFrame * kFrameRate) / hz) / 16; // 70224 clocks per frame, 60 frames per second, 16 parts of wave data

		channel.waveIndex++;
		channel.waveIndex %= 32;

	}
	else {
		channel.timer--;
	}
}

void APU::wave(Channel & channel) {

	channel.pcc--;
	if (channel.pcc <= 0) {
		channel.pcc = kClocksPerSample;

		if (channel.enabled == false ||
			BitTest(memory->Read(Address::Channel3SoundOnOFF), 7) == false ||
			BitTest(memory->Read(Address::SoundOutputSelection), 2) == false && BitTest(memory->Read(Address::SoundOutputSelection), 6) == false) {

			channel.buffer.push_back(0);
		}
		else {

			byte wave = memory->Read(Address::Channel3WavePatternRam + (channel.waveIndex / 2));
			if (channel.waveIndex % 2 == 0) {
				wave &= 0xF;
			}
			else {
				wave >>= 4;
			}

			byte volume = (memory->Read(Address::Channel3SelectOutputLevel) >> 5) & 3;
			if (volume != 0) {
				wave >>= (volume - 1);
			}
			else {
				wave >>= 4;
			}

			channel.buffer.push_back(wave);
		}
	}
}

void APU::noiseTimer(Channel & channel) {

	if (channel.timer <= 0) {

		byte s = memory->Read(Address::Channel4PolynomialCounter) & 0xF0;
		float r = memory->Read(Address::Channel4PolynomialCounter) & 0x07;

		if (r == 0) {
			r = 0.5f;
		}

		float hz = 526680.0f / r / pow(2, s + 1);
		channel.timer = ((kClocksPerFrame * kFrameRate) / hz);

		byte xorRes = (channel.lfsr & 0x1) ^ ((channel.lfsr & 0x2) >> 1);
		channel.lfsr >>= 1;
		channel.lfsr |= xorRes << 14;

		if (BitTest(memory->Read(Address::Channel4PolynomialCounter), 3) == true) {
			channel.lfsr |= xorRes << 6;
			channel.lfsr &= 0x7F;
		}
	}
	else {
		channel.timer--;
	}
}

void APU::noiseBuffer(Channel & channel) {

	channel.pcc--;
	if (channel.pcc <= 0) {
		channel.pcc = kClocksPerSample;

		if (channel.enabled == false ||
			channel.duty == false) {

			channel.buffer.push_back(0);
		}
		else {

			channel.buffer.push_back(channel.amp);
		}
	}
}

void APU::fillDisplayBuffer(Channel & channel) {

	channel.displayBuffer.resize(channel.buffer.size());
	for (size_t i = 0; i < channel.buffer.size(); i++) {
		channel.displayBuffer[i] = channel.buffer[i];
	}
	channel.buffer.clear();
}
