//! GUI application using nana GUI library.
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#include "gui-app.hpp"
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <scanlib/core/scanner_utils.h>
#include <sstream>#include <strstream>

using cbool = bool const;
using namespace nana;
using namespace std::chrono;
using namespace std;

template <typename arg_, size_t n_>
constexpr size_t countof( arg_ ( & )[n_] ) { return n_; }

int gui_app( int argc, char** argv )
{
    FScannerProtocolHandler Scanner;
    ScannerMainForm         fm( &Scanner );
    fm.show();
    exec();
    return 0;
}

int gui_view_app( int argc, char** argv )
{
    vector<std::unique_ptr<form>>                forms;
    vector<std::unique_ptr<ScannerViewerWidget>> widgets;

    for ( size_t i = 1; i < argc; i++ ) {
        printf( "%s\n", argv[i] );
        ifstream           fs { argv[i], ios::binary | ios::in };
        ScanDataHeaderType f;
        ScanDataPixelType* p;
        if ( scanlib::ScanDataReadFrom( fs, &p, &f ) ) {
            printf( "Opening viewer form ...\n" );
            auto& fm   = *forms.emplace_back( make_unique<form>() );
            auto  desc = FScanImageDesc( f.WIDTH, f.HEIGHT, f.ASPECT_RATIO, (FPxlData*)p );
            auto& vp   = *widgets.emplace_back( make_unique<ScannerViewerWidget>( fm, std::move( desc ) ) );
            fm.div( "vert margin=10<weight=10><<weight=10><ALL><weight=10>><weight=10>" );
            fm["ALL"] << vp;
            fm.caption( argv[i] );
            fm.collocate();
            fm.size( { 800, 600 } );
            vp.bgcolor( color().from_rgb( 176, 176, 176 ) );
            fm.show();
        }
        else {
            printf( "error: failed to read image file\n    :: %s\n", argv[i] );
        }
    }
    exec();
    return 0;
}

ScannerMainForm::ScannerMainForm( FScannerProtocolHandler* Scanner, std::string const& fontName )
    : form( API::make_center( 1024, 1024 ) )
    , bEnableAutoRestart( false )
    , bInStartMode( false )

{
    caption( "scangui" );

    mFormatFont = paint::font { fontName, 10.0 };
    mLayout.div( ScannerMainFormLayoutScript );

    /// Determine Layout
    {
        // mLayout["viewport"] << mViewportGroup;
        mLayout["viewmode"] << mViewModeSelect;
        mLayout["request_pane"] << mRequestGroup;
        mLayout["report_pane"] << mReportGroup;
        mLayout["status_pane"] << mStatusGroup;
        mLayout["config_pane"] << mConfigGroup;
    }

    /// Determine app styles
    {
        auto GroupColor = mReportGroup.bgcolor();

        // Style report
        mStatusGroup.caption( "Status" );
        mStatus.create( mStatusGroup );
        mStatus.reset( "Connecting... " );
        mStatus.editable( false );
        mStatus.multi_lines( true );
        paint::font::font_style status_style = {};
        status_style.weight                  = 500;
        auto font                            = paint::font { fontName, 10, status_style };
        mStatus.typeface( font );
        mStatus.bgcolor( color().from_rgb( 64, 64, 64 ) );
        mStatus.fgcolor( color().from_rgb( 54, 255, 54 ) );

        mStatusGroup.div( "<margin=10 status>" );
        mStatusGroup["status"] << mStatus;
        mStatusGroup.collocate();

        // Style status
        mReportGroup.caption( "Console" );
        mMenualCommand.create( mReportGroup );
        mMenualCommand.editable( true );
        mMenualCommand.multi_lines( false );
        mMenualCommand.bgcolor( color().from_rgb( 255, 211, 222 ) );
        mMenualCommand.typeface( mFormatFont );
        mReport.create( mReportGroup );
        mReport.typeface( mFormatFont );
        mReport.bgcolor( color().from_rgb( 33, 33, 33 ) );
        mReport.fgcolor( color().from_rgb( 215, 215, 215 ) );
        mReport.multi_lines( true );
        mReport.editable( false );
        mReport.line_wrapped( true );

        mReportGroup.div( "vert<margin=10 report><command margin=[0,10,5,10] weight=25>" );
        mReportGroup["command"] << mMenualCommand;
        mReportGroup["report"] << mReport;
        mReportGroup.collocate();

        // Style viewport
        mViewport.create( *this );
        mLayout["viewport"] << mViewport;
        mViewport.init( {} );
        mViewport.bgcolor( color().from_rgb( 46, 46, 46 ) );
        // mViewportGroup.caption( "" );
        // mViewportGroup.div( "<view>" );
        // mViewportGroup["view"] << mViewport;

        // Viewmode
        mViewModeSelect.caption( "View Mode" );
        mViewModeSelect.div( "vert<><<autosave><lbl weight=100><sel weight=50> weight=20 margin=[0,30]><>" );
        mViewIndexLabel.create( mViewModeSelect );
        mViewIndexLabel.bgcolor( GroupColor );
        mViewIndexInput.create( mViewModeSelect );
        mViewIndexInput.events().text_changed( [this]( auto ) {
            mViewImgIndex = mViewIndexInput.to_int();
            mViewIndexLabel
                .caption( mViewImgIndex == 0
                              ? string( "Viewing Scanning" )
                              : string( "Viewing Captured" ) );
            mViewIndexInput
                .bgcolor( mViewImgIndex == 0
                              ? color().from_rgb( 0, 0, 0 )
                              : color().from_rgb( 255, 255, 255 ) );
            AutoUpdateImage();
        } );
        UpdateImageHistoryBox( 0 );
        mViewIndexLabel.caption( " Ready... " );
        mViewIndexInput.bgcolor( color().from_rgb( 0, 0, 0 ) );
        mViewIndexInput.value( "0" );
        mViewModeSelect["lbl"] << mViewIndexLabel;
        mViewModeSelect["sel"] << mViewIndexInput;

        // Autosave path
        mAutoSavePathDisplay.create( mViewModeSelect );
        mViewModeSelect["autosave"] << mAutoSavePathDisplay;
        mAutoSavePathDisplay.text_align( align::left, align_v::bottom );
        mAutoSavePathDisplay.caption( "Auto-save is unavailable.\n" );
        mAutoSavePathDisplay.bgcolor( GroupColor );

        // Style request buttons
        mRequestGroup.caption( "Operations" );
        mRequestGroup.div( "<vert margin=10 gap = 10 <vert gap=5 "
                           "btns><margin=[5, 0, 0, 0] btns_sub weight=30%>" );
        mStartScan.create( mRequestGroup );
        mStopScan.create( mRequestGroup );
        mMoveMotorBtn.create( mRequestGroup );
        mPrecisionMode.create( mRequestGroup );
        mRequestGroup["btns"] << mStartScan << mStopScan;
        mRequestGroup["btns_sub"] << mPrecisionMode << mMoveMotorBtn;

        mPrecisionMode.caption( "Close Distance Mode" );
        mPrecisionMode.bgcolor( GroupColor );
        mStartScan.caption( "Start Scanning" );
        mStopScan.caption( "Stop Scanning" );
        mMoveMotorBtn.caption( "Adjust Motor" );

        // Setup control group
        mConfigGroup.caption( "Configure (Hover over to view tooltips)" );
        mConfigGroup.div( "<vert margin=[10,0] texts weight=30%>       "
                          "<vert margin=10                             "
                          " <><margin=[2.5, 0] gap = 5 b0 weight=30>   "
                          " <><margin=[2.5, 0] gap = 5 b1 weight=30>   "
                          " <><margin=[2.5, 0] gap = 5 b2 weight=30>   "
                          " <><margin=[2.5, 0] gap = 5 b3 weight=30>   "
                          " <><margin=[2.5, 0] gap = 5 b4 weight=30>   "
                          "<>>" );
        char constexpr* conftxt[]     = { "Offset Angle", "FOV", "Resolution", "Measure Delay \n[usec]", "Motor Speed \n[Accel/Max]" };
        char constexpr* conftooltip[] = {
            //
            // Offset angle tooltip
            "Target offset angle from motor origin [x, y] [degree]",
            //
            // FOV tooltip
            "Target field of view [x, y] [degree]",
            //
            // Resolution
            "Target image resolution value [x, y] [pixels]\n"
            "Resolution can have large error from the target value, since priority of FOV is higher.",
            //
            // Delay
            "Sensor delay on every measurement. [microseconds]",
            //
            // Motor Speed
            "The left column shows the maximum step speed of the motor in Hz. \n"
            "The item on the right shows the step acceleration of the motor in Hz/s.",
        };
        for ( size_t idx = 0; idx < countof( conftxt ); idx++ ) {
            auto& i = mConfText[idx];
            i.create( mConfigGroup );
            i.bgcolor( GroupColor );
            i.caption( conftxt[idx] );
            i.text_align( align::center, align_v::center );
            i.tooltip( conftooltip[idx] );
            mConfigGroup["texts"] << i;
        }
        char constexpr* confgrp[]     = { "b0", "b1", "b2", "b4" };
        spinbox*        confwidgets[] = { mConfOfst, mConfAngle, mConfResolution, mConfMotorSpd };
        double const    RangeMin[]    = { -120, 0, 1, 0 };
        double const    RangeMax[]    = { 120, 120, 100000, 10000000 };
        double const    RangeStep[]   = { 5, 5, 25, 1000 };
        int const       RangeInit[]   = { 0, 5, 25, 15400 };
        for ( size_t i = 0; i < countof( confgrp ); i++ ) {
            for ( size_t k = 0; k < 2; k++ ) {
                auto& widget = confwidgets[i][k];
                widget.create( mConfigGroup );
                mConfigGroup[confgrp[i]] << confwidgets[i][k];
                if ( i == 2 || i == 3 ) {
                    widget.range( (int)RangeMin[i], (int)RangeMax[i], (int)RangeStep[i] );
                }
                else {
                    widget.range( RangeMin[i], RangeMax[i], RangeStep[i] );
                }
                widget.value( to_string( RangeInit[i] ) );
            }
        }

        mConfDelay.create( mConfigGroup );
        mConfigGroup["b3"] << mConfDelay;
        mConfDelay.range( 300, 1000000, 100 );
        mConfDelay.value( "2000" );
    }

    mLayout.collocate();

    /// Setup menus
    {
        auto& file = mMenu.push_back( "File" );
        auto& ops  = mMenu.push_back( "Mode" );

        // Setup output file location
        filebox fb( *this, true );

        // Setup auto retrigger
        ops.append( "Auto restart capture", [this]( auto proxy ) { bEnableAutoRestart = proxy.checked(); } )
            .check_style( menu::checks::highlight );
        ops.append( "Set Angle Per Step", [this]( auto proxy ) { OpenStepPerAngleSettingDialog(); } );
        ops.append( "Set Motor Drive Clock Speed", [this]( auto proxy ) { OpenMotorDrvClkSettingDialog(); } );
        ops.append_splitter();
        ops.append( "Send test signal", [this]( auto proxy ) { mScan->Test(); } );

        file.append( "Save Complete Image As...", [this]( auto ) { OpenSaveAs(); } );
        file.append( "Open Depth Map File ...", [this]( auto ) { OpenDepthMapFile(); } );
        file.append_splitter();
        file.append( "Set autosave path...", [this]( auto ) { SetAutosavePath(); } );
        file.append( "Auto save when capture done", [this]( auto proxy ) { bFileEnableAutosave = proxy.checked(); } )
            .check_style( menu::checks::highlight );
        file.append_splitter();
        file.append( "Exit", [this]( auto proxy ) { this->close(); } );
    }

    /// Bind app logics
    BindScanner( Scanner );
    mMenualCommand.events().key_char( [this]( arg_keyboard const& arg ) {
        UpdateReportText();
    } );
    mMenualCommand.events().key_press( [this]( arg_keyboard const& arg ) {
        switch ( arg.key ) {
        case SCANLIB_ASCIIVAL_ENTER: {
            auto cmd = mMenualCommand.text();
            mScan->SendString( cmd.c_str() );
            mMenualCommand.reset();
            if ( mCommandHistory.size() >= NumMaxCommandHistory ) {
                mCommandHistory.pop_front();
            }
            mCommandHistory.emplace_back( std::move( cmd ) );
            mCommandCursor = 0;
        } break;
        case SCANLIB_ASCIIVAL_UPARROW:
        case SCANLIB_ASCIIVAL_DNARROW: {
            if ( arg.key == SCANLIB_ASCIIVAL_UPARROW && mCommandCursor < mCommandHistory.size() )
                mCommandCursor++;
            else if ( arg.key == SCANLIB_ASCIIVAL_DNARROW && mCommandCursor )
                mCommandCursor--;

            if ( mCommandCursor == 0 ) {
                mMenualCommand.reset();
                break;
            }

            mMenualCommand.reset( *( mCommandHistory.end() - mCommandCursor ) );
        } break;
        default:
            break;
        }
    } );

    mStartScan.events().click( [this]( arg_click const& arg ) { bInStartMode = true; StartCapture(); } );
    mStopScan.events().click( [this]( arg_click const& arg ) { bInStartMode = false; StopCapture(); } );
    mMoveMotorBtn.events().click( [this]( arg_click const& arg ) { RequestMotorMovement(); } );

    /// Activate condition report timer
    mUpdate.elapse( [this]() {
        UpdateTimerEventHandler();
        mUpdate.interval( UpdatePeriod );
        mUpdate.start();
    } );

    /// Setup scans
    {
        mCaptureParam.bPrescisionMode = true;
    }

    mUpdate.interval( UpdatePeriod );
    mUpdate.start();
}

ScannerMainForm::~ScannerMainForm()
{
    if ( mComSearchTask.valid() ) {
        mComSearchTask.wait();
    }
    mScan->Logger = nullptr;
}

void ScannerMainForm::BindScanner( FScannerProtocolHandler* scanRef )
{
    //! @todo.
    mScan                = scanRef;
    mScan->OnReport      = [this]( auto rep ) { this->OnUpdateReport( rep ); };
    mScan->OnReceiveLine = [this]( auto rep ) { this->OnUpdateImage( rep ); };
    mScan->OnFinishScan  = [this]( auto rep ) { this->OnScannerCaptureDone( rep ); };
    mScan->Logger        = [this]( auto str ) {
        enum { LINE_CLEAR_THRESHOLD = 1000,
               LINE_LEFT_DIVIDER    = 2 };
        if ( mReport.text_line_count() > LINE_CLEAR_THRESHOLD ) {
            auto   cpy      = mReport.text();
            size_t inv_ofst = cpy.size();
            auto   riter    = cpy.crbegin();
            for ( size_t i = 0, length = LINE_CLEAR_THRESHOLD / 4;
                  i < length; ++riter, --inv_ofst ) {
                if ( *riter == '\n' ) {
                    ++i;
                }
            }
            mReport.reset( cpy.substr( cpy.size() - inv_ofst ) );
        }
        mReport.append( str, true );
    };
}
void ScannerMainForm::UpdateImageHistoryBox( size_t newval )
{
    auto bkup = mViewIndexInput.value();
    mViewIndexInput.range( 0, newval, 1 );
    mViewIndexInput.value( bkup );
}

FScanImageDesc const& ScannerMainForm::GetViewingImage() const
{
    if ( mCapturedImage.empty() ) {
        return {};
    }
    return *( mCapturedImage.end() - std::max( size_t( 1 ), mViewImgIndex ) );
}

void ScannerMainForm::OnScannerCaptureDone( FScanImageDesc const& desc )
{
    if ( desc.CData() == nullptr ) {
        return;
    }

    if ( mCapturedImage.size() >= mNumMaxImageHistory ) {
        mCapturedImage.pop_front();
    }
    mCapturedImage.emplace_back( desc.Clone() );
    UpdateImageHistoryBox( std::min( mCapturedImage.size(), mNumMaxImageHistory ) );

    // Write images to auto save path
    // File name is yyyy-mm-dd-hh-mm-ss-s.dpta
    if ( bFileEnableAutosave && mAutoSavePath.empty() == false ) {
        // Make file name
        using namespace std;
        using namespace std::chrono;
        auto time = system_clock::to_time_t( system_clock::now() );
        tm   t    = *localtime( &time );

        // Open file
        wstringstream ws;
        ws << mAutoSavePath.substr( 0, mAutoSavePath.find_last_of( '.' ) ) << '-';
        ws << t.tm_year + 1900;
        ws << std::setfill( L'0' ) << std::setw( 2 ) << t.tm_mon + 1;
        ws << std::setfill( L'0' ) << std::setw( 2 ) << t.tm_mday;
        ws << '-';
        ws << std::setfill( L'0' ) << std::setw( 2 ) << t.tm_hour;
        ws << std::setfill( L'0' ) << std::setw( 2 ) << t.tm_min;
        ws << std::setfill( L'0' ) << std::setw( 2 ) << t.tm_sec;
        ws << '.' << SCAN_DATA_FORMAT_EXTENSION;

        print( "Saving file as %S ... \n", ws.str().c_str() );
        SaveCurrentImage( desc, ws.str().c_str() );
    }

    // Update image for case not viewing scanning image
    AutoUpdateImage();
}

void ScannerMainForm::OnUpdateReport( FDeviceStat const& Stat )
{
    mLastStat = Stat;
    char   buf[2048];
    double progress = !Stat.bIsIdle * 100.0 * ( ( Stat.CurMotorStepY - Stat.OfstY ) / (double)( Stat.SizeY * Stat.StepPerPxlY ) );

    sprintf( buf,
             "\n"
             "                 %8c %8c unit \n"
             " StepPerPxl    [ %8d %8d step ]\n"
             " AnglePerPxl   [ %8.4f %8.4f  deg ]\n"
             " Resolution    [ %8d %8d  pxl ]\n"
             " FOV           [ %8.4f %8.4f  deg ]\n"
             " OfstInSteps   [ %8d %8d step ]\n"
             " OfstInAngle   [ %8.4f %8.4f  deg ]\n"
             " AnglePerStep  [ %8.6f %8.6f  deg ]\n"
             " CurMotorPos   [ %8d %8d step ]\n"
             "\n"
             " MeasureDelay  [ %17d   us ]\n"
             " DistanceMode  [ %17s      ]\n"
             " %-13s [ %17.3f    s ]\n"
             "\n"
             "      :::      %04.1f%% done      ::: ",
             'x', 'y',
             Stat.StepPerPxlX, Stat.StepPerPxlY,
             Stat.StepPerPxlX * Stat.DegreePerStepX, Stat.StepPerPxlY * Stat.DegreePerStepY,
             Stat.SizeX, Stat.SizeY,
             Stat.SizeX * Stat.StepPerPxlX * Stat.DegreePerStepX, Stat.SizeY * Stat.StepPerPxlY * Stat.DegreePerStepY,
             Stat.OfstX, Stat.OfstY,
             Stat.OfstX * Stat.DegreePerStepX, (int)Stat.OfstY * Stat.DegreePerStepY,
             Stat.DegreePerStepX, Stat.DegreePerStepY,
             Stat.CurMotorStepX, Stat.CurMotorStepY,
             Stat.DelayPerCapture,
             Stat.bIsPrecisionMode ? "Near" : "Far",
             Stat.bIsIdle ? "TimeLaunch" : "TimeMeasure",
             Stat.TimeAfterLaunch_us / 1e6,
             progress );

    mStatusText = buf;
}

void ScannerMainForm::OnUpdateImage( FScanImageDesc const& desc )
{
    //! Update GUI
    if ( mViewImgIndex && !mCapturedImage.empty() ) {
        return;
    }

    mViewport.ReplaceDesc( desc );
}

static void Internal_RenderImage(
    FScanImageDesc const&                      desc,
    function<float( FPxlData )>                calc_H,
    function<void( float H, color& out )>      calc_col,
    function<void( int i, int j, color& out )> set_pixel,
    int                                        HorizontalCalib );

void ScannerViewerWidget::RenderImage(
    FScanImageDesc const&  desc,
    nana::paint::graphics& gp,
    bool                   bRenderAmp,
    float                  MaxDistance,
    float                  MinDistance,
    int                    HorizontalCalib,
    ColorMappingMode       ColorMode,
    void*                  CacheBuff )
{
    //! Reserve buffer if size is not sufficient
    if ( desc.Width == 0 || desc.Height == 0 ) {
        return;
    }
    if ( gp.size().width != desc.Width || gp.size().height != desc.Height ) {
        gp.resize( { (uint32_t)desc.Width, (uint32_t)desc.Height } );
    }

    // Setup callbacks
    function<float( FPxlData )> calc_H;
    if ( bRenderAmp == false ) {
        calc_H = [MaxDistance, MinDistance, desc]( FPxlData d ) {
            return (float)( d.Distance / (float)(Q9_22_ONE_INT)-MinDistance ) / ( MaxDistance - MinDistance );
        };
    }
    else {
        calc_H = []( FPxlData d ) {
            return d.AMP / (float)( UQ12_4_ONE_INT * ( 1 << 12 ) );
        };
    }
    function<void( float H, color& out )> calc_col;

    switch ( ColorMode ) {
    case ScannerViewerWidget::ColorMappingMode::WTOK:
        calc_col =
            []( float H, color& out ) {
                H          = clamp( H, 0.f, 1.f );
                auto ratio = H * 3.f;
                auto b     = unsigned( clamp( 255.f * ( 1.f - ratio ), 0.f, 255.5f ) );
                auto g     = unsigned( clamp( 255.f * ( 1.f - ratio / 2.f ), 0.f, 255.5f ) );
                auto r     = unsigned( clamp( 255.f * ( 1.f - ratio / 3.f ), 0.f, 255.5f ) );
                out.from_rgb( r, g, b );
            };
        break;
    case ScannerViewerWidget::ColorMappingMode::RAINBOW:
        calc_col =
            []( float H, color& out ) {
                H          = clamp( H, 0.f, 1.f );
                auto ratio = H * 3.f;

                auto h = clamp( 1.f - ratio / 2.f, 0.f, 1.f );
                auto s = clamp( 1.f - ratio / 3.f, 0.f, 1.f );
                auto l = clamp( 1.f - ratio / 3.f, 0.f, 1.f );

                out.from_hsl( ( h * 360.f ) - 240.f, s, l );
            };
        break;
    case ScannerViewerWidget::ColorMappingMode::BGR:
        calc_col =
            []( float H, color& out ) {
                H           = clamp( H, 0.f, 1.f );
                float ratio = 2 * H;
                auto  b     = unsigned( max( 0.f, 255.f * ( 1.f - ratio ) ) );
                auto  r     = unsigned( max( 0.f, 255.f * ( ratio - 1.f ) ) );
                auto  g     = 255u - b - r;

                out.from_rgb( r, g, b );
            };
        break;
    case ScannerViewerWidget::ColorMappingMode::GREYSCALE:
    default:
        calc_col = []( float H, color& out ) { H = 1.0f - clamp(H, 0.0f, 1.0f); out.from_rgb((int) ( H * 255 ), (int) ( H * 255 ), (int) ( H * 255 )); };
        break;
    }

    auto data = desc.Data();
    if ( CacheBuff && API_MappToRGB( (intptr_t)gp.context(),

                                     [&]( void* data ) {
                                         Internal_RenderImage(
                                             desc,
                                             calc_H,
                                             calc_col,
                                             [data, w = desc.Width, h = desc.Height]( int x, int y, color const& c ) {
                                                 uint32_t* ptr  = (uint32_t*)data;
                                                 ptr[y * w + x] = c.argb().value;
                                             },
                                             HorizontalCalib );
                                     },
                                     CacheBuff ) ) {
        gp.flush();
        return;
    }

    Internal_RenderImage(
        desc,
        calc_H,
        calc_col,
        [&gp]( int x, int y, color const& c ) { gp.set_pixel( x, y, c ); },
        HorizontalCalib );

    gp.flush();
}

static void Internal_RenderImage(
    FScanImageDesc const&                      desc,
    function<float( FPxlData )>                calc_H,
    function<void( float H, color& out )>      calc_col,
    function<void( int x, int y, color& out )> set_pixel,
    int                                        HorizontalCalib )
{
    //! Render image in mapped BGR format
    color col;
    auto  head = desc.Data();
    for ( size_t i = 0; i < desc.Height; i++ ) {
        for ( size_t j = 0; j < desc.Width; j++ ) {
            auto  pxl = *head++;
            float H   = calc_H( pxl );
            calc_col( H, col );

            size_t jnew = j + ( bool( i & 1 ) * (int64_t)HorizontalCalib );
            jnew        = clamp( jnew, size_t( 0 ), size_t( desc.Width - 1 ) );
            if ( j < HorizontalCalib || j > desc.Width - HorizontalCalib ) {
                set_pixel( j, i, col );
            }
            set_pixel( jnew, i, col );
        }
    }
}

void ScannerMainForm::AutoUpdateImage()
{
    FScanImageDesc desc;
    if ( ( mViewImgIndex == 0 && mScan->GetScanningImage( desc ) ) || ( mViewImgIndex && mCapturedImage.size() && ( desc = GetViewingImage() ).CData() ) ) {
        mViewport.ReplaceDesc( desc );
    }
}

void ScannerMainForm::OpenSaveAs()
{
    // Check whether there's data to save
    if ( mCapturedImage.empty() ) {
        return;
    }

    nana::filebox fb { *this, false };
    char          buf[1024];
    auto          desc = GetViewingImage();
    sprintf( buf, "fov[%1.0f-%1.0f]-r[%d-%d]-d[%d]-mot[%d]",
             mConfAngle[0].to_double(),
             mConfAngle[1].to_double(),
             mConfResolution[0].to_int(),
             mConfResolution[1].to_int(),
             mConfDelay.to_int(),
             mConfMotorSpd[1].to_int() );
    fb.init_file( buf );
    fb.allow_multi_select( false );
    fb.add_filter( "Scan data file (*." SCAN_DATA_FORMAT_HEADER ")", "*." SCAN_DATA_FORMAT_HEADER );

    auto path = fb();
    if ( path.empty() == false ) {
        SaveCurrentImage( GetViewingImage(), path.front().c_str() );
    }
}

void ScannerMainForm::SetAutosavePath()
{
    nana::filebox fb { *this, false };
    fb.add_filter( "Scan data file (*." SCAN_DATA_FORMAT_HEADER ")", "*." SCAN_DATA_FORMAT_HEADER );
    fb.allow_multi_select( false );

    auto path = fb();
    if ( path.empty() == false ) {
        mAutoSavePath = path.front();
        mAutoSavePathDisplay.caption( mAutoSavePath );
    }
}

void ScannerMainForm::OpenDepthMapFile()
{
    nana::filebox fb { *this, true };
    fb.add_filter( "Scan data file (*." SCAN_DATA_FORMAT_HEADER ")", "*." SCAN_DATA_FORMAT_HEADER );
    fb.allow_multi_select( true );

    auto paths = fb();
    if ( paths.empty() && fb.path().empty() == false ) {
        paths.push_back( fb.path() );
    }

    // Before begin, destroy all unused windows
    for ( int i = mUnnamedForms.size() - 1; i >= 0; i-- ) {
        auto& r = mUnnamedForms[i];
        if ( r->visible() == false ) {
            r = std::move( mUnnamedForms.back() );
            mUnnamedForms.pop_back();
        }
    }

    // For each paths
    for ( auto& path : paths ) {
        // Load image from selected path
        //! @todo. Find wfopen()'s linux version.
        ifstream           fs { path.c_str(), ios::binary };
        FPxlData*          ptr;
        ScanDataHeaderType ods;
        if ( scanlib::ScanDataReadFrom( fs, (ScanDataPixelType**)&ptr, &ods ) ) {
            // Create descriptor from loaded image
            FScanImageDesc desc { ods.WIDTH, ods.HEIGHT, ods.ASPECT_RATIO, ptr };

            // New image form
            auto& frm     = *mUnnamedForms.emplace_back( make_unique<form>( *this ) );
            auto& view    = static_cast<ScannerViewerWidget&>( *mUnnamedRefs.emplace_back( make_unique<ScannerViewerWidget>( frm, desc ) ) );
            auto  pathstr = path.string();
            frm.div( "ALL margin=15" );
            frm.size( { 800, 600 } );
            frm["ALL"] << view;
            frm.caption( pathstr.substr( pathstr.find_last_of( '\\' ) ) );
            frm.collocate();
            view.bgcolor( color {}.from_rgb( 53, 53, 53 ) );
            frm.show();
        }
    }
}

void ScannerMainForm::SaveCurrentImage( FScanImageDesc const& desc, wchar_t const* PATH )
{
    wprintf( L"Trying save file into %s...\n", PATH );

    //! @todo. Find wfopen()'s linux version.
#ifdef _WIN32
    FILE* fp = _wfopen( PATH, L"wb" );
#endif

    if ( fp == NULL )
        return;

    ScanDataWriteTo( fp, desc.CData(), desc.Width, desc.Height, desc.AspectRatio );

    fclose( fp );
}

void ScannerMainForm::StartCapture()
{
    mRetriggerCnt = ( 500ms / UpdatePeriod ) + 1;

    auto& c           = mCaptureParam;
    c.bPrescisionMode = mPrecisionMode.checked();
    c.CaptureDelayUs  = mConfDelay.to_int();
    c.DesiredAngle.emplace( mConfAngle[0].to_double(), mConfAngle[1].to_double() );
    c.DesiredOffset.emplace( mConfOfst[0].to_double(), mConfOfst[1].to_double() );
    c.DesiredResolution.emplace( mConfResolution[0].to_int(), mConfResolution[1].to_int() );

    if ( mScan->IsDeviceRunning() ) {
        mScan->TryPauseOrResume();
    }
    else {
        int slow = mConfMotorSpd[0].to_int();
        int fast = mConfMotorSpd[1].to_int();
        mScan->SetMotorDriveClockSpeed( fast );
        mScan->SetMotorAcceleration( slow );
        mScan->BeginCapture( &c );
    }
}

void ScannerMainForm::StopCapture() { mScan->StopCapture(); }

void ScannerMainForm::RequestMotorMovement()
{
    // Create input box
    inputbox inp { *this, "Enter motor angle to request movement", "Motor Movement Request" };

    inputbox::real angle_x { "X angle", 0.0, -180.0, 180.0, 3 };
    inputbox::real angle_y { "Y angle", 0.0, -180.0, 180.0, 3 };

    if ( !inp.show( angle_x, angle_y ) ) {
        return;
    }

    auto x = angle_x.value();
    auto y = angle_y.value();

    // Calculate motor steps from angle
    int sx = static_cast<int>( x / mLastStat.DegreePerStepX );
    int sy = static_cast<int>( y / mLastStat.DegreePerStepY );

    // Send reqeust to move motor
    print( "Requesting motor movement ... by %d, %d\n", sx, sy );
    mScan->RequestMotorMovement( sx, sy );
}

void ScannerMainForm::OpenStepPerAngleSettingDialog()
{
    inputbox inp { *this, "Enter motor angle per step", "Set Degrees per Step" };

    inputbox::real angle_x { "X angle", 1.8 / 32, 0.0, 10.0, 0.0001 };
    inputbox::real angle_y { "Y angle", 1.8 / 32, 0.0, 10.0, 0.0001 };

    if ( !inp.show( angle_x, angle_y ) ) {
        return;
    }

    auto x = angle_x.value();
    auto y = angle_y.value();

    printf( "Resetting steps per angle ... %f, %f \n", x, y );
    mScan->SetDegreesPerStep( x, y );
}

void ScannerMainForm::OpenMotorDrvClkSettingDialog()
{
    inputbox inp { *this, "Set motor drive clock speed", "Set Motor Speed" };

    inputbox::integer clk { "Hz", 1000, 1, 100000, 100 };

    if ( !inp.show( clk ) ) {
        return;
    }

    auto v = clk.value();
    mScan->SetMotorDriveClockSpeed( v );
}

void ScannerMainForm::UpdateReportText()
{
}

void ScannerMainForm::UpdateTimerEventHandler()
{
    // Retry connect
    cbool         bIsConnect          = mScan->IsConnected();
    future_status wait_result         = {};
    cbool         bAsyncProcessActive = mComSearchTask.valid() && ( wait_result = mComSearchTask.wait_for( 0ms ) ) != future_status::ready;
    cbool         bAsyncProcessDone   = mComSearchTask.valid() && wait_result == future_status ::ready;
    cbool         bConnectResult      = bAsyncProcessDone && mComSearchTask.get();

    if ( !bIsConnect && ( !bAsyncProcessActive || ( bAsyncProcessDone && !bConnectResult ) ) ) {
        mStatusText    = "-- No connection -- ";
        mComSearchTask = async( launch::async, [this]() {
            print( "Finding scanner connection ... \n" );
            cbool res = API_RefreshScannerControl( *mScan );
            if ( res == false ) {
                print( "Failed to find connection\n" );
                this_thread::sleep_for( 500ms );
            }
            return res;
        } );
    }

    // Let timer to retrigger if flag is active
    if ( bInStartMode
         && mScan->IsDeviceRunning() == false
         && bEnableAutoRestart
         && mRetriggerCnt-- == 0 ) {
        print( "Auto re-triggering capture ... \n" );
        StartCapture();
    }

    // Update start button status
    if ( mScan->IsDeviceRunning() ) {
        mStartScan.caption( mScan->IsPaused() ? "Resume" : "Pause" );
        mMenualCommand.bgcolor( mScan->IsPaused() ? color().from_rgb( 255, 255, 0 ) : color().from_rgb( 255, 125, 125 ) );
    }
    else {
        mStartScan.caption( "Start Capture" );
        mMenualCommand.bgcolor( color().from_rgb( 125, 255, 125 ) );
    }

    // Update idle state indicator
    mStatus.reset( mStatusText );

    if ( auto now = chrono::system_clock::now(); now - mLastReportReqTime > ReportPeriod ) {
        mLastReportReqTime = now;
        mScan->Report();
    }
}

static constexpr char* divtxt =
    R"(
vert<margin=10><<><settings margin=10 weight = 560><> weight=160>
)";

ScannerViewerWidget::ScannerViewerWidget( nana::form& w, FScanImageDesc const& desc )
    : picture( w )
{
    init( desc );
}

ScannerViewerWidget::~ScannerViewerWidget()
{
    mAsyncDraw.wait();
}

void ScannerViewerWidget::init( FScanImageDesc const& desc )
{
    if ( desc.CData() )
        mImgDesc = desc.Clone();

    mLayout.bind( *this );
    mLayout.div( divtxt );

    // div( "ALLVIEW margin=22" );
    // ( *this )["ALLVIEW"] << mViewport;
    mConfigGroup.create( *this );
    // mViewport.bgcolor( color().from_rgb( 175, 175, 175 ) );

    // Assign viewport event
    mViewport.events().mouse_wheel( [this]( arg_wheel const& arg ) {
        double v = mConfZoom.to_double();
        v *= arg.upwards ? 1.33 : 1 / 1.33;
        mConfZoom.value( to_string( v ) );
    } );
    mViewport.events().mouse_move( [this]( arg_mouse const& arg ) {
        auto& prv = mViewporPreviousMousePos;
        auto  d   = arg.pos - prv;

        if ( arg.left_button ) {
            double zoom = mConfZoom.to_double();
            mConfXPos.value( to_string( mConfXPos.to_double() + d.x / ( zoom * 0.01 ) ) );
            mConfYPos.value( to_string( mConfYPos.to_double() + d.y / ( zoom * 0.01 ) ) );
        }

        prv = arg.pos;
    } );

    mViewportDraw = make_unique<drawing>( mViewport );
    mViewportDraw->draw( [this]( paint::graphics& graph ) {
        viewportDraw( graph );
    } );
    mLayout["settings"] << mConfigGroup;
    mLayout.collocate();

    // Initalize configuration group
    mConfigGroup.caption( "" );
    mConfigGroup.div( "<vert gap = 20 margin = 10"
                      "<<rangetxt weight= 44%>  <mind><maxd>>"
                      "<<postxt weight= 44%>    <posx><posy>> "
                      "<<zoomtxt weight= 44%>   <zoom>>"
                      "<<calibtxt weight= 44%>  <calibv><ampv>>"
                      ">" );

    nana::spinbox* boxs[] = {
        &mConfMinDist,
        &mConfMaxDist,
        &mConfXPos,
        &mConfYPos,
        &mConfZoom,
        &mConfCalib };
    double init_min_max_step[countof( boxs )][4] = {
        { 0.0, 0.0, 100.0, 0.25 },
        { 3.0, 0.0, 100.0, 0.25 },
        { 0, -1000000000, 1000000000, 1 },
        { 0, -1000000000, 1000000000, 1 },
        { 100.0, 0.0001, 100000, 0.5 },
        { 0, -25, 25, 1 },
    };
    const char* divs[] = {
        "mind", "maxd", "posx", "posy", "zoom", "calibv" };

    for ( size_t i = 0; i < countof( boxs ); i++ ) {
        auto& bx = *boxs[i];
        bx.create( mConfigGroup );
        mConfigGroup[divs[i]] << bx;
        bx.range( init_min_max_step[i][1], init_min_max_step[i][2], init_min_max_step[i][3] );
        bx.value( to_string( init_min_max_step[i][0] ) );
        bx.bgcolor( color().from_rgb( 215, 215, 215 ) );

        if ( i >= 2 && i != 5 )
            bx.events().text_changed( [this]( auto& ) { refreshScreen(); } );
        else
            bx.events().text_changed( [this]( auto& ) { rerenderBuf(); } );
    }

    mConfCalib.range( -25, 25, 1 );
    mConfCalib.value( "0" );
    mConfMinDist.bgcolor( color().from_rgb( 155, 155, 255 ) );
    mConfMaxDist.bgcolor( color().from_rgb( 255, 155, 155 ) );
    mConfRenderAmp.create( mConfigGroup );
    mConfigGroup["ampv"] << mConfRenderAmp;
    mConfRenderAmp.bgcolor( mConfigGroup.bgcolor() );
    mConfRenderAmp.caption( "Render Amplitude" );
    mConfRenderAmp.events().checked( [this]( auto& ) { rerenderBuf(); } );

    const char* divs2[] = {
        "rangetxt", "postxt", "zoomtxt", "calibtxt" };

    const char* txts[] = {
        "Distance [min, MAX] [meter]", "Position [x, y]", "Zoom value [%]", "Horizontal Calibration" };

    for ( size_t i = 0; i < countof( mConfTexts ); i++ ) {
        auto& lb = mConfTexts[i];
        lb.create( mConfigGroup );
        lb.caption( txts[i] );
        lb.bgcolor( mConfigGroup.bgcolor() );
        mConfigGroup[divs2[i]] << lb;
    }
    mConfigGroup.collocate();
    // collocate();

    // Color mode bar
    mMenu.create( *this );
    auto& cmod = mMenu.push_back( "Color Mapping" );
    cmod.append( "GBR Range", [this]( auto ) { mColorMode = ColorMappingMode::BGR; rerenderBuf(); } );
    cmod.append( "Grayscale", [this]( auto ) { mColorMode = ColorMappingMode::GREYSCALE; rerenderBuf(); } );
    cmod.append( "Rainbow", [this]( auto ) { mColorMode = ColorMappingMode::RAINBOW; rerenderBuf(); } );
    cmod.append( "WTOK Slide", [this]( auto ) { mColorMode = ColorMappingMode::WTOK; rerenderBuf(); } );

    rerenderBuf();
}

void ScannerViewerWidget::rerenderBuf()
{
    bPendingRender = true;
    if ( mAsyncDraw.valid() && mAsyncDraw.wait_for( 0ms ) != future_status::ready ) {
        return;
    }

    mAsyncDraw = async( launch::async, [this]( void ) {
        while ( bPendingRender.exchange( false ) ) {
            auto required = mImgDesc.Width * mImgDesc.Height;
            if ( mTmpBufSz < required ) {
                mTmpBuf   = make_unique<uint32_t[]>( required );
                mTmpBufSz = required;
            }

            RenderImage(
                mImgDesc, mViewportBuf[!mFwd],
                mConfRenderAmp.checked(),
                mConfMaxDist.to_double(),
                mConfMinDist.to_double(),
                mConfCalib.to_int(),
                mColorMode,
                mTmpBuf.get() );

            mFwd = !mFwd;
            refreshScreen( true );
        }
    } );
}

void ScannerViewerWidget::refreshScreen( bool bTry )
{
    mViewportDraw->update();
}

void ScannerViewerWidget::viewportDraw( nana::paint::graphics& gr )
{
    // Temporary error handler ...
    // Refresh() is called when form destruction, which causes the form to reference dead instance.
    if ( mConfXPos.visible() == false )
        return;

    // Translate image into buffer
    TranslateInto( gr, mViewportBuf[mFwd],
                   mImgDesc.AspectRatio,
                   mConfXPos.to_double(),
                   mConfYPos.to_double(),
                   mConfZoom.to_double() );
}

void ScannerViewerWidget::TranslateInto(
    nana::paint::graphics& dst,
    nana::paint::graphics& src,
    double                 aspect,
    double                 x,
    double                 y,
    double                 zoom_percent )
{
    double zoom   = zoom_percent / 100.0;
    auto   dstw   = dst.width();
    auto   dsth   = dst.height();
    auto   larger = max( src.width(), src.height() );
    auto   w      = static_cast<unsigned int>( larger * zoom * aspect );
    auto   h      = static_cast<unsigned int>( larger * zoom );
    auto   dx     = (int)std::min( (double)dstw, x * zoom + dstw / 2 - w / 2 );
    auto   dy     = (int)std::min( (double)dsth, y * zoom + dsth / 2 - h / 2 );

    src.stretch( dst, rectangle { dx, dy, w, h } );
};