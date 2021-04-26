#include "OBCAssembler.h"
#include <fstream>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace obc 
{

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

	std::string RESERVED_WORDS[28] = { "HOP", "DIV", "PRO", "RSU", "ADD", "SUB", "CLA", "AND",
		"MPY", "TRA", "SHF", "TMI", "STO", "SPQ", "CLD", "TNZ", 
		"SHL", "SHR", "NOP", "HOPC", "CODE", "DATA", "DEC", "OCT", "SYN", "EQU", "HALF", "NORM" };

	std::string OPCODES[16] = { "HOP", "DIV", "PRO", "RSU", "ADD", "SUB", "CLA", "AND",
		"MPY", "TRA", "SHF", "TMI", "STO", "SPQ", "CLD", "TNZ"};

	OBCAssembler::OBCAssembler()
	{
		target = NULL;
	}

	OBCAssembler::OBCAssembler(gemini::GDC* tgt)
	{
		target = tgt;
	}

	void OBCAssembler::Assemble(const std::string& filename)
	{
		Assemble(filename, 0, 2, 0);
	}

	void OBCAssembler::Assemble(const std::string& filename, unsigned short usSector, unsigned short usSyllable, unsigned short usAddress)
	{
		char buffer[256];

		

		std::string listfilename;
		if (target == NULL)
			return;
		std::string::size_type pos = filename.find_last_of('\\');
		obcFilePath = filename.substr(0, pos + 1);
		obcFileName = filename.substr(pos + 1);
		listfilename = "list-" + obcFileName + ".txt";
		obcFileName.append(".obc");

		sprintf_s(buffer, 255, "[OBCASM] Assembling source file \"%s\" in \"%s\".", obcFileName.c_str(), obcFilePath.c_str());
		oapiWriteLog(buffer);
		sprintf_s(buffer, 255, "[OBCASM] Listing file is \"%s\".", listfilename.c_str());
		oapiWriteLog(buffer);
		//PASS 1: Gather symbols. Reserve memory for HOPC 
		//PASS 2: Validate code
		//PASS 3: Write instructions directly into target memory

		std::ifstream obcfile;
		

		obcfile.open(obcFilePath + obcFileName);
		if (!obcfile) {
			
			sprintf_s(buffer, 255, "[OBCASM] OBC source file \"%s\" not found in \"%s\".", obcFileName.c_str(), obcFilePath.c_str());
			oapiWriteLog(buffer);
			obcfile.close();
			return;
		}
		
		//PASS 1: SYMBOL GENERATION
		listing.open(listfilename);
		listing << "SOURCE PATH: " << obcFilePath << std::endl;
		listing << "SOURCE FILE: " << obcFileName << std::endl;
		listing << std::endl;
		listing << std::endl << "PASS 1: SYMBOL GENERATION" << std::endl << std::endl;
		ResetAddressSpace();
		linenumber = 1;
		while (obcfile)
		{
			obcfile.getline(buffer, 255);
			if (buffer[0] != '#')
			{
				ParseLinePass1(buffer);
			}
			
			linenumber++;
		}
		obcfile.close();
		//PASS 2: RESOLVE TEMPORARY REFERENCES
		listing << std::endl << "PASS 2: RESOLVE FORWARD REFERENCES" << std::endl;
		ResolveMissingSymbols();
		//PASS 3: CODE GENERATION
		listing << std::endl << "PASS 3: CODE GENERATION" << std::endl << std::endl;
		ResetAddressSpace();
		obcfile.open(obcFilePath + obcFileName);
		linenumber = 1;
		while (obcfile)
		{
			obcfile.getline(buffer, 255);
			if (buffer[0] != '#')
			{
				ParseLinePass3(buffer);
			}
			linenumber++;
		}
		obcfile.close();
		listing.close();
	}

	void OBCAssembler::ParseLinePass1(const std::string &line) 
	{
		SplitLine(line);
		if (!ParseAssemblerStatements())
		{
			if (!DeclareConstants())
			{
				ReplaceSpecialOpcodes();
				DeclareCodeSymbols();
			}
		}
	}

	void OBCAssembler::ParseLinePass3(const std::string &line)
	{
		SplitLine(line);
		if (!ParseAssemblerStatements())
		{
			if (!DefineConstants())
			{
				ReplaceSpecialOpcodes();
				DefineCodeSymbols();

			}
		}
	}

	bool isReservedWord(const std::string& x) {
		for (unsigned int i = 0; i < 28; ++i)
		{
			if (x == RESERVED_WORDS[i])
			{
				return true;
			}
		}
		return false;
	}

	void ParseLongAddressConstant(const std::string& addrs, unsigned short &module, unsigned short &sector, 
		unsigned short &syllable, unsigned short &addr) 
	{
		std::istringstream iss(addrs);

		iss >> std::oct >> module;
		iss.ignore(1);
		iss >> std::oct >> sector;
		iss.ignore(1);
		iss >> std::oct >> syllable;
		iss.ignore(1);
		iss >> std::oct >> addr;
	}

	bool OBCAssembler::ParseAssemblerStatements() 
	{
		if (opcode == "CODE")
		{
			listing << "*** CODE " << operand << "[" << linenumber << "]" << std::endl;
			ParseLongAddressConstant(operand, usCodeModule, usCodeSector, usCodeSyllable, usCodeAddress);
			return true;
		}
		else if (opcode == "DATA")
		{
			listing << "*** DATA " << operand << "[" << linenumber << "]" << std::endl;
			ParseLongAddressConstant(operand, usDataModule, usDataSector, usDataSyllable, usDataAddress);
			return true;
		}
		else if (opcode == "NORM")
		{
			listing << "*** NORM [" << linenumber << "]" << std::endl;
			return true;
		}
		else if (opcode == "HALF")
		{
			listing << "*** HALF [" << linenumber << "]" << std::endl;
			return true;
		}
		return false;
	}

	bool OBCAssembler::DeclareConstants()
	{
		long lData;
		if (opcode.empty())
		{
			if (!symbol.empty()) 
			{
				DefineSymbol(symbol, usDataModule, usDataSector, usDataSyllable, usDataAddress);
				usDataAddress++;
			}
			return true;
		}
		else if (opcode == "DEC")
		{
			std::istringstream iss(operand);
			iss >> std::dec >> lData;
			if (!symbol.empty())
			{
				DefineSymbol(symbol, usDataModule, usDataSector, usDataSyllable, usDataAddress);
				symbolDict[symbol]->SetValue(lData);
			}
			usDataAddress++;
			return true;
		}
		else if (opcode == "OCT")
		{
			std::istringstream iss(operand);
			iss >> std::oct >> lData;
			if (!symbol.empty())
			{
				DefineSymbol(symbol, usDataModule, usDataSector, usDataSyllable, usDataAddress);
				symbolDict[symbol]->SetValue(lData);
			}
			usDataAddress++;
			return true;
		}
		else if (opcode == "HOPC")
		{
			unsigned short module, sector, syllable, addr;
			if (std::isdigit(operand[0]))
			{
				ParseLongAddressConstant(operand, module, sector, syllable, addr);
			}
			else 
			{
				DefineSymbol(operand);
			}

			if (!symbol.empty())
			{
				DefineSymbol(symbol, usDataModule, usDataSector, usDataSyllable, usDataAddress);
					
			}
			usDataAddress++;
			return true;
		}
		else if (opcode == "EQU")
		{
			if (!symbol.empty())
			{
				DefineSymbol(symbol, usDataModule, usDataSector, usDataSyllable, usDataAddress);
			}

			usDataAddress++;
			return true;
		}
		else if (opcode == "SYN") 
		{
			if (!symbol.empty())
			{
				DefineIncompleteSymbol(symbol, operand);
			}
			return true;
		}
		return false;
	}

	long MakeHOPC(unsigned short usModule, unsigned short usSector, unsigned short usSyllable, unsigned short usAddr)
	{
		long x = 0;
		if (usSector == 15) {
			x = 0x20000;
		}
		return x | (usSyllable & 0x3) << 15 | (usSector & 0xF) << 10 | (usAddr & 0x1FF);
	}

	bool OBCAssembler::DefineConstants()
	{
		long lData;
		if (opcode.empty())
		{
			if (!symbol.empty())
			{
				ListData("OCT", 0);
				WriteData(0);
				usDataAddress++;
			}
			return true;
		}
		else if (opcode == "DEC")
		{
			std::istringstream iss(operand);
			iss >> std::dec >> lData;
			if (!symbol.empty())
			{
				symbolDict[symbol]->SetValue(lData);
			}
			ListData("OCT", lData);
			WriteData(lData);
			usDataAddress++;
			return true;
		}
		else if (opcode == "OCT")
		{
			std::istringstream iss(operand);
			iss >> std::oct >> lData;
			if (!symbol.empty())
			{
				symbolDict[symbol]->SetValue(lData);
			}
			ListData("OCT", lData);
			WriteData(lData);
			usDataAddress++;
			return true;
		}
		else if (opcode == "HOPC")
		{
			unsigned short module, sector, syllable, addr;
			if (std::isdigit(operand[0]))
			{
				ParseLongAddressConstant(operand, module, sector, syllable, addr);
			}
			else
			{
				module = symbolDict[operand]->GetModule();
				sector = symbolDict[operand]->GetSector();
				syllable = symbolDict[operand]->GetSyllable();
				addr = symbolDict[operand]->GetAddress();
			}
			lData = MakeHOPC(module, sector, syllable, addr);
			ListData("HOPC", lData);
			WriteData(lData);
			usDataAddress++;
			return true;
		}
		else if (opcode == "EQU")
		{
			lData = symbolDict[operand]->GetValue();
			ListData("OCT", lData);
			WriteData(lData);
			usDataAddress++;
			return true;
		}
		else if (opcode == "SYN")
		{
			return true;
		}
		return false;
	}

	void OBCAssembler::DeclareCodeSymbols()
	{
		static char buffer[256];
		bool bValid = false;
		for (unsigned int i = 0; i < 16; ++i) {
			if (OPCODES[i] == opcode) {
				bValid = true;
				break;
			}
		}
		if (bValid)
		{
			if (!symbol.empty()) {
				DefineSymbol(symbol, usCodeModule, usCodeSector, usCodeSyllable, usCodeAddress);
			}
			usCodeAddress++;	
		}
		else {
			sprintf_s(buffer, 255, "[OBCASM] Line %d: Illegal opcode \"%s\"", linenumber, opcode.c_str());
			oapiWriteLog(buffer);
		}
	}

	unsigned short MakeInstruction(unsigned short usOpcode, unsigned short usOperand)
	{
		return (usOpcode & 0xF) << 10 | (usOperand & 0x1FF);
	}

	void OBCAssembler::DefineCodeSymbols()
	{
		unsigned short usOpcode, usOperand;
		static char buffer[256];
		bool bValid = false;
		for (usOpcode = 0; usOpcode < 16; ++usOpcode) {
			if (OPCODES[usOpcode] == opcode) {
				bValid = true;
				break;
			}
		}
		if (bValid)
		{
			switch (usOpcode)
			{
			case opHOP:
			case opADD:
			case opAND:
			case opSUB:
			case opMPY:
			case opDIV:
			case opTRA:
			case opCLA:
			case opRSU:
			case opTMI:
			case opSTO:
			case opSPQ:
			case opTNZ:
				if (std::isdigit(operand[0])) {
					std::istringstream iss(operand);
					iss >> std::oct >> usOperand;
					usOperand = usOperand & 0x1FF;
				}
				else {
					usOperand = symbolDict[operand]->GetAddress() & 0xFF;
					if (symbolDict[operand]->GetSector() == 15 && usCodeSector != 15)
					{
						usOperand |= 0x100;
					}
				}
				break;
			case opPRO:
			case opCLD:
			case opSHF:
				if (std::isdigit(operand[0])) {
					std::istringstream iss(operand);
					iss >> std::oct >> usOperand;
					usOperand = usOperand & 0x1FF;
				}
				else {
					//Error
				}
			}
			ListInstruction(opcode, MakeInstruction(usOpcode, usOperand), usOperand);
			WriteCode(MakeInstruction(usOpcode, usOperand));
			usCodeAddress++;
		}
		else {
			sprintf_s(buffer, 255, "[OBCASM] Line %d: Illegal opcode \"%s\"", linenumber, opcode.c_str());
			oapiWriteLog(buffer);
		}
	}

	void OBCAssembler::ReplaceSpecialOpcodes()
	{
		static char buffer[256];
		//Understand special instructions
		if (opcode == "NOP") {
			opcode = "TRA";
			operand = "*+1";
		}
		else if (opcode == "SHL")
		{
			opcode = "SHF";
			if (operand == "1") 
			{
				operand = "30";
			}
			else if (operand == "2") {
				operand = "40";
			}
			else {
				sprintf_s(buffer, 255, "[OBCASM] Line %d:Illegal operand in simplified shift instruction : %s",
					linenumber, operand.c_str());
				oapiWriteLog(buffer);
			}

		}
		else if (opcode == "SHR")
		{
			opcode = "SHF";
			if (operand == "1")
			{
				operand = "21";
			}
			else if (operand == "2") {
				operand = "20";
			}
			else {
				sprintf_s(buffer, 255, "[OBCASM] Line %d:Illegal operand in simplified shift instruction : %s",
					linenumber, operand.c_str());
				oapiWriteLog(buffer);
			}

		}

		//Translate special operands
		if (operand[0] == '*') 
		{
			
			if (operand[1] == '+' || operand[1] == '-')
			{
				std::ostringstream oss;
				oss.str("");
				unsigned long offset = strtoul(operand.substr(2).c_str(), NULL, 8);
				long addr = usCodeAddress;
				if (operand[1] == '+') {
					addr += offset;
				}
				else {
					addr -= offset;
				}
				if (addr >= 0 && addr <= 255) {
					oss << std::oct << addr;
					operand = oss.str();
				}
				else {
					sprintf_s(buffer, 255, "[OBCASM] Line %d: IAR-relative operand points to illegal address: %s",
						linenumber, operand.c_str());
					oapiWriteLog(buffer);
				}
				
			}
			else
			{
				sprintf_s(buffer, 255, "[OBCASM] Line %d:Illegal operation in IAR-relative operand : %s",
					linenumber, operand.c_str());
				oapiWriteLog(buffer);
			}
		}
	}

	bool isWhiteSpace(char c) 
	{
		return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\b';
	}
	void OBCAssembler::ResetAddressSpace()
	{
		usCodeModule = 0;
		usDataModule = 0;
		usCodeSector = 0;
		usDataSector = 0;
		usCodeAddress = 0;
		usDataAddress = 0;
		usCodeSyllable = 2;
		usDataSyllable = 0;
	}

	void OBCAssembler::SplitLine(const std::string& line) 
	{
		bool bLHSIsReservedWord, skip;
		std::string fields[4];
		symbol = "";
		opcode = "";
		operand = "";
		comment = "";

		if (line.size() == 0)
			return;
		
		int fc = 0;
		skip = isWhiteSpace(line[0]);
		fields[0] = "";
		
		for (unsigned int i = 0; i < line.size(); ++i) {
			if (skip) 
			{
				if (!isWhiteSpace(line[i])) 
				{
					skip = false;
					++fc;
					fields[fc] = line[i];
				}
			}
			else
			{
				if (isWhiteSpace(line[i])) {
					if (fc < 3) {
						skip = true;
					}
					else {
						//No longer skip in comment field
						fields[fc] += line[i];
					}
				}
				else
				{
					fields[fc] += line[i];
				}
			}
		}

		
		bLHSIsReservedWord = false;

		for (unsigned int i = 0; i < 28; ++i)
		{
			if (fields[0] == RESERVED_WORDS[i])
			{
				bLHSIsReservedWord = true;
				break;
			}
		}
		
		if (bLHSIsReservedWord) {
			symbol = "";
			opcode = fields[0];
			operand = fields[1];
			comment = fields[2] + " " + fields[3];
		}
		else 
		{
			symbol = fields[0];
			opcode = fields[1];
			operand = fields[2];
			comment = fields[3];
		}
		//Erase comments
		if (operand[0] == '#')
		{
			comment = operand + " " + comment;
			operand = "";
		}

		if (opcode[0] == '#')
		{
			comment = opcode + " " + comment;
			opcode = "";
		}

		if (symbol[0] == '#')
		{
			comment = symbol + " " + comment;
			symbol = "";
		}

		//static char buffer[256];
		//sprintf_s(buffer, 255, "[OPCASM] Split: (%s,%s,%s,%s)[%d]", symbol.c_str(), opcode.c_str(), operand.c_str(), comment.c_str(), line.size());
		//oapiWriteLog(buffer);
	}

	void OBCAssembler::DefineSymbol(const std::string& symname, unsigned short module, unsigned short sector, unsigned short syllable, unsigned short addr)
	{
		static char buffer[256];
		sprintf_s(buffer, 255, "[OBCASM] DefineSymbol(%s, %o, %o, %o, %o)", symname.c_str(), module, sector, syllable, addr);
		oapiWriteLog(buffer);

		if (symbolDict.count(symname) > 0)
		{
			if (symbolDict[symname]->IsIncomplete())
			{
				sprintf_s(buffer, 255, "[OBCASM] Update existing temporary symbol.");
				oapiWriteLog(buffer);
				symbolDict[symname]->Define(module, sector, syllable, addr);
			}
			else 
			{
				sprintf_s(buffer, 255, "[OBCASM] Line %d: Symbol  %s already defined.", linenumber, symname);
				oapiWriteLog(buffer);
			}
			
		}
		else
		{
			//Lege neues Symbol an
			sprintf_s(buffer, 255, "[OBCASM] Create new defined symbol %s", symname.c_str());
			oapiWriteLog(buffer);
			Symbol* ps = new Symbol(symname, module, sector, syllable, addr);
			symTable.push_back(ps);
			symbolDict.insert(std::make_pair(symname, ps));
			
			
		}
	}

	void OBCAssembler::DefineSymbol(const std::string& symname)
	{
		static char buffer[256];
		sprintf_s(buffer, 255, "[OBCASM] DefineSymbol(%s)", symname.c_str());
		oapiWriteLog(buffer);
		if (symbolDict.count(symname) > 0)
		{
				sprintf_s(buffer, 255, "[OBCASM] Line %d: Symbol  %s already defined.", linenumber, symname);
				oapiWriteLog(buffer);
		}
		else
		{
			//Lege neues Symbol an
			Symbol* ps = new Symbol(symname, NULL);
			symTable.push_back(ps);
			symbolDict.insert(std::make_pair(symname, ps));
		}
	}

	void OBCAssembler::DefineIncompleteSymbol(const std::string& symname, const std::string& refsymbol) 
	{
		static char buffer[256];
		sprintf_s(buffer, 255, "[OBCASM] DefineIncompleteSymbol(%s, %s)", symname.c_str(), refsymbol.c_str());
		oapiWriteLog(buffer);

		if (symbolDict.count(symname) > 0)
		{
			if (symbolDict[symname]->IsIncomplete() && symbolDict[symname]->GetReference() == NULL) 
			{
				Symbol* prs = NULL;
				sprintf_s(buffer, 255, "[OBCASM] Update temporary symbol %s", symname.c_str());
				oapiWriteLog(buffer);
				if (symbolDict.count(refsymbol) > 0)
				{
					sprintf_s(buffer, 255, "[OBCASM] Found reference %s, seek terminal.", refsymbol.c_str());
					oapiWriteLog(buffer);

					prs = symbolDict[refsymbol];
					//Seek Terminal
					while (prs->GetReference() != NULL && prs->IsIncomplete()) {
						prs = prs->GetReference();
					}
					sprintf_s(buffer, 255, "[OBCASM] Terminal is %s.", prs->GetName().c_str());
					oapiWriteLog(buffer);
				}
				else
				{
					sprintf_s(buffer, 255, "[OBCASM] Create terminal reference %s", refsymbol.c_str());
					oapiWriteLog(buffer);
					prs = new Symbol(refsymbol, NULL);
					symTable.push_back(prs);
					symbolDict.insert(std::make_pair(refsymbol, prs));
				}
				symbolDict[symname]->SetReference(prs);
			}
			else {
				sprintf_s(buffer, 255, "[OBCASM] Line %d: Symbol  %s already defined.", linenumber, symname.c_str());
				oapiWriteLog(buffer);
			}
		}
		else
		{
			Symbol* prs = NULL;
			
			if (symbolDict.count(refsymbol) > 0) 
			{
				sprintf_s(buffer, 255, "[OBCASM] Found reference %s, seek terminal.", refsymbol.c_str());
				oapiWriteLog(buffer);

				prs = symbolDict[refsymbol];
				//Seek Terminal
				while(prs->GetReference() != NULL && prs->IsIncomplete()) {				
					prs = prs->GetReference();
				}
				sprintf_s(buffer, 255, "[OBCASM] Terminal is %s.", prs->GetName().c_str());
				oapiWriteLog(buffer);
			}
			else
			{
				sprintf_s(buffer, 255, "[OBCASM] Create terminal reference %s", refsymbol.c_str());
				oapiWriteLog(buffer);
				prs = new Symbol(refsymbol, NULL);
				symTable.push_back(prs);
				symbolDict.insert(std::make_pair(refsymbol, prs));
			}

			Symbol* ps = NULL;
			if (prs->IsIncomplete()) 
			{
				//Verweise auf das unvollständige Symbol
				sprintf_s(buffer, 255, "[OBCASM] Create temporary symbol.");
				oapiWriteLog(buffer);
				ps = new Symbol(symname, prs);
			}
			else
			{
				//Refer to the memory location
				sprintf_s(buffer, 255, "[OBCASM] Resolved reference, is defined as %s", prs->ToString().c_str());
				oapiWriteLog(buffer);
				ps = new Symbol(symname, prs->GetModule(), prs->GetSector(), prs->GetSyllable(), prs->GetAddress());
			}
			symTable.push_back(ps);
			symbolDict.insert(std::make_pair(symname, ps));

		}
	}

	bool sortByAddress(Symbol* p1, Symbol* p2)
	{
		if (p1 == NULL || p1->IsIncomplete()) 
		{
			return true;
		}
		else if (p2 == NULL || p2->IsIncomplete()) {
			return false;
		}
		else {
			if (p1->GetModule() == p2->GetModule()) {
				if (p1->GetSector() == p2->GetSector())
				{
					if (p1->GetSyllable() == p2->GetSyllable())
					{
						return p1->GetAddress() < p2->GetAddress();
					}
					else
					{
						if (p1->GetAddress() == p2->GetAddress())
						{
							return p1->GetName() < p2->GetName();
						}
						else 
						{
							return p1->GetSyllable() < p2->GetSyllable();
						}
						
					}
				}
				else
				{
					return p1->GetSector() < p2->GetSector();
				}
			}
			else
			{
				return p1->GetModule() < p2->GetModule();
			}
		}
	}

	void OBCAssembler::ResolveMissingSymbols()
	{
		static char buffer[256];
		
		for (unsigned int i = 0; i < symTable.size(); ++i)
		{
			if (symTable[i]->IsIncomplete()) {
				Symbol* ps = symTable[i];
				while (ps->GetReference() != NULL && ps->IsIncomplete())
				{
					ps = ps->GetReference();
				}

				if (ps->IsIncomplete())
				{
					//Can't be resolved
					sprintf_s(buffer, 255, "[OBCASM] PASS 2: Symbol  %s can't be resolved, terminal %s is still incomplete.", 
						symTable[i]->GetName().c_str(), ps->GetName().c_str());
					oapiWriteLog(buffer);

				} 
				else {
					//Resolve
					symTable[i]->Define(ps->GetModule(), ps->GetSector(), ps->GetSyllable(), ps->GetAddress());
				}

			}
		}

		//Sortiere nach Addressen und Liste auf
		std::sort(symTable.begin(), symTable.end(), sortByAddress);
		for (unsigned int i = 0; i < symTable.size(); ++i)
		{
			listing << symTable[i]->ToString() << " " << symTable[i]->GetName() << std::endl;
		}
	}

	bool OBCAssembler::IsDefined(const std::string sym) const
	{
		return symbolDict.count(sym) > 0;
	}

	void OBCAssembler::WriteData(long lValue)
	{
		if (target != NULL)
		{
			if (usDataModule == 0)
			{
				unsigned short word1 = (lValue & 0x1FFF);
				unsigned short word2 = (lValue >> 13) & 0x1FFF;
				target->SetMemory(usDataSector, usDataAddress, 0, word1);
				target->SetMemory(usDataSector, usDataAddress, 1, word2);
			}		
		}
	}

	void OBCAssembler::WriteCode(unsigned short usInstruction)
	{
		if (target != NULL)
		{	
			if (usCodeModule == 0) {
				target->SetMemory(usCodeSector, usCodeAddress, usCodeSyllable, usInstruction);
			}
			
		}
	}

	std::string MakeAddress(unsigned short module, unsigned short sector, unsigned short syllable, unsigned short addr)
	{
		std::ostringstream oss;
		oss << std::oct << std::setfill('0') << std::setw(1) << module
			<< "-" << std::setw(2) << sector
			<< "-" << std::setw(1) << syllable
			<< "-" << std::setw(3) << addr;
		return oss.str();
	}

	void OBCAssembler::ListData(const std::string& typ, long v)
	{
		unsigned long v2 = v & 0x3FFFFFF;
		listing << MakeAddress(usDataModule, usDataSector, usDataSyllable, usDataAddress)
			<< " " << std::oct << std::setfill('0') << std::setw(9) << v2
			<< std::setw(0) << " " << std::setfill(' ') << std::setw(8) << symbol
			<< std::setw(0) << " " << std::setfill(' ') << std::setw(4) << typ
			<< std::setw(0) << " " << std::oct << std::setfill('0') << std::setw(9) << v2 << std::endl;
	}

	void OBCAssembler::ListInstruction(const std::string& opcode, unsigned short usInstr, unsigned short usOperand)
	{
		listing << std::setw(0) << MakeAddress(usCodeModule, usCodeSector, usCodeSyllable, usCodeAddress)
			<< "     " << std::oct << std::setfill('0') << std::setw(5) << usInstr
			<< std::setw(0) << " " << std::setfill(' ') << std::setw(8) << symbol
			<< std::setw(0) << " " << std::setfill(' ') << std::setw(4) << opcode
			<< std::setw(0) << " " << std::oct << std::setfill('0') << std::setw(3) << usOperand << std::endl;
	}
	
};