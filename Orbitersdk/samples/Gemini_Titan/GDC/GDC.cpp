#include "GeminiDigitalComputer.h"
#include <cmath>

namespace gemini
{

	const char GDC_PHASES[5] = { 'A', 'B', 'C', 'D', 'E' };
	const char* GDC_OR_NAMES[16] = {
		"HOP", "DIV", "PRO", "RSU", "ADD", "SUB", "CLA", "AND",
		"MPY", "TRA", "SHF", "TMI", "STO", "SPQ", "CLD", "TNZ"
	};

	GDC::GDC(core::AbstractSubsystemContainer &host, const std::string& name)
		: ElectricSubsystem(host, name)
	{
		usMainPhase = 0;
		usIOPhase = 0;
		usSectorRegister = 0;
		usSyllableRegister = 2;
		usSenseDriver = 0;
		usMAR = 0;
		usIAR = 0;
		usOR = 0;
		lAccumulator = 0;
		lPQ = 0;
		lMD1 = lMD2 = 0;
		usMDTimer = 0;

		powerOnOff = CreateInput("powerOff");
		powerControl = CreateOutput("XCEP");
		power27Vdc = CreateInput("XSP27VDC");
		powerN27Vdc = CreateInput("XSM27VDC");
		power28Vdc = CreateInput("XSP28VDC");
		power9Vdc = CreateInput("XSP9VDC");
		powerLossSensing = CreateInput("XQBND");

		
	}

	GDC::~GDC()
	{

	}

	void GDC::Execute()
	{
		switch (usMainPhase) {
		case 0:
			//PHASE A
			//Decode instruction
			usIAR = ReadSyllable();
			usOR = (usIAR >> 9) & 0xF;
			usLastOperand = usIAR & 0x1FF;
			bResidualBit = ((usIAR >> 8) & 0x1);
			break;
		case 1:
			//PHASE B

			PhaseB();
			break;
		case 2:
			//PHASE C
			PhaseC();

			break;
		case 3:
			//PHASE D
			PhaseD();
			break;
		case 4:
			//PHASE E
			usMAR = usIAR;
			break;
		default:
			//Phase Error
			break;
		}

		++usMainPhase;
		if (usMainPhase > 4)
		{
			usMainPhase = 0;
		}

		++usIOPhase;
		if (usIOPhase > 3)
		{
			usIOPhase = 0;
		}
	}

	bool GDC::IsControlPower()
	{
		return true;
	}

	void GDC::PreStep(double fSimT, double fDeltaT, double fMJD)
	{
		if (IsControlPower())
		{
			fElapsedTimer += fDeltaT;

			double T = fDeltaT + fClockResidual;

			long phasecount = static_cast<long>(T / GDC_PHASE_TIME);

			fClockResidual = T - phasecount * GDC_PHASE_TIME;
			long count = phasecount;

			while (count > 0) {
				Execute();
				--count;
			}
		}

		/*sprintf_s(oapiDebugString(), 255, "GDC HOP=%09o (ADR=%02o-%1o-%03o HWM=%1d VAL=%05o) ACC=%09o PQ=%09o | %03o %3s %03o",
			ulHOPRegister, usHOPSector, usHOPSyllable, usHOPAddress, bHalfWordMode ? 1 : 0, (usHOPValue & 0x1FFF),
			static_cast<unsigned long>(lAccumulator & 0x3FFFFFF),
			static_cast<unsigned long>(lPQ & 0x3FFFFFF),
			usIAR, GDC_OR_NAMES[usOR], usLastOperand);*/
		
	}

	void GDC::PostStep(double fSimT, double fDeltaT, double fMJD)
	{

	}

	unsigned short GDC::ReadSyllable() const
	{
		unsigned short value = memory[usSectorRegister][usMAR][usSyllableRegister] & 0x1FFF;
		return value;
	}

	unsigned short GDC::ReadSyllable(unsigned short addr) const
	{
		unsigned short value = memory[usSectorRegister][addr][usSyllableRegister] & 0x1FFF;
		return value;
	}

	void GDC::PhaseB()
	{
		unsigned short usTemp;
		usTemp = (usMAR + 1) & 0xFF;
		usMAR = usIAR & 0xFF;
		if (usOR == opTRA || usOR == opTNZ || usOR == opTMI)
		{
			usIAR = usMAR;
		}
		else
		{
			usIAR = usTemp;
		}

		switch (usOR)
		{
		case opMPY:
			MDOp = mdMULTIPLY;
			lMD1 = lAccumulator;
			usMDTimer = 3;
			break;
		case opDIV:
			MDOp = mdDIVIDE;
			lMD1 = lAccumulator;
			usMDTimer = 6;
			break;
		}
	}

	void GDC::PhaseC()
	{
		switch (usOR) {
		case opHOP:
			ulHOPRegister = static_cast<unsigned long>(ReadDataWord());
			break;
		case opCLA:
			lAccumulator = ReadDataWord();
			break;
		case opADD:
			lAccumulator += ReadDataWord();
			break;
		case opSUB:
			lAccumulator -= ReadDataWord();
			break;
		case opRSU:
			lAccumulator = ReadDataWord() - lAccumulator;
			break;
		case opAND:
			lAccumulator &= ReadDataWord();
			break;

		case opTRA:
		case opTNZ:
		case opTMI:
			break;
		case opMPY:
		case opDIV:
			lMD2 = ReadDataWord();
			break;
		}

	}
	void GDC::PhaseD()
	{
		switch (usOR) {
		case opHOP:
			usHOPAddress = ulHOPRegister & 0xFF;
			usIAR = usHOPAddress;
			if (ulHOPRegister & 0x100)
			{
				usHOPSector = 15;

			}
			else
			{
				usHOPSector = (ulHOPRegister >> 10) & 0xF;
			}
			usSectorRegister = usHOPSector;
			usHOPSyllable = (ulHOPRegister >> 15) & 0x3;
			usSyllableRegister = usHOPSyllable;
			bHalfWordMode = (ulHOPRegister & 0x200) > 0;
			usHOPValue = ReadSyllable(usHOPAddress);
			break;
		case opSTO:
			WriteDataWord();
			break;
		case opSPQ:
			WriteDataWord(lPQ);
			break;
		}

		//Simple implementation: More correct would be iterating through two shift registers every phase
		if (usMDTimer > 0)
		{
			switch (MDOp)
			{
			case mdMULTIPLY:

				usMDTimer--;
				if (usMDTimer == 0)
					lPQ = lMD1 * lMD2;
				break;
			case mdDIVIDE:
				usMDTimer--;
				if (usMDTimer == 0)
					lPQ = lMD1 / lMD2;
				break;
			}

			if (usMDTimer == 0)
			{
				MDOp = mdIDLE;
				lMD1 = 0;
				lMD2 = 0;
			}
		}


	}

	long GDC::ReadDataWord() const
	{
		if (bHalfWordMode)
		{
			long value = memory[usSectorRegister][usMAR][usSyllableRegister] & 0x1FFF;
			return value;
		}
		else
		{
			long value1 = memory[usSectorRegister][usMAR][0] & 0x1FFF;
			long value2 = memory[usSectorRegister][usMAR][1] & 0x1FFF;
			long value = value2 << 13 | value1;
			if (value & 0x2000000)
			{
				//Fill two complement
				value |= 0xFC000000;
			}
			return value;
		}
	}


	void GDC::WriteDataWord()
	{
		WriteDataWord(lAccumulator);
	}


	void GDC::WriteDataWord(long lValue)
	{
		unsigned short word1 = lValue & 0x1FFF;
		unsigned short word2 = (lValue >> 13) & 0x1FFF;
		if (bHalfWordMode)
		{
			if (usSyllableRegister != 2)
				SetMemory(usSectorRegister, usMAR, usSyllableRegister, word1);
		}
		else
		{
			SetMemory(usSectorRegister, usMAR, 0, word1);
			SetMemory(usSectorRegister, usMAR, 1, word2);
		}
	}


	void GDC::SetMemory(unsigned short sector, unsigned short address, unsigned short usSyllable, unsigned short word)
	{
		if (sector < 16 && address < 256 && usSyllable < 3)
		{
			memory[sector][address][usSyllable] = word & 0x1FFF;
		}
		else
		{
			static char buffer[255];
			sprintf_s(buffer, 255, "[GEMINI %s] Illegal memory address in SetMemory: %02o-%1o-%03o", host.GetHostName().c_str(), sector, usSyllable, address);
		}
	}


}