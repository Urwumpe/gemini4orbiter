#include "IGS.h"

namespace gemini
{
	IGSPowerSupply::IGSPowerSupply(core::AbstractSubsystemContainer& host, const std::string& name)
		: core::eps::ElectricSubsystem(host, name)
	{
			inP28VDC = CreateInput("P28VDC");
			inP28VDC_ACPS = CreateInput("P28VDC_ACPS");

			outP40VDC = CreateOutput("P40VDC");
			outM40VDC = CreateOutput("M40VDC");
			outM3VDC = CreateOutput("M3VDC"); 
			outP35VDC = CreateOutput("P35VDC");
			outP12VDC = CreateOutput("P12VDC");
			outP35VDC_precision = CreateOutput("P35VDCP");
			outM40VDC_precision = CreateOutput("M40VDCP");
			outM35VDC = CreateOutput("M35VDC");
			outM22VDC = CreateOutput("M22VDC");
			outP22VDC = CreateOutput("P22VDC");
			outP5VDC = CreateOutput("P5VDC");
			outP28_6VDC = CreateOutput("P28.6VDC");
			outP10_2VDC = CreateOutput("P10.2VDC");
			outM28_6VDC = CreateOutput("M28.6VDC");
			outP20_7VDC = CreateOutput("P20.7VDC");
			outP28VDC = CreateOutput("P28VDC");
			outP28VDC_ACPS = CreateOutput("P28VDC_ACPS");
	}
	
	IGSPowerSupply::~IGSPowerSupply()
	{

	}



	void IGSPowerSupply::OnCurrentChange(const core::eps::InputPort &src)
	{
		//Calculate the available current for every system and fail if demand exceeds
		//Simplification: Use functions of input voltage and output resistance
		if (inP28VDC->GetVoltage() > 24.0)
		{
			outP40VDC->SetCurrent(0.1, 40.0);
			outM40VDC->SetCurrent(0.1, -40.0);
			outM3VDC->SetCurrent(0.1, 3);
			outP35VDC->SetCurrent(0.1, 35.0);
			outP12VDC->SetCurrent(0.1, 12.0);
			outP35VDC_precision->SetCurrent(0.1, 35.0);
			outM40VDC_precision->SetCurrent(0.1, -40.0);
			outM35VDC->SetCurrent(0.1, -35.0);
			outM22VDC->SetCurrent(0.1, -22.0);
			outP22VDC->SetCurrent(0.1, 22.0);
			outP5VDC->SetCurrent(0.1, 5.0);
			outP28_6VDC->SetCurrent(0.1, 28.6);
			outP10_2VDC->SetCurrent(0.1, 10.2);
			outM28_6VDC->SetCurrent(0.1, -28.6);
			outP20_7VDC->SetCurrent(0.1, 20.7);
			outP28VDC->SetCurrent(0.1, 28.0);
			outP28VDC_ACPS->SetCurrent(0.01, 28.0);
		}
		else {
			
			outP40VDC->SetCurrent(0.0, 0.0);
			outM40VDC->SetCurrent(0.0, 0.0);
			outM3VDC->SetCurrent(0.0, 0.0);
			outP35VDC->SetCurrent(0.0, 0.0);
			outP12VDC->SetCurrent(0.0, 0.0);
			outP35VDC_precision->SetCurrent(0.0, 0.0);
			outM40VDC_precision->SetCurrent(0.0, 0.0);
			outM35VDC->SetCurrent(0.0, 0.0);
			outM22VDC->SetCurrent(0.0, 0.0);
			outP22VDC->SetCurrent(0.0, 0.0);
			outP5VDC->SetCurrent(0.0, 0.0);
			outP28_6VDC->SetCurrent(0.0, 0.0);
			outP10_2VDC->SetCurrent(0.0, 0.0);
			outM28_6VDC->SetCurrent(0.0, 0.0);
			outP20_7VDC->SetCurrent(0.0, 0.0);
			outP28VDC->SetCurrent(0.0, 0.0);
			outP28VDC_ACPS->SetCurrent(0.00, 0.0);
		}
	}

	double OutputPower(const core::eps::OutputPort &p)
	{
		return pow(p.GetVoltage(), 2) / p.GetResistance();
	}

	void IGSPowerSupply::OnResistanceChange(const core::eps::OutputPort &src)
	{
		//Some improvements needed here, since the system has much more functions
		//Sum up the output power and define the DC power demand
		double fP = 0.0;

		fP += OutputPower(*outP40VDC);
		fP += OutputPower(*outM40VDC);
		fP += OutputPower(*outM3VDC);
		fP += OutputPower(*outP35VDC);
		fP += OutputPower(*outP12VDC);
		fP += OutputPower(*outP35VDC_precision);
		fP += OutputPower(*outM40VDC_precision);
		fP += OutputPower(*outM35VDC);
		fP += OutputPower(*outM22VDC);
		fP += OutputPower(*outP22VDC);
		fP += OutputPower(*outP5VDC);
		fP += OutputPower(*outP28_6VDC);
		fP += OutputPower(*outP10_2VDC);
		fP += OutputPower(*outM28_6VDC);
		fP += OutputPower(*outP20_7VDC);
		fP += OutputPower(*outP28VDC);
		fP += OutputPower(*outP28VDC_ACPS);

		inP28VDC->SetResistance(pow(28, 2) / fP);
	}
}
