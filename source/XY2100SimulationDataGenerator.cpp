#include "XY2100SimulationDataGenerator.h"
#include "XY2100AnalyzerSettings.h"

#include <AnalyzerHelpers.h>

XY2100SimulationDataGenerator::XY2100SimulationDataGenerator()
	:mXpos(0x5555),
	mYpos(0x0000)     // just some arbitrary addresses to start our simulation
{
}

XY2100SimulationDataGenerator::~XY2100SimulationDataGenerator()
{
}

void XY2100SimulationDataGenerator::Initialize( U32 simulation_sample_rate, XY2100AnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;
	mSamples_per_half_cycle = mSimulationSampleRateHz / (2 * mSettings->mXY2_FClk);

	mClk = mXY2SimulationChannels.Add(settings->mClkChannel, mSimulationSampleRateHz, BIT_LOW);
	mSync = mXY2SimulationChannels.Add(settings->mSyncChannel, mSimulationSampleRateHz, BIT_HIGH);
	mX = mXY2SimulationChannels.Add(settings->mXChannel, mSimulationSampleRateHz, BIT_LOW);
	mY = mXY2SimulationChannels.Add(settings->mYChannel, mSimulationSampleRateHz, BIT_LOW);
}

U32 XY2100SimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mClk->GetCurrentSampleNumber() < adjusted_largest_sample_requested &&
		mSync->GetCurrentSampleNumber() < adjusted_largest_sample_requested &&
		mX->GetCurrentSampleNumber() < adjusted_largest_sample_requested &&
		mY->GetCurrentSampleNumber() < adjusted_largest_sample_requested)
	{
		CreateXY2Frame();
	}

	*simulation_channels = mXY2SimulationChannels.GetArray();
	return mXY2SimulationChannels.GetCount();
}

void XY2100SimulationDataGenerator::CreateXY2Frame() {
	U16 x = mXpos;
	U16 y = mYpos;

	mXpos += 3;  // each frame incs by a different amount 
	mYpos += 1;
	
	U8 x_bit_count = 1;  // we count 1's so that we can set parity
	U8 y_bit_count = 1;  // we always start with a single 1 in the ctrl bits

	// First we do the ctrl bits
	// C2
	mClk->TransitionIfNeeded(BIT_HIGH);
	mSync->TransitionIfNeeded(BIT_HIGH);
	mX->TransitionIfNeeded(BIT_LOW);
	mY->TransitionIfNeeded(BIT_LOW);
	mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
	// C2 + 1/2
	mClk->Transition();
	mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
	// C1, X and Y are still 0
	mClk->Transition();
	mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
	// C1 + 1/2
	mClk->Transition();
	mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
	// C0
	mClk->Transition();
	mX->TransitionIfNeeded(BIT_HIGH);  // here are our first 1's that we
	mY->TransitionIfNeeded(BIT_HIGH);  // already counted
	mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
	// C0 + 1/2
	mClk->Transition();
	mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
	// Data
	for (U16 bit_mask = 0x8000; bit_mask > 0; bit_mask = bit_mask >> 1) {
		// we change on first phase (Clk = 1)
		mClk->Transition();
		// set X address bit
		if (x & bit_mask) { // 1
			mX->TransitionIfNeeded(BIT_HIGH);
			x_bit_count++;
		}
		else { // 0
			mX->TransitionIfNeeded(BIT_LOW);
		}
		//set Y address bit
		if (y & bit_mask) { // 1
			mY->TransitionIfNeeded(BIT_HIGH);
			y_bit_count++;
		}
		else { // 0
			mY->TransitionIfNeeded(BIT_LOW);
		}
		mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
		// second phase, Clk goes to 0
		mClk->Transition();
		mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
	}
	// and finally even parity and sync
	mClk->Transition();
	if (x_bit_count & 0x01) { // odd number of 1's?
		mX->TransitionIfNeeded(BIT_HIGH); // generate a 1
	}
	else {
		mX->TransitionIfNeeded(BIT_LOW);
	}
	if (y_bit_count & 0x01) {
		mY->TransitionIfNeeded(BIT_HIGH);
	}
	else {
		mY->TransitionIfNeeded(BIT_LOW);
	}
	mSync->Transition(); // SYNC goes low
	mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
	mClk->Transition();
	mXY2SimulationChannels.AdvanceAll(mSamples_per_half_cycle);
}