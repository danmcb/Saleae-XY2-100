#include "XY2100AnalyzerSettings.h"
#include <AnalyzerHelpers.h>


XY2100AnalyzerSettings::XY2100AnalyzerSettings()
:	mClkChannel( UNDEFINED_CHANNEL ),
	mSyncChannel( UNDEFINED_CHANNEL),
	mXChannel( UNDEFINED_CHANNEL ),
	mYChannel( UNDEFINED_CHANNEL )
{
	mClkChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mClkChannelInterface->SetTitleAndTooltip( "CLK", "XY2-100 2MHz Clock" );
	mClkChannelInterface->SetSelectionOfNoneIsAllowed(false);
	mClkChannelInterface->SetChannel( mClkChannel );

	mSyncChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mSyncChannelInterface->SetTitleAndTooltip("SYNC", "XY2-100 Sync");
	mSyncChannelInterface->SetSelectionOfNoneIsAllowed(false);
	mSyncChannelInterface->SetChannel(mSyncChannel);

	mXChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mXChannelInterface->SetTitleAndTooltip("X", "XY2-100 X Address");
	mXChannelInterface->SetSelectionOfNoneIsAllowed(false);
	mXChannelInterface->SetChannel(mXChannel);

	mYChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mYChannelInterface->SetTitleAndTooltip("Y", "XY2-100 Y Address");
	mYChannelInterface->SetSelectionOfNoneIsAllowed(false);
	mYChannelInterface->SetChannel(mYChannel);

	AddInterface( mClkChannelInterface.get() );
	AddInterface( mSyncChannelInterface.get() );
	AddInterface( mXChannelInterface.get() );
	AddInterface( mYChannelInterface.get() );

	mXY2_FClk = 2000000;

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
}

XY2100AnalyzerSettings::~XY2100AnalyzerSettings()
{
}

bool XY2100AnalyzerSettings::SetSettingsFromInterfaces()
{
	mClkChannel = mClkChannelInterface->GetChannel();
	mSyncChannel = mSyncChannelInterface->GetChannel();
	mXChannel = mXChannelInterface->GetChannel();
	mYChannel = mYChannelInterface->GetChannel();

	ClearChannels();
	AddChannel( mClkChannel, "XY2 Clk", true );
	AddChannel( mSyncChannel, "XY2 Sync", true);
	AddChannel( mXChannel, "XY2 X", true);
	AddChannel( mYChannel, "XY2 Y", true);
	return true;
}

void XY2100AnalyzerSettings::UpdateInterfacesFromSettings()
{
	mClkChannelInterface->SetChannel( mClkChannel );
	mSyncChannelInterface->SetChannel( mSyncChannel );
	mXChannelInterface->SetChannel( mXChannel );
	mYChannelInterface->SetChannel( mYChannel );
}

void XY2100AnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mClkChannel;
	text_archive >> mSyncChannel;
	text_archive >> mXChannel;
	text_archive >> mYChannel;

	ClearChannels();
	AddChannel( mClkChannel, "XY2 Clk", true );
	AddChannel( mSyncChannel, "XY2 Sync", true);
	AddChannel( mXChannel, "XY2 X", true);
	AddChannel( mYChannel, "XY2 Y", true);

	UpdateInterfacesFromSettings();
}

const char* XY2100AnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mClkChannel;
	text_archive << mSyncChannel;
	text_archive << mXChannel;
	text_archive << mYChannel;

	return SetReturnString( text_archive.GetString() );
}
