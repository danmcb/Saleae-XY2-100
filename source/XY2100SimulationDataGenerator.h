#ifndef XY2100_SIMULATION_DATA_GENERATOR
#define XY2100_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class XY2100AnalyzerSettings;

class XY2100SimulationDataGenerator
{
public:
	XY2100SimulationDataGenerator();
	~XY2100SimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, XY2100AnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	XY2100AnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateXY2Frame();
	U32 mSamples_per_half_cycle;
	U16 mXpos;
	U16 mYpos;

	SimulationChannelDescriptorGroup mXY2SimulationChannels;
	SimulationChannelDescriptor* mClk;
	SimulationChannelDescriptor* mSync;
	SimulationChannelDescriptor* mX;
	SimulationChannelDescriptor* mY;

};
#endif //XY2100_SIMULATION_DATA_GENERATOR