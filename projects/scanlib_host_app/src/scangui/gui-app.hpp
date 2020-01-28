//! GUI-App class definition
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#pragma once
#include "app.hpp"
#include <deque>
#include <memory>
#include <nana/gui.hpp>
#include <nana/gui/drawing.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/spinbox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/paint/graphics.hpp>
#include <nana/paint/image.hpp>
#include <string>
#include <utility>

class ScannerViewerWidget : public nana::picture {
public:
    ScannerViewerWidget( nana::form& w, FScanImageDesc const& desc );
    ScannerViewerWidget() = default;
    ~ScannerViewerWidget();

    void init( FScanImageDesc const& desc );

    void ReplaceDesc( FScanImageDesc const& desc, bool bShouldClone = false )
    {
        mImgDesc = bShouldClone ? desc.Clone() : desc;
        rerenderBuf();
    }

private:
    enum class ColorMappingMode {
        NONE,
        BGR,
        GREYSCALE,
        WTOK,
        RAINBOW,
    };

private:
    void        rerenderBuf();
    void        refreshScreen( bool bTryLock = false );
    void        viewportDraw( nana::paint::graphics& gr );
    static void TranslateInto(
        nana::paint::graphics& dst,
        nana::paint::graphics& src,
        double                 AspectRatio,
        double                 x,
        double                 y,
        double                 zoom_percent );
    static void RenderImage(
        FScanImageDesc const&,
        nana::paint::graphics& To,
        bool                   bRenderAmp,
        float                  MaxDistance     = 10.0f,
        float                  MinDistance     = 0.0f,
        int                    HorizontalCalib = 0,
        ColorMappingMode       ColorMode       = {},
        void*                  CacheBuff       = NULL );

private:
    nana::place   mLayout = {};
    nana::menubar mMenu   = {};

    nana::picture&                 mViewport                = { *this };
    nana::paint::graphics          mViewportBuf[2]          = {};
    bool                           mFwd                     = true;
    std::unique_ptr<nana::drawing> mViewportDraw            = {};
    nana::point                    mViewporPreviousMousePos = {};

    nana::group    mConfigGroup = {};
    nana::label    mConfTexts[4];
    nana::spinbox  mConfMinDist   = {};
    nana::spinbox  mConfMaxDist   = {};
    nana::spinbox  mConfXPos      = {};
    nana::spinbox  mConfYPos      = {};
    nana::spinbox  mConfZoom      = {};
    nana::spinbox  mConfCalib     = {};
    nana::checkbox mConfRenderAmp = {};

    FScanImageDesc mImgDesc = {};

    //! Async drawer thread
    std::future<void> mAsyncDraw     = {};
    std::atomic_bool  bPendingRender = false;
    ColorMappingMode  mColorMode     = {};

    std::unique_ptr<uint32_t[]> mTmpBuf   = {};
    size_t                      mTmpBufSz = 0;
};

//! Layout script for main form
static char const* ScannerMainFormLayoutScript =
    R"(
vert
<   
    margin=[25, 10, 2, 10] 
    <vert 
        <viewmode weight=45>
        <viewport>
    >
    <
        vert weight=350
        <vert status_pane margin=[2, 10]>
        <vert config_pane margin=[2, 10] weight=225>
        <vert request_pane margin=[2, 10] weight=150>
    >
> 
<
        <report_pane >
        weight=300 margin=[2, 10]
>
)";

#ifdef WIN32
#    define SCANLIB_ASCIIVAL_ENTER   13
#    define SCANLIB_ASCIIVAL_UPARROW 38
#    define SCANLIB_ASCIIVAL_DNARROW 40
#endif

//! Simple set of components that make up application
//! @todo. Improve color range view
//! @todo. Output to png file
//! @todo. Capture file history
class ScannerMainForm : public nana::form {
public:
    ScannerMainForm( FScannerProtocolHandler* mScannerRef, std::string const& fontName = "consolas" );
    ~ScannerMainForm();

    void BindScanner( FScannerProtocolHandler* scanRef );

public:
    size_t                    NumMaxCommandHistory = 512;
    std::chrono::milliseconds UpdatePeriod { 100 };
    std::chrono::milliseconds ReportPeriod { 100 };

    size_t NumMaxImageHistory() const { return mNumMaxImageHistory; }
    void   NumMaxImageHistory( size_t val )
    {
        mNumMaxImageHistory = val;
        UpdateImageHistoryBox( val );
    }

private:
    void OnScannerCaptureDone( FScanImageDesc const& );
    void OnUpdateReport( FDeviceStat const& );
    void OnUpdateImage( FScanImageDesc const& );
    void StartCapture();
    void StopCapture();
    void AutoUpdateImage();
    void SaveCurrentImage( FScanImageDesc const& desc, wchar_t const* PATH );
    void OpenSaveAs();
    void SetAutosavePath();
    void OpenDepthMapFile();
    void RequestMotorMovement();
    void OpenStepPerAngleSettingDialog();
    void OpenMotorDrvClkSettingDialog();
    void UpdateReportText();
    void UpdateTimerEventHandler();
    void UpdateImageHistoryBox( size_t newval );

    FScanImageDesc const& GetViewingImage() const;

    template <typename... args_>
    void print( char const* fmt, args_... args )
    {
        if ( mScan == nullptr )
            return;

        time_t t;
        time( &t );
        auto tm = localtime( &t );
        mScan->print( "system %02d:%02d:%02d:: ", tm->tm_hour, tm->tm_min, tm->tm_sec );
        mScan->print( fmt, std::forward<args_>( args )... );
    }

    template <class widget_, typename... args_>
    widget_& createWidget( args_&&... args )
    {
        mUnnamedRefs.emplace_back( std::make_unique<widget_>( std::forward<args_>( args )... ) );
        return *static_cast<widget_*>( mUnnamedRefs.back().get() );
    }

private:
    FScannerProtocolHandler* mScan;

    //! Local cached capture option
    FScannerProtocolHandler::CaptureParam mCaptureParam;
    //! Scanner status cache
    FDeviceStat mLastStat;

    //! Logics
    nana::timer                           mUpdate            = {};
    std::chrono::system_clock::time_point mLastReportReqTime = {};

    //! File output path
    bool         bFileEnableAutosave : 1;
    bool         bFileSaveDepthMapRgb : 1;
    bool         bFileSaveAmpMapRgb : 1;
    bool         bEnableAutoRestart : 1;
    bool         bInStartMode : 1;
    uint8_t      mRetriggerCnt = 0;
    std::wstring mAutoSavePath;
    nana::label  mAutoSavePathDisplay;

    //! Primary layout
    nana::place mLayout = { *this };

    //! Widgets
    nana::menubar mMenu = { *this };

    nana::group mConfigGroup = { *this };
    nana::label mConfText[5] = {};

    nana::spinbox mConfOfst[2]       = {}; //!< Each value indicates x, y
    nana::spinbox mConfAngle[2]      = {};
    nana::spinbox mConfResolution[2] = {};
    nana::spinbox mConfDelay         = {};
    nana::spinbox mConfMotorSpd[2]   = {};

    nana::group    mRequestGroup  = { *this };
    nana::button   mStartScan     = {};
    nana::button   mStopScan      = {};
    nana::button   mMoveMotorBtn  = {};
    nana::checkbox mPrecisionMode = {};

    nana::group   mStatusGroup = { *this };
    nana::textbox mStatus      = {};
    std::string   mStatusText  = {};

    nana::paint::graphics      mViewportImageBuff = {};
    nana::group                mViewModeSelect    = { *this };
    nana::checkbox             mViewAspectRatio   = {};
    nana::group                mViewportGroup     = { *this };
    ScannerViewerWidget        mViewport          = {};
    nana::spinbox              mViewIndexInput    = {};
    nana::label                mViewIndexLabel    = {};
    std::deque<FScanImageDesc> mCapturedImage     = {};
    size_t                     mViewImgIndex      = {};

    nana::group             mReportGroup    = { *this };
    nana::textbox           mReport         = {};
    nana::textbox           mMenualCommand  = {};
    std::deque<std::string> mCommandHistory = {};
    size_t                  mCommandCursor  = {};

    std::atomic_bool bShouldRerender = false;

    std::vector<std::unique_ptr<nana::widget>> mUnnamedRefs;
    std::vector<std::unique_ptr<nana::form>>   mUnnamedForms;

    std::future<bool> mComSearchTask = {};

    nana::paint::font mFormatFont = {};

    size_t mNumMaxImageHistory = 100;
};
