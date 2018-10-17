#ifndef XY2100_ANALYZER_RESULTS
#define XY2100_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class XY2100Analyzer;
class XY2100AnalyzerSettings;

class XY2100AnalyzerResults : public AnalyzerResults
{
public:
	XY2100AnalyzerResults( XY2100Analyzer* analyzer, XY2100AnalyzerSettings* settings );
	virtual ~XY2100AnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	XY2100AnalyzerSettings* mSettings;
	XY2100Analyzer* mAnalyzer;
};

#endif //XY2100_ANALYZER_RESULTS
