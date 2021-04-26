#pragma once
#include "GeminiDigitalComputer.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>

namespace obc {

	class Symbol;

	class Symbol {
		std::string name;
		bool incomplete;
		unsigned short module;
		unsigned short sector;
		unsigned short syllable;
		unsigned short addr;
		long value;

		Symbol* reference;

	public:
		Symbol()
		{
			incomplete = true;
			name = "";
			module = 0;
			sector = 0;
			syllable = 0;
			addr = 0;
			value = 0;
		}

		Symbol(const std::string& _name, unsigned short m, unsigned short s, unsigned short syll, unsigned short a)
		{
			name = _name;
			incomplete = false;
			module = m;
			sector = s;
			syllable = syll;
			addr = a;
			value = 0;
		}

		Symbol(const std::string& _name, Symbol* refsym)
		{
			name = _name;
			incomplete = true;
			reference = refsym;
			value = 0;
		}

		~Symbol()
		{

		}

		inline const std::string& GetName() const
		{
			return name;
		}

		inline void SetName(const std::string& n) 
		{
			name = n;
		}

		inline bool IsIncomplete() const
		{
			return incomplete;
		}

		inline void Define(unsigned short m, unsigned short s1, unsigned short s2, unsigned short a)
		{
			if (incomplete)
			{
				incomplete = false;
				module = m;
				sector = s1;
				syllable = s2;
				addr = a;
				reference = NULL;
			}		
		}

		inline unsigned short GetModule() const 
		{
			return module;
		}

		inline void SetModule(unsigned short m)
		{
			module = m;
		}

		inline unsigned short GetSector() const 
		{
			return sector;
		}

		inline void SetSector(unsigned short s)
		{
			sector = s;
		}

		inline unsigned short GetSyllable() const 
		{
			return syllable;
		}

		inline void SetSyllable(unsigned short s)
		{
			syllable = s;
		}

		inline unsigned short GetAddress() const
		{
			return addr;
		}

		inline void SetAddress(unsigned short a)
		{
			addr = a;
		}

		inline Symbol* GetReference() const
		{
			return reference;
		}

		inline void SetReference(Symbol* refsym) 
		{
			reference = refsym;
		}

		inline long GetValue() const {
			return value;
		}

		inline void SetValue(long v) {
			value = v;
		}

		inline std::string ToString() const
		{
			std::ostringstream oss;
			if (incomplete)
			{
				oss << "(";
				if (reference != NULL)
				{
					oss << reference->GetName();
				}
				else {
					oss << "TBD";
				}
				oss << ")";
			}
			else
			{
				oss << std::oct << std::setfill('0') << std::setw(1) << module
					<< "-" << std::setw(2) << sector
					<< "-" << std::setw(1) << syllable
					<< "-" << std::setw(3) << addr;
			}
			
			return oss.str();
		}
	};


	class OBCAssembler 
	{
		gemini::GDC* target;

		std::string obcFilePath;
		std::string obcFileName;
		std::ofstream listing;
		unsigned long linenumber;

		std::string symbol;
		std::string opcode;
		std::string operand;
		std::string comment;

		std::map<std::string, Symbol*> symbolDict;
		std::vector<Symbol*> symTable;

		unsigned short usCodeModule , usDataModule;
		unsigned short usCodeSector, usDataSector;
		unsigned short usCodeSyllable, usDataSyllable;
		unsigned short usCodeAddress, usDataAddress;

		void SplitLine(const std::string& line);
		void ParseLinePass1(const std::string& line);
		void ParseLinePass3(const std::string& line);
		bool ParseAssemblerStatements();
		void ResetAddressSpace();
		bool DeclareConstants();
		void DeclareCodeSymbols();
		bool DefineConstants();
		void DefineCodeSymbols();
		void ReplaceSpecialOpcodes();
		bool IsDefined(const std::string sym) const;
		void DefineIncompleteSymbol(const std::string& symname, const std::string& refsymbol);
		void DefineSymbol(const std::string& symname, unsigned short module, unsigned short sector, unsigned short syllable, unsigned short addr);
		void DefineSymbol(const std::string& symname);
		void ResolveMissingSymbols();
		void ListData(const std::string& typ, long v);
		void ListInstruction(const std::string& opcode, unsigned short usInstr, unsigned short usOperand);
		void WriteData(long v);
		void WriteCode(unsigned short usInstruction);
	public:
		OBCAssembler();
		OBCAssembler(gemini::GDC* tgt);

		void Assemble(const std::string& filename);
		void Assemble(const std::string& filename, unsigned short usSector, unsigned short usSyllable, unsigned short usAddress);
	};
};