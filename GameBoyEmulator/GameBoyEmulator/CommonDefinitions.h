#pragma once
#include <string>
#include <vector>

typedef unsigned char byte;
typedef unsigned short word;

const std::string TitleName = "412.5_GBC";
const int ScreenWidth = 160;
const int ScreenHeight = 144;
const int BackgroundLayerSize = 256;
const int DefaultDisplayScale = 4;
const int FrameRate = 60; // real gameboy hardware is 59.72 but 60 is used because of SFML and it matches most monitors
const int ClocksPerFrame = 70224;
const int ClocksPerScanLine = 456;
const int AudioFrequency = 44100;
const int ClocksPerSample = 95; // sound is 44100 hz thus ClocksPerFrame * Framerate / 44100 = 95

struct Address {
	typedef enum {

		CGBFlag = 0x0143,
		CartridgeType = 0x0147,
		RomSize = 0x0148,
		RamSize = 0x0149,

		TilePattern0 = 0x8000,
		TilePattern1 = 0x8800,

		BGWTileInfo0 = 0x9800,
		BGWTileInfo1 = 0x9C00,

		SpriteAttributes = 0xFE00,

		Joypad = 0xFF00,
		SerialTransfer = 0xFF01,
		SIOControl = 0xFF02,
		DIVRegister = 0xFF04,
		Timer = 0xFF05,
		TimerModulo = 0xFF06,
		TimerControl = 0xFF07,
		InteruptFlag = 0xFF0F,

		Channel1Sweep = 0xFF10,
		Channel1SoundLengthWavePatternDuty = 0xFF11,
		Channel1VolumeEnvlope = 0xFF12,
		Channel1FrequencyLow = 0xFF13,
		Channel1FrequencyHigh = 0xFF14,

		Channel2Tone = 0xFF16,
		Channel2VolumeEnvlope = 0xFF17,
		Channel2FrequencyLow = 0xFF18,
		Channel2FrequencyHigh = 0xFF19,

		Channel3SoundOnOFF = 0xFF1A,
		Channel3SoundLength = 0xFF1B,
		Channel3SelectOutputLevel = 0xFF1C,
		Channel3FrequencyLow = 0xFF1D,
		Channel3FrequencyHigh = 0xFF1E,
		Channel3WavePatternRam = 0xFF30,

		Channel4SoundLength = 0xFF20,
		Channel4VolumeEnvlope = 0xFF21,
		Channel4PolynomialCounter = 0xFF22,
		Channel4Counter = 0xFF23,
		ChannelControl = 0xFF24,
		SoundOutputSelection = 0xFF25,
		SoundOnOFF = 0xFF26,

		LCDC = 0xFF40,
		LCDCStatus = 0xFF41,
		ScrollY = 0xFF42,
		ScrollX = 0xFF43,
		LY = 0xFF44,
		LYCompare = 0xFF45,
		DMA = 0xFF46,
		BGWPalette = 0xFF47,
		OBJPalette0 = 0xFF48,
		OBJPalette1 = 0xFF49,
		WindowY = 0xFF4A,
		WindowX = 0xFF4B,
		PrepareSpeedSwitch = 0xFF4D,
		VRAMBank = 0xFF4F,
		InfraredPort = 0xFF56,
		GBColorBackgroundPaletteIndex = 0xFF68,
		GBColorBackgroundPaletteData = 0xFF69,
		GBColorSpritePaletteIndex = 0xFF68,
		GBColorSpritePaletteData = 0xFF69,
		WramBank = 0xFF70,
		InteruptEnable = 0xFFFF

	} Type;
};


enum Bits {
	b0 = (1 << 0),
	b1 = (1 << 1),
	b2 = (1 << 2),
	b3 = (1 << 3),
	b4 = (1 << 4),
	b5 = (1 << 5),
	b6 = (1 << 6),
	b7 = (1 << 7)
};

bool BitTest(byte value, int bit);
bool BitTestReverse(byte value, int bit); // order of bits is reversed
void BitSet(byte& value, int bit);
void BitReset(byte& value, int bit);

bool BitTest(word value, int bit);

std::string ToHex(byte value);


static std::string OpcodeNames[] = {	"NOP", "LD16 BC", "A->(BC)", "INC16 BC" , "INC B" , "DEC B" , "LD B" , "RLC A" , "SP->(NN)" , "ADDHL BC" , "(BC)->A" , "DEC16 BC" , "INC C" , "DEC C" , "LD C" , "RRC A", // 0
										"STOP", "LD16 DE", "(DE)<-A" , "INC16 DE" , "INC D" , "DEC D" , "LD D" , "RL A" , "JR" , "ADDHL DE" , "A<-(HL)" , "DEC16 DE" , "INC E" , "DEC E" , "LD E" , "RR A",
										"JRnz", "LD16 HL", "(HL)<-A INC HL" , "INC16 HL" , "INC H" , "DEC H" , "LD H" , "DAA" , "JRz" , "ADDHL HL" , "A<-(HL) INC HL" , "DEC16 HL" , "INC L" , "DEC L" , "LD L" , "CPL", // 2
										"JRnc", "LD16 SP", "(HL)<-A DEC HL" , "INC16 SP" , "INC (HL)" , "DEC (HL)" , "(HL)<-N" , "SCF" , "JRc" , "ADDHL SP" , "A<-(HL) DEC HL" , "DEC16 SP" , "INC A" , "DEC A" , "LD A" , "CCF",
										"LD B<-B", "LD B<-C", "LD B<-D" , "LD B<-E" , "LD B<-H" , "LD B<-L" , "LD B<-(HL)" , "LD B<-A" , "LD C<-B" , "LD C<-C" , "LD C<-D" , "LD C<-E" , "LD C<-H" , "LD C<-L" , "LD C<-(HL)" , "LD C<-A", // 4
										"LD D<-B", "LD D<-C", "LD D<-D" , "LD D<-E" , "LD D<-H" , "LD D<-L" , "LD D<-(HL)" , "LD D<-A" , "LD E<-B" , "LD E<-C" , "LD E<-D" , "LD E<-E" , "LD E<-H" , "LD E<-L" , "LD E<-(HL)" , "LD E<-A",
										"LD H<-B", "LD H<-C", "LD H<-D" , "LD H<-E" , "LD H<-H" , "LD H<-L" , "LD H<-(HL)" , "LD H<-A" , "LD L<-B" , "LD L<-C" , "LD L<-D" , "LD L<-E" , "LD L<-H" , "LD L<-L" , "LD L<-(HL)" , "LD L<-A", // 6
										"(HL)<-B", "(HL)<-C", "(HL)<-D" , "(HL)<-E" , "(HL)<-H" , "(HL)<-L" , "HALT" , "(HL)<-A" , "LD A<-B" , "LD A<-C" , "LD A<-D" , "LD A<-E" , "LD A<-H" , "LD A<-L" , "LD A<-(HL)" , "LD A<-A",
										"ADD B", "ADD C", "ADD D" , "ADD E" , "ADD H" , "ADD L" , "ADD (HL)" , "ADD A" , "ADDC B" , "ADDC C" , "ADDC D" , "ADDC E" , "ADDC H" , "ADDC L" , "ADDC (HL)" , "ADDC A", // 8
										"SUB B", "SUB C", "SUB D" , "SUB E" , "SUB H" , "SUB L" , "SUB (HL)" , "SUB A" , "SUBC B" , "SUBC C" , "SUBC D" , "SUBC E" , "SUBC H" , "SUBC L" , "SUBC (HL)" , "SUBC A",
										"AND B", "AND C", "AND D" , "AND E" , "AND H" , "AND L" , "AND (HL)" , "AND A" , "XOR B" , "XOR C" , "XOR D" , "XOR E" , "XOR H" , "XOR L" , "XOR (HL)" , "XOR A", // A
										"OR B", "OR C", "OR D" , "OR E" , "OR H" , "OR L" , "OR (HL)" , "OR A" , "CP B" , "CP C" , "CP D" , "CP E" , "CP H" , "CP L" , "CP (HL)" , "CP A",
										"RETnz", "POP BC", "JPnz" , "JP" , "CALLnz" , "PUSH BC" , "ADD N" , "RST00" , "RETz" , "RET" , "JPz" , "Extended: " , "CALLz" , "CALL" , "ADDC N" , "RST08", // C
										"RETnc", "POP DE", "JPnc" , "missing opcode" , "CALLnc" , "PUSH DE" , "SUB N" , "RST10" , "RETc" , "RETI" , "JPc" , "missing opcode" , "CALLc" , "missing opcode" , "missing opcode" , "RST18",
										"(0xFF00+N)<-A", "POP HL", "(0xFF00+C)<-A" , "missing opcode" , "missing opcode" , "PUSH HL" , "AND N" , "RST20" , "ADD SP sN" , "JPHL" , "(NN)<-A" , "missing opcode" , "missing opcode" , "missing opcode" , "XOR N" , "RST28", // E
										"A<-(0xFF00+N)", "POP AF", "A<-(0xFF00+C)" , "DI" , "missing opcode" , "PUSH AF" , "missing opcode" , "RST30" , "HL<-SP+sN" , "SP<-HL" , "A<-(NN)" , "EI" , "missing opcode" , "missing opcode" , "CP N" , "RST38" };

static std::string ExtendedOpcodeNames[] = {	"RLC B", "RLC C", "RLC D", "RLC E", "RLC H", "RLC L", "RLC (HL)", "RLC A", "RRC B", "RRC C", "RRC D", "RRC E", "RRC H", "RRC L", "RRC (HL)", "RRC A",
												"RL B", "RL C", "RL D", "RL E", "RL H", "RL L", "RL (HL)", "RL A", "RR B", "RR C", "RR D", "RR E", "RR H", "RR L", "RR (HL)", "RR A",
												"SLA B", "SLA C", "SLA D", "SLA E", "SLA H", "SLA L", "SLA (HL)", "SLA A", "SRA B", "SRA C", "SRA D", "SRA E", "SRA H", "SRA L", "SRA (HL)", "SRA A",
												"SWAP B", "SWAP C", "SWAP D", "SWAP E", "SWAP H", "SWAP L", "SWAP (HL)", "SWAP A", "SRL B", "SRL C", "SRL D", "SRL E", "SRL H", "SRL L", "SRL (HL)", "SRL A",
												"BIT B 0", "BIT C 0", "BIT D 0", "BIT E 0", "BIT H 0", "BIT L 0", "BIT (HL) 0", "BIT A 0", "BIT B 1", "BIT C 1", "BIT D 1", "BIT E 1", "BIT H 1", "BIT L 1", "BIT (HL) 1", "BIT A 1",
												"BIT B 2", "BIT C 2", "BIT D 2", "BIT E 2", "BIT H 2", "BIT L 2", "BIT (HL) 2", "BIT A 2", "BIT B 3", "BIT C 3", "BIT D 3", "BIT E 3", "BIT H 3", "BIT L 3", "BIT (HL) 3", "BIT A 3",
												"BIT B 4", "BIT C 4", "BIT D 4", "BIT E 4", "BIT H 4", "BIT L 4", "BIT (HL) 4", "BIT A 4", "BIT B 5", "BIT C 5", "BIT D 5", "BIT E 5", "BIT H 5", "BIT L 5", "BIT (HL) 5", "BIT A 5",
												"BIT B 6", "BIT C 6", "BIT D 6", "BIT E 6", "BIT H 6", "BIT L 6", "BIT (HL) 6", "BIT A 6", "BIT B 7", "BIT C 7", "BIT D 7", "BIT E 7", "BIT H 7", "BIT L 7", "BIT (HL) 7", "BIT A 7",
												"SET B 0", "SET C 0", "SET D 0", "SET E 0", "SET H 0", "SET L 0", "SET (HL) 0", "SET A 0", "SET B 1", "SET C 1", "SET D 1", "SET E 1", "SET H 1", "SET L 1", "SET (HL) 1", "SET A 1",
												"SET B 2", "SET C 2", "SET D 2", "SET E 2", "SET H 2", "SET L 2", "SET (HL) 2", "SET A 2", "SET B 3", "SET C 3", "SET D 3", "SET E 3", "SET H 3", "SET L 3", "SET (HL) 3", "SET A 3",
												"SET B 4", "SET C 4", "SET D 4", "SET E 4", "SET H 4", "SET L 4", "SET (HL) 4", "SET A 4", "SET B 5", "SET C 5", "SET D 5", "SET E 5", "SET H 5", "SET L 5", "SET (HL) 5", "SET A 5",
												"SET B 6", "SET C 6", "SET D 6", "SET E 6", "SET H 6", "SET L 6", "SET (HL) 6", "SET A 6", "SET B 7", "SET C 7", "SET D 7", "SET E 7", "SET H 7", "SET L 7", "SET (HL) 7", "SET A 7",
												"RES B 0", "RES C 0", "RES D 0", "RES E 0", "RES H 0", "RES L 0", "RES (HL) 0", "RES A 0", "RES B 1", "RES C 1", "RES D 1", "RES E 1", "RES H 1", "RES L 1", "RES (HL) 1", "RES A 1",
												"RES B 2", "RES C 2", "RES D 2", "RES E 2", "RES H 2", "RES L 2", "RES (HL) 2", "RES A 2", "RES B 3", "RES C 3", "RES D 3", "RES E 3", "RES H 3", "RES L 3", "RES (HL) 3", "RES A 3",
												"RES B 4", "RES C 4", "RES D 4", "RES E 4", "RES H 4", "RES L 4", "RES (HL) 4", "RES A 4", "RES B 5", "RES C 5", "RES D 5", "RES E 5", "RES H 5", "RES L 5", "RES (HL) 5", "RES A 5",
												"RES B 6", "RES C 6", "RES D 6", "RES E 6", "RES H 6", "RES L 6", "RES (HL) 6", "RES A 6", "RES B 7", "RES C 7", "RES D 7", "RES E 7", "RES H 7", "RES L 7", "RES (HL) 7", "RES A 7" };

static std::string SFMLKeyNames[] = {	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
										"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
										"U", "V", "W", "X", "Y", "Z", "Num0",  "Num1",  "Num2",  "Num3",
										"Num4",  "Num5",  "Num6", "Num7", "Num8", "Num9", "Escape", "LControl", "LShift", "LAlt",
										"LSystem", "RControl", "RShift", "RAlt", "RSystem", "Menu", "LBracket", "RBracket", "Semicolon", "Comma",
										"Period", "Quote", "Slash", "Backslash", "Tilde", "Equal", "Hyphen", "Space", "Enter", "Backspace",
										"Tab", "PageUp", "PageDown", "End", "Home", "Insert", "Delete", "Add", "Subtract", "Multiply",
										"Divide", "Left", "Right", "Up", "Down", "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4",
										"Numpad5", "Numpad6", "Numpad7", "Numpad8", "Numpad9", "F1", "F2", "F3", "F4", "F5",
										"F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15",
										"Pause" };