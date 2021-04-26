#pragma once
#include <Subsystems.h>
#include <Electrics.h>

namespace gemini
{

	class IGSPowerSupply : public core::eps::ElectricSubsystem
	{
		core::eps::InputPort* inP28VDC;
		core::eps::InputPort* inP28VDC_ACPS;
		
		core::eps::OutputPort* outP40VDC;
		core::eps::OutputPort* outM40VDC;
		core::eps::OutputPort* outM3VDC;
		core::eps::OutputPort* outP35VDC;
		core::eps::OutputPort* outP12VDC;
		core::eps::OutputPort* outP35VDC_precision;
		core::eps::OutputPort* outM40VDC_precision;
		core::eps::OutputPort* outM35VDC;
		core::eps::OutputPort* outM22VDC;
		core::eps::OutputPort* outP22VDC;
		core::eps::OutputPort* outP5VDC;
		core::eps::OutputPort* outP28_6VDC;
		core::eps::OutputPort* outP10_2VDC;
		core::eps::OutputPort* outM28_6VDC;
		core::eps::OutputPort* outP20_7VDC;
		core::eps::OutputPort* outP28VDC;
		core::eps::OutputPort* outP28VDC_ACPS;
	public:
		IGSPowerSupply(core::AbstractSubsystemContainer& host, const std::string& name);
		virtual ~IGSPowerSupply();

		virtual void OnCurrentChange(const core::eps::InputPort &src);
		virtual void OnResistanceChange(const core::eps::OutputPort &src);
	};
}