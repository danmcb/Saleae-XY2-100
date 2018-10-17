#ifndef XY2100_ANALYZER_SETTINGS
#define XY2100_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>
#include <SimulationChannelDescriptor.h>

// some flags we can add to frames
#define X_CTRL_FRAME_FLAG (1 << 0)
#define Y_CTRL_FRAME_FLAG (1 << 1)
#define PARITY_X_ERROR_FLAG (1 << 2)
#define PARITY_Y_ERROR_FLAG (1 << 3)
#define SYNC_ERROR_FLAG (1 << 4)

class XY2100AnalyzerSettings : public AnalyzerSettings
{
public:
	XY2100AnalyzerSettings();
	virtual ~XY2100AnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	Channel mClkChannel;
	Channel mSyncChannel;
	Channel mXChannel;
	Channel mYChannel;

	U32 mXY2_FClk; // this will always be 2MHz

protected:
	SimulationChannelDescriptorGroup mXY2SimulationChannels;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mClkChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mSyncChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mXChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mYChannelInterface;
};

#endif //XY2100_ANALYZER_SETTINGS
