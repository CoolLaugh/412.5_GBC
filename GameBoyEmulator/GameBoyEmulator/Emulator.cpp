#include "Emulator.h"

Emulator::Emulator() {

	m_GameBank = new unsigned char[0x200000];
	m_Rom = new unsigned char[0x10000];

	LoadRom("./Tetris.gb");
}

Emulator::~Emulator() {

}

bool Emulator::LoadRom(const std::string fileName) {

	memset(m_Rom, 0, sizeof(m_Rom));
	memset(m_GameBank, 0, sizeof(m_GameBank));

	std::ifstream romFile (fileName, std::ios::in, std::ios::binary);
	romFile.read((char*)m_GameBank, 0x200000);
	romFile.close();

	memcpy(m_Rom, m_GameBank, 0x8000);
	cpu.m_rom = m_Rom;

	return true;
}

void Emulator::Update() {

	for (size_t i = 0; i < 70221; i++) {

		cpu.ExecuteOpcode();
	}

}
