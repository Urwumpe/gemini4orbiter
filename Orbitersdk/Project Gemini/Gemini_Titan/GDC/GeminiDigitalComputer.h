#pragma once
#include <Subsystems.h>
#include <Electrics.h>

namespace gemini {

	const double GDC_PHASE_TIME = 28E-6;

	const unsigned short opHOP = 0x0;
	const unsigned short opDIV = 0x1;
	const unsigned short opPRO = 0x2;
	const unsigned short opRSU = 0x3;
	const unsigned short opADD = 0x4;
	const unsigned short opSUB = 0x5;
	const unsigned short opCLA = 0x6;
	const unsigned short opAND = 0x7;
	const unsigned short opMPY = 0x8;
	const unsigned short opTRA = 0x9;
	const unsigned short opSHF = 0xA;
	const unsigned short opTMI = 0xB;
	const unsigned short opSTO = 0xC;
	const unsigned short opSPQ = 0xD;
	const unsigned short opCLD = 0xE;
	const unsigned short opTNZ = 0xF;

	enum MDOPERATION {
		mdIDLE,
		mdMULTIPLY,
		mdDIVIDE
	};

	class GDC: public core::eps::ElectricSubsystem {
	private:
		///Select if this digital computer is the pre-ATM or the ATM version
		bool bATMVersion;
		/// Sector - Address - Syllable
		short memory[16][256][3];
		
		unsigned short usSectorRegister;
		unsigned short usSyllableRegister;
		///Memory Address Register
		unsigned short usMAR;
		///Instruction Address Register
		unsigned short usIAR;
		///Operation Register
		unsigned short usOR;
		long lAccumulator;
		long lPQ;
		///Remainder of a phase clock cycle, added to the timestep calculations of the next prestep
		double fClockResidual;
		/// A, B, C, D, E
		unsigned short usMainPhase;
		/// H1, H2, H3, H4
		unsigned short usIOPhase;

		unsigned short usMDTimer;
		long lMD1, lMD2;
		MDOPERATION MDOp;

		unsigned short usLastOperand;

		unsigned short usSenseDriver;
		unsigned long ulHOPRegister;
		/// Use the halfword mode
		bool bHalfWordMode;

		bool bResidualBit;

		double fElapsedTimer;


		unsigned short usHOPSector, usHOPAddress, usHOPSyllable, usHOPValue;
		
		//Find solution for AC power
		core::eps::InputPort* powerOnOff;
		core::eps::OutputPort* powerControl;

		//Power supply from IGC
		core::eps::InputPort* power28Vdc;
		core::eps::InputPort* power27Vdc;
		core::eps::InputPort* powerN27Vdc;
		core::eps::InputPort* power20Vdc;
		core::eps::InputPort* power9Vdc;
		core::eps::InputPort* powerLossSensing;

		core::eps::InputPort* discreteIn[40];

		core::eps::OutputPort* discreteOut[30];
	protected:
		/*! @brief Execute a single cycle
		 */
		void Execute();
		/*! @brief Execute a number of cycles
		 *  @param[in] numberOfCycles The number of cycles to process
		 */
		void Execute(unsigned int numberOfCycles);
		/*! Read a yaASM file and parse it directly into OBC memory
		TODO Should better get moved into a factory class that also writes ATM tape data for the mission
		TODO Should better be started by scenario command as well
		@param[in] sourcefile The name of a sourcefile without the ".obc" extension in the "Config/Gemini/OBC" folder
		*/
		void LoadOBC(const std::string& sourcefile);

		void PhaseB();
		void PhaseC();
		void PhaseD();

		bool IsControlPower();
		unsigned short ReadSyllable() const;
		unsigned short ReadSyllable(unsigned short addr) const;
		long ReadDataWord() const;
		void WriteDataWord();
		void WriteDataWord(long lValue);
	public:
		GDC(core::AbstractSubsystemContainer &parent, const std::string &name);
		virtual ~GDC();
		virtual void PreStep(double fSimT, double fSimDT, double fMJD);
		virtual void PostStep(double fSimT, double fSimDT, double fMJD);

		inline long GetAccumulatorRegister() const 
		{ 
			return lAccumulator; 
		};

		inline long GetPQRegister() const
		{
			return lPQ;
		};

		inline unsigned short GetOperationRegister() const
		{
			return usOR;
		};

		inline unsigned short GetMemoryAddressRegister() const
		{
			return usMAR;
		};

		inline unsigned short GetInstructionAddressRegister() const
		{
			return usIAR;
		};

		inline unsigned short GetSectorRegister() const
		{
			return usSectorRegister;
		};

		inline unsigned short GetSyllableRegister() const
		{
			return usSyllableRegister;
		};

		virtual void SetMemory(unsigned short sector, unsigned short address, unsigned short usSyllable, unsigned short word);
	};
};