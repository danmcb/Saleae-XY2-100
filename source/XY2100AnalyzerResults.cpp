#include "XY2100AnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "XY2100Analyzer.h"
#include "XY2100AnalyzerSettings.h"
#include <iostream>
#include <fstream>

XY2100AnalyzerResults::XY2100AnalyzerResults( XY2100Analyzer* analyzer, XY2100AnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

XY2100AnalyzerResults::~XY2100AnalyzerResults()
{
}

void XY2100AnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );
	char value_str[32]; // this length seems to be needed to allow binary display
	char sync_err_str[3] = "";
	char par_err_str[3] = "";
	char ctrl_str[3] = "";

	if (frame.mFlags & SYNC_ERROR_FLAG) strcpy(sync_err_str, "!S");
	
	if (channel == mSettings->mXChannel) {
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 16, value_str, 32);
		if (frame.mFlags & PARITY_X_ERROR_FLAG) strcpy(par_err_str, "!P");
		if (frame.mFlags & X_CTRL_FRAME_FLAG) strcpy(ctrl_str, "!C");
	}
	else if (channel == mSettings->mYChannel) {
		AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 16, value_str, 32);
		if (frame.mFlags & PARITY_Y_ERROR_FLAG) strcpy(par_err_str, "!P");
		if (frame.mFlags & Y_CTRL_FRAME_FLAG) strcpy(ctrl_str, "!C");
	}
	AddResultString(ctrl_str, par_err_str, sync_err_str, value_str);
}

void XY2100AnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	//std::ofstream file_stream( file, std::ios::out );

	//U64 trigger_sample = mAnalyzer->GetTriggerSample();
	//U32 sample_rate = mAnalyzer->GetSampleRate();

	//file_stream << "Time [s],Value" << std::endl;

	//U64 num_frames = GetNumFrames();
	//for( U32 i=0; i < num_frames; i++ )
	//{
	//	Frame frame = GetFrame( i );
		
	//	char time_str[128];
	//	AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

	//	char number_str[128];
	//	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

	//	file_stream << time_str << "," << number_str << std::endl;

	//	if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
	//	{
	//		file_stream.close();
	//		return;
	//	}
	//}

	//file_stream.close();
}

void XY2100AnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	AddTabularText( number_str );
#endif
}

void XY2100AnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void XY2100AnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}