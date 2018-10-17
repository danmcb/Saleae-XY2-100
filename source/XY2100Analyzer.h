#ifndef XY2100_ANALYZER_H
#define XY2100_ANALYZER_H

#include <Analyzer.h>
#include "XY2100AnalyzerResults.h"
#include "XY2100SimulationDataGenerator.h"

class XY2100AnalyzerSettings;
class ANALYZER_EXPORT XY2100Analyzer : public Analyzer2
{
public:
	XY2100Analyzer();
	virtual ~XY2100Analyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

	void AdvanceToNextClkNegEdge(void);

protected: //vars
	std::auto_ptr< XY2100AnalyzerSettings > mSettings;
	std::auto_ptr< XY2100AnalyzerResults > mResults;
	AnalyzerChannelData* mClk;
	AnalyzerChannelData* mSync;
	AnalyzerChannelData* mX;
	AnalyzerChannelData* mY;

	XY2100SimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;

	// funcs
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //XY2100_ANALYZER_H
