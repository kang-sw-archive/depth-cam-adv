#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <vector>
#include "../common/scanner_protocol.h"
#include "communication_handler.hpp"

/**
 * @brief Defines procedure parameters
 **/
struct FCommunicationProcedureInitStruct
{
    size_t TimeoutMs                 = 1000;       //!< Timeout in milliseconds. -1 to no timeout
    size_t ReceiveBufferSize         = 4096;       //!< Buffer size used internally.
    size_t ConnectionRetryCount      = (size_t)-1; //!< Number of retries.
    size_t ConnectionRetryIntervalMs = 500;        //!<
};

//! Scanned image buffer descriptor.
//! Read-only.
struct FScanImageDesc
{
    int   Width       = {}; //!< Image width in pixels
    int   Height      = {}; //!< Image height in pixels
    float AspectRatio = {}; //!< Aspect ratio from angles

    //! Returns read-only data pointer.
    FPxlData const* Data() const noexcept { return mReadData; }
    FPxlData const* CData() const noexcept { return mReadData; }

    //! Returns modifiable pointer only when this is the owner of reference.
    FPxlData* Data() noexcept { return mData; }

    //! Returns clone of active instance.
    FScanImageDesc Clone() const noexcept;

    FScanImageDesc( size_t w, size_t h, float aspect, FPxlData* RawPtr ) noexcept;
    FScanImageDesc( size_t w, size_t h, float aspect ) noexcept;
    FScanImageDesc( FScanImageDesc const& ) noexcept;
    FScanImageDesc( FScanImageDesc&& ) noexcept;
    FScanImageDesc& operator=( FScanImageDesc const& ) noexcept;
    FScanImageDesc& operator=( FScanImageDesc&& ) noexcept;
    FScanImageDesc( FPxlData const* p = nullptr )
        : mReadData( p )
    {
    }
    ~FScanImageDesc() noexcept;

private:
    FPxlData*       mData     = {}; //!< Data pointer only available when cloned.
    FPxlData const* mReadData = {}; //!< Read-only data pointer
};

//! Scanner protocol handler. Wraps communication handler base's
//! functionalities. Provides:
//! - Asynchronous communication handler procedure
//! - Connection check via ping
//! - Abstract buffer management
//! @todo. Save configuration as file
class FScannerProtocolHandler : public ICommunicationHandlerBase
{
public:
    std::function<void( FScanImageDesc const& )> OnFinishScan;
    std::function<void( FScanImageDesc const& )> OnReceiveLine;
    std::function<void( const FDeviceStat& )>    OnReport;
    std::function<void( char const* )>           Logger;
    std::function<void( FPointData const& )>     OnPointRecv;
    bool                                         bSuppressDeviceLog = false;

public:
    using PortOpenFunctionType = std::function<std::unique_ptr<std::streambuf>( FScannerProtocolHandler& )>;
    using super                = ICommunicationHandlerBase;

public:
    //! Prevents hiding base class constructor.
    FScannerProtocolHandler()
        : ICommunicationHandlerBase()
    {
    }
    ~FScannerProtocolHandler();

    //! Initializes communication process on different thread.
    //! This function returns control right away.
    //! @param COM: COM port name.
    //! @param params: Required parameters to initiate procedure
    enum ActivateResult
    {
        ACTIVATE_OK              = 0,
        ACTIVATE_INVALID_COM     = -1,
        ACTIVATE_ALREADY_RUNNING = -2,
    } Activate( PortOpenFunctionType ComOpener, FCommunicationProcedureInitStruct const& params, bool bAsync = true ) noexcept;

    //! Check if connection is alive
    bool IsConnected() const noexcept;

    //! Check if background process is running
    bool IsActive() const noexcept;

    //! Returns current scanner state descriptor
    FDeviceStat GetDeviceStatus() const noexcept;

    //! Get image information
    //! Returns true if complete image exists.
    bool GetCompleteImage( FScanImageDesc& out ) const noexcept;

    //! Check if there's image that already complete.
    bool CheckCompleteImageExists() const noexcept { return !mCompleteImage.empty(); }

    //! Get currently scanning image
    //! Returns false if there is no operation.
    bool GetScanningImage( FScanImageDesc& out ) const noexcept;

    //! Required Capture parameters for function BeginCapture();
    struct CaptureParam
    {
        template <typename ty_>
        struct Point
        {
            Point( ty_ xx = ty_ {}, ty_ yy = ty_ {} )
                : x( xx )
                , y( yy )
            {
            }
            ty_ x, y; //!< To keep options as current, Set this value negative
                      //!< value e.g. -1
        };
        std::optional<Point<int>> DesiredResolution;     //!< Desired image resolution. Can be modified
                                                         //!< with other conditions.
        std::optional<Point<float>> DesiredAngle;        //!< Angular width and height
        std::optional<Point<float>> DesiredOffset;       //!< Angular offset from beginning point.
        int                         CaptureDelayUs = -1; //!< Delay time per capture in microseconds.
        std::optional<bool>         bPrescisionMode;     //!< If true, short-range
                                                         //!< high-precision mode activates.
    };

    //! Request begin capture
    //! @param params: Pass nullptr here to keep previous settings.
    //! @returns false when the device is not connected.
    bool BeginCapture( CaptureParam const* params = nullptr, size_t TimeoutMs = 1000 );

    //! Try resume operation
    void TryPauseOrResume() { SendString( "capture scan-start" ); }

    //! Check if paused
    bool IsPaused() const noexcept { return IsDeviceRunning() && mStat.load().bIsPaused; }

    //! Check whether device is running
    //! Updates status descriptor internally.
    bool IsDeviceRunning() const noexcept;

    //! Stop capture synchronously
    void StopCapture() noexcept;

    //! Request Report
    void Report() noexcept { SendString( "capture report" ); }

    //! Configure capture
    //! Enforces reinitialization

    //! Move motor position by given amount
    void RequestMotorMovement( int xstep, int ystep ) noexcept;

    //! Set motor drive clock speed
    void SetMotorDriveClockSpeed( int Hz ) noexcept;

    //! Set motor large movement clock speed.
    //! This is to reduce physical impact on large movement
    void SetMotorAcceleration( int Hz ) noexcept;

    //! Relocate motor to origin point

    //! Reset motor origin as current point
    void ResetMotorPosition() noexcept;

    //! Try shutdown
    //! Returns control immediately if background process is already shutdown.
    void Shutdown() noexcept;

    //! Sets degree per steps of device.
    //! Use this when using another motor / driver device.
    void SetDegreesPerStep( float x, float y ) noexcept;

    //! Send test signal
    void Test() noexcept { SendString( "test" ); }

    //! Output log
    void print( char const* fmt, ... ) const noexcept;

protected:
    virtual void OnString( char const* str ) override;
    virtual void OnBinaryData( char const* data, size_t len ) override;

private:
    void procedureThread( PortOpenFunctionType ComOpener, FCommunicationProcedureInitStruct params ) noexcept;
    bool requestReport( bool bSync, size_t TimeoutMs );
    void configureCapture( CaptureParam const& arg, bool bForce );

private:
    template <typename arg_>
    struct LockArg
    {
        std::condition_variable cv;
        std::mutex              mtx;
        arg_                    arg;
    };

private:
    //! Background process reference.
    std::future<void> mBackgroundProcess = {};
    //! Device status descriptor
    FDeviceStat mStatCache = {};
    //!
    FDeviceStat mCompleteImageStat = {};
    //! Cached previous image capture parameters.
    std::optional<CaptureParam> mPrevCaptureParam = {};
    //! Device status descriptor
    std::atomic<FDeviceStat> mStat;
    std::vector<FPxlData>    mImage;
    //! Complete image that finished scanning.
    std::vector<FPxlData> mCompleteImage;
    //! For waiting report update ...
    LockArg<std::atomic_bool> mReportWait;
    //! Connection flag ...
    std::atomic_bool bIsConnected = false;
    //! Controls whether the async process should be done.
    std::atomic_bool bShutdown = false;
    //! To discard previous requests
    std::atomic_bool bRequestingCapture = false;
};