#include "XY2100Analyzer.h"
#include "XY2100AnalyzerSettings.h"
#include <AnalyzerChannelData.h>

XY2100Analyzer::XY2100Analyzer()
:	Analyzer2(),  
	mSettings( new XY2100AnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

XY2100Analyzer::~XY2100Analyzer()
{
	KillThread();
}

void XY2100Analyzer::SetupResults()
{
	mResults.reset( new XY2100AnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mXChannel );
	mResults->AddChannelBubblesWillAppearOn(mSettings->mYChannel);
}

void XY2100Analyzer::WorkerThread()
{
	
	U64 start_sample = 0;
	U64 next_sample = 0;

	mSampleRateHz = GetSampleRate();

	mClk = GetAnalyzerChannelData( mSettings->mClkChannel );
	mSync = GetAnalyzerChannelData(mSettings->mSyncChannel);
	mX = GetAnalyzerChannelData(mSettings->mXChannel);
	mY = GetAnalyzerChannelData(mSettings->mYChannel);

	// find start of first frame, that is rising edge of sync
	if (mSync->GetBitState() == BIT_LOW) {
		mSync->AdvanceToNextEdge();
	}
	else { // you have to go down to get up again
		mSync->AdvanceToNextEdge();
		mSync->AdvanceToNextEdge();
	}
	// and sync the clock to the frame start
	next_sample = mSync->GetSampleNumber();
	mClk->AdvanceToAbsPosition(next_sample);

	while (1){  // decode starts here
		U16 x_val = 0;
		U16 y_val = 0;
		U8 x_par_ctr = 0;
		U8 y_par_ctr = 0;
		bool x_is_data = true; // we will only show decoded data when ctrl info is 001
		bool y_is_data = true;
		bool sync_ok = true;     // sync must only be low on last clock edge
		bool x_parity_ok = false;
		bool y_parity_ok = false;

		start_sample = next_sample;
		
		// find next clock falling edge, we sample Sync, X and Y on it
		// this is C2, XY should be 0
		AdvanceToNextClkNegEdge();
		if (mX->GetBitState() == BIT_HIGH) {
			x_is_data = false;
			x_par_ctr += 1;
		}
		if (mY->GetBitState() == BIT_HIGH) {
			y_is_data = false;
			y_par_ctr += 1;
		}
		if (mSync->GetBitState() == BIT_LOW) sync_ok = false;
		// this is C1, XY should be 0
		AdvanceToNextClkNegEdge();
		if (mX->GetBitState() == BIT_HIGH) {
			x_is_data = false;
			x_par_ctr += 1;
		}
		if (mY->GetBitState() == BIT_HIGH) {
			y_is_data = false;
			y_par_ctr += 1;
		}
		if (mSync->GetBitState() == BIT_LOW) sync_ok = false;
		// this is C0, SY should be 1
		AdvanceToNextClkNegEdge();
		if (mX->GetBitState() == BIT_LOW) x_is_data = false;
		else x_par_ctr += 1;
		if (mY->GetBitState() == BIT_LOW) y_is_data = false;
		else y_par_ctr += 1;
		if (mSync->GetBitState() == BIT_LOW) sync_ok = false;
		// now we can sample the data
		for (U16 bit_mask = 0x8000; bit_mask > 0; bit_mask = bit_mask >> 1) {
			AdvanceToNextClkNegEdge();
			if (mX->GetBitState() == BIT_HIGH) {
				x_val = x_val | bit_mask;
				x_par_ctr += 1;
			}
			if (mY->GetBitState() == BIT_HIGH) {
				y_val = y_val | bit_mask;
				y_par_ctr += 1;
			}
			if (mSync->GetBitState() == BIT_LOW) sync_ok = false;
		}
		// now the last bit (sync/parity)
		AdvanceToNextClkNegEdge();
		if (mSync->GetBitState() == BIT_HIGH) {
			// this is a framing error, we try to resync
			sync_ok = false;
			mSync->AdvanceToNextEdge(); // sync is low
			mSync->AdvanceToNextEdge(); // sync is high
		} else {
			// if we counted an odd number of 1's, P should be 1
			// if we counted an even number, it should be 0
			// in other words, the ctr LSB and P bit should be the same
			x_parity_ok = ((mX->GetBitState() == BIT_HIGH) == (x_par_ctr & 0x01));
			y_parity_ok = ((mY->GetBitState() == BIT_HIGH) == (y_par_ctr & 0x01));
			// now go to next frame start
			mSync->AdvanceToNextEdge();
		}
		// SYNC should be at frame start, we move CLK to smae pos
		next_sample = mSync->GetSampleNumber();
		mClk->AdvanceToAbsPosition(next_sample);

		// the frame is complete. Move to next CLK rising edge.
		Frame frame;
		frame.mData1 = x_val;
		frame.mData2 = y_val;
		frame.mFlags = 0;
		if (!x_is_data) {
			frame.mFlags |= X_CTRL_FRAME_FLAG;
			frame.mFlags |= DISPLAY_AS_WARNING_FLAG;
		}
		if (!y_is_data) {
			frame.mFlags |= Y_CTRL_FRAME_FLAG;
			frame.mFlags |= DISPLAY_AS_WARNING_FLAG;
		}

		if (!x_parity_ok) frame.mFlags |= PARITY_X_ERROR_FLAG;
		if (!y_parity_ok) frame.mFlags |= PARITY_Y_ERROR_FLAG;
		if (!sync_ok) frame.mFlags |= SYNC_ERROR_FLAG;
		if (!(x_parity_ok && y_parity_ok && sync_ok)) frame.mFlags |= DISPLAY_AS_ERROR_FLAG;
		frame.mStartingSampleInclusive = start_sample;
		frame.mEndingSampleInclusive = next_sample - 1;

		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
	}
}

void XY2100Analyzer::AdvanceToNextClkNegEdge(void) {
	U64 pos = 0;

	mClk->AdvanceToNextEdge();
	if (mClk->GetBitState() == BIT_HIGH) mClk->AdvanceToNextEdge();
	pos = mClk->GetSampleNumber();
	mSync->AdvanceToAbsPosition(pos);
	mX->AdvanceToAbsPosition(pos);
	mY->AdvanceToAbsPosition(pos);
}

bool XY2100Analyzer::NeedsRerun()
{
	return false;
}

U32 XY2100Analyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 XY2100Analyzer::GetMinimumSampleRateHz()
{
	return 8000000; // 2MHz * 4
}

const char* XY2100Analyzer::GetAnalyzerName() const
{
	return "XY2-100";
}

const char* GetAnalyzerName()
{
	return "XY2-100";
}

Analyzer* CreateAnalyzer()
{
	return new XY2100Analyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}