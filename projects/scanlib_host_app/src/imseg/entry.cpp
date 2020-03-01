//! @file       entry.cpp
//! @brief      File brief description
//!
//! @author     Seungwoo Kang (ki6080@gmail.com)
//! @copyright  Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
//!             File detailed description
//! @todo       Find center of each super pixels
//! @todo       Calibrate motor and camera center position
//! @todo
#include <chrono>
#include <cstdarg>
#include <future>
#include <gflags/gflags.h>
#include <iostream>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc/lsc.hpp>
#include <opencv2/ximgproc/seeds.hpp>
#include <opencv2/ximgproc/slic.hpp>
#include <optional>
#include <scanlib/arch/utility.hpp>
#include <scanlib/core/scanner_protocol_handler.hpp>

#include <SlicCudaHost.h>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

/////////////////////////////////////////////////////////////////////////////
// GLOBAL FLAGS
DEFINE_int32( cam_index, 1, "Specify camera index to use" );
DEFINE_int32( desired_pixel_cnt, int( 4e5 ), "Number of intended pixels" );
DEFINE_double( slic_compactness, 65.f, "Pixel compactness" );
DEFINE_int32( desired_superpixel_cnt, 16000, "Number of desired super pixels" );
DEFINE_double( vertical_fov, 38, "Camera vertical FOV in degree" );
DEFINE_int32( ocl_dev_idx, 0, "Specify open-cl device index" );

DEFINE_double(
  path_x_tolerance,
  0.06f,
  "X axis movement tolerance of Depscan" );
DEFINE_int32( device_x_accel, 32400, "Stepper motor acceleration in Hz/s" );
DEFINE_int32( device_y_accel, 544800, "Stepper motor acceleration in Hz/s" );
DEFINE_int32(
  device_sample_delay,
  6000,
  "Distance sensor delay in microseconds" );
/////////////////////////////////////////////////////////////////////////////
// Static types
using depth_t = struct
{
    float Amp, Range;
};

/////////////////////////////////////////////////////////////////////////////
// Static method declares
static void
FindCenters( cv::Mat Contour, std::vector<cv::Point2f>& Out, size_t NumSpxls );

static bool InitializeMeasurementDevice();
static bool MeasureSampleDepths(
  std::vector<cv::Point2f> const& Points,
  std::vector<depth_t>*           Depths,
  std::chrono::milliseconds       DeviceTimeout );

static std::optional<cv::Mat> CaptureDepthImage( cv::Mat FrameData );

/////////////////////////////////////////////////////////////////////////////
// Logging utility
static std::string stringf( char const* fmt, ... );
#define LOG_INFO( fmt, ... ) CV_LOG_INFO( nullptr, stringf( fmt, __VA_ARGS__ ) )
#define LOG_VERBOSE( fmt, ... )                                                \
    CV_LOG_VERBOSE( nullptr, stringf( fmt, __VA_ARGS__ ) )
#define LOG_WARNING( fmt, ... )                                                \
    CV_LOG_WARNING( nullptr, stringf( fmt, __VA_ARGS__ ) )
#define LOG_ERROR( fmt, ... )                                                  \
    CV_LOG_ERROR( nullptr, stringf( fmt, __VA_ARGS__ ) )
#define LOG_FATAL( fmt, ... )                                                  \
    CV_LOG_FATAL( nullptr, stringf( fmt, __VA_ARGS__ ) )

/////////////////////////////////////////////////////////////////////////////
// Exported
FScannerProtocolHandler gScan;

/////////////////////////////////////////////////////////////////////////////
// Static Declares / Data

static std::vector<cv::Point2f> Centers;
static std::vector<depth_t>     Depths;
static size_t                   NumSpxls;
static float                    AspectRatio;
static SlicCuda                 GpuSLIC;
static bool                     bScannerValid;

/////////////////////////////////////////////////////////////////////////////
// Core lop
int main( int argc, char* argv[] )
{
    cv::VideoCapture Video;
    cv::Mat          FrameData;
    cv::cuda::GpuMat CudaFrame;

    std::future<std::optional<cv::Mat>> FrameTask;

    gflags::ParseCommandLineFlags( &argc, &argv, true );
    Video.open( FLAGS_cam_index );

    cv::ocl::setUseOpenCL( true );

    // Enumerate GPU devices
    {
        cv::ocl::Context context;
        if ( !context.create( cv::ocl::Device::TYPE_GPU ) )
        {
            cout << "Failed creating the context..." << endl;
            // return;
        }

        cout << context.ndevices() << " GPU devices are detected."
             << endl; // This bit provides an overview of the OpenCL devices you
                      // have in your computer
        for ( int i = 0; i < context.ndevices(); i++ )
        {
            cv::ocl::Device device = context.device( i );
            cout << "name:              " << device.name() << endl;
            cout << "available:         " << device.available() << endl;
            cout << "imageSupport:      " << device.imageSupport() << endl;
            cout << "OpenCL_C_Version:  " << device.OpenCL_C_Version() << endl;
            cout << endl;
        }

        cv::ocl::Device( context.device( 1 ) );
    }

    if ( Video.isOpened() == false )
    {
        CV_LOG_ERROR( nullptr, "Failed to open camera" );
        return -1;
    }

    // Configure video resolution ... Set pixel count as the desired
    // configuration
    {
        // Query the camera's maximum pixel count
        Video.set( cv::CAP_PROP_FRAME_WIDTH, 1e6 );
        Video.set( cv::CAP_PROP_FRAME_HEIGHT, 1e6 );
        auto w = Video.get( cv::CAP_PROP_FRAME_WIDTH );
        auto h = Video.get( cv::CAP_PROP_FRAME_HEIGHT );

        // Calculate required reduction ratio
        auto DesiredImageScale = sqrt( FLAGS_desired_pixel_cnt / ( w * h ) );
        w *= DesiredImageScale;
        h *= DesiredImageScale;

        Video.set( cv::CAP_PROP_FRAME_WIDTH, w );
        Video.set( cv::CAP_PROP_FRAME_HEIGHT, h );
        w = Video.get( cv::CAP_PROP_FRAME_WIDTH );
        h = Video.get( cv::CAP_PROP_FRAME_HEIGHT );
        LOG_INFO( "Video resolution is set to %.0f %.0f", w, h );

        AspectRatio = w / h;
    }

    // Create Super pixel object
    Video >> FrameData; // Dummy frame to load meta data
    if ( FrameData.empty() )
    {
        LOG_ERROR( "Failed to capture first frame from Video \n" );
        return -1;
    }
    GpuSLIC.initialize(
      FrameData,
      FLAGS_desired_superpixel_cnt,
      SlicCuda::SLIC_NSPX,
      FLAGS_slic_compactness );

    // Open named window
    cv::namedWindow( "camera" );
    cv::namedWindow( "depth" );
    cv::namedWindow( "active" );

    // Reset device's origin point
    while ( InitializeMeasurementDevice() == false )
    {
        LOG_INFO(
          "Device connection failed. Retrying ... (Press any key to abort)" );
        if ( cv::waitKey( 500 ) != -1 )
        {
            return -1;
        }
    }
    LOG_INFO( "Successfully connected to DepScan device.\n" );
    gScan.QueuePoint( 0, 0, 0 );

    // Initialize super-pixel object
    for ( ;; )
    {
        if ( auto key = cv::waitKey( 33 ); key == -1 )
        {
            Video >> FrameData;
            imshow( "active", FrameData );
            continue;
        }
        else if ( key == 27 )
        {
            break;
        }

        for ( ;; )
        {
            if ( cv::waitKey( 33 ) == 27 )
                return -1;

            if ( FrameTask.valid() == false )
            {
                FrameTask = async( CaptureDepthImage, FrameData );
                continue;
            }

            if ( FrameTask.wait_for( 0ms ) != future_status::ready )
            {
                continue;
            }

            if ( auto Depth = FrameTask.get(); Depth )
            {
                cv::imshow( "camera", FrameData );
                cv::imshow( "depth", Depth.value() / 5.0f );
                gScan.QueuePoint( 0, 0, 0 );
                break;
            }
        }
    }

    CV_LOG_INFO( nullptr, "Shutting down ... " );
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Logics

static std::optional<cv::Mat> CaptureDepthImage( cv::Mat Frame )
{
    // Scale image for optimization
    // cv::resize( Frame, Frame, DesiredImageSize );
    cv::Mat Out;

    CV_LOG_INFO( nullptr, "-- START OF NEW FRAME --" );

    // Transfer it into actual data - UMat
    auto GpuFrame  = Frame.getUMat( cv::ACCESS_READ );
    auto TimeBegin = system_clock::now();

    // Apply SLIC algorithms
    GpuSLIC.segment( Frame );
    GpuSLIC.enforceConnectivity();
    auto TimeIterDone = system_clock::now();

    LOG_INFO(
      "Time consumed to iterate: %ul",
      duration_cast<milliseconds>( TimeIterDone - TimeBegin ).count() );

    cv::Mat Contour = GpuSLIC.getLabels().clone();
    Contour.convertTo( Contour, CV_32S );

    auto TimeContourDone = system_clock::now();
    LOG_INFO(
      "Time Consumed to Extract Contour: %ul ",
      duration_cast<milliseconds>( TimeContourDone - TimeIterDone ).count() );

    // Get number of super pixels
    {
        double Max;
        cv::minMaxLoc( Contour, nullptr, &Max );
        NumSpxls = static_cast<size_t>( Max + 1.5 );
    }

    // Find center of each super pixels
    FindCenters( Contour, Centers, NumSpxls );
    auto TimeCenterLookupDone = system_clock::now();
    LOG_INFO(
      "Time Consumed to Calculate Centers: %ul",
      duration_cast<milliseconds>( TimeCenterLookupDone - TimeContourDone )
        .count() );

    //! @todo Implement operations below.

    // Transforms collected centers into the motor's step dimension, then
    // performs measurement for all central pixels
    if ( MeasureSampleDepths( Centers, &Depths, 1000ms ) == false )
    {
        LOG_WARNING( "Sampling failed. Discarding current frame." );
        return {};
    }
    auto TimeDepthSamplingDone = system_clock::now();
    LOG_INFO(
      "Time Consumed to Sample Depths: %ul",
      duration_cast<milliseconds>(
        TimeDepthSamplingDone - TimeCenterLookupDone )
        .count() );

    // Fill depth map with initial values
    auto    NumRows = Contour.rows;
    auto    NumCols = Contour.cols;
    int     Sizes[] = { NumRows, NumCols };
    cv::Mat DepthMap { 2, Sizes, CV_32F };
    {
        for ( size_t i = 0; i < NumRows; i++ )
        {
            auto LabelRow = Contour.ptr<int>( i );
            auto DepthRow = DepthMap.ptr<float>( i );
            for ( size_t j = 0; j < NumCols; j++ )
            {
                DepthRow[j] = Depths[LabelRow[j]].Range;
            }
        }
    }

#if 1 // Debug display ...
    cv::Mat DebugContour;
    {
        cv::UMat GpuContour;
        Contour.copyTo( GpuContour );
        cv::Laplacian( GpuContour, GpuContour, -1 );
        cv::compare( GpuContour, 0, GpuContour, cv::CMP_NE );
        cv::cvtColor( GpuContour, GpuContour, cv::COLOR_GRAY2BGR );
        GpuContour.convertTo( GpuContour, CV_8UC3 );
        cv::add( GpuContour, GpuFrame, Frame );
        GpuContour.copyTo( DebugContour );
    }
#endif
#if 0
    {
        Out = DepthMap.clone();
        Out = 1.0f - ( Out / 5.0f );
    }
#endif
    // Apply Gaussian blurring on gradient contour.
    // Superpixels that have a close relationship should be interpolated
    // smoothly. On the other hand, the cliff, which indicates a large
    // difference between Superpixels, should not be interpolated.
    {
        cv::Mat BlurImage;
        DepthMap.copyTo( BlurImage );

        cv::bilateralFilter( DepthMap, BlurImage, 0, 0.16, 14.0 );
        BlurImage.copyTo( Out );
    }

    // Calculates the distance between each super-pixels then create a
    // distance matrix to select nearby super-pixels to evaluate.

    // Apply blurring operation to depth image based on the relationship
    // between super-pixels.

    return Out;
}

std::string stringf( char const* fmt, ... )
{
    va_list vpa, vpb;
    va_start( vpa, fmt );
    va_copy( vpb, vpa );

    auto        n = vsnprintf( NULL, 0, fmt, vpa );
    std::string str;
    str.resize( n + 2 );
    vsnprintf( str.data(), str.size(), fmt, vpb );

    return str;
    va_end( vpa );
    va_end( vpb );
}
/////////////////////////////////////////////////////////////////////////////
// Helpers

static void
FindCenters( cv::Mat Contour, std::vector<cv::Point2f>& Out, size_t NumSpxls )
{
    Out.clear();
    auto             NumRows = Contour.rows;
    auto             NumCols = Contour.cols;
    std::vector<int> NumPxls; // Number of pixels for each super pixel
    size_t           ArrayLen = NumSpxls;

    // Prepare for output data array
    Out.resize( ArrayLen, {} );
    NumPxls.resize( ArrayLen, 0 );
    LOG_INFO( "Number of label: %d", Out.size() );
    CV_Assert( Contour.channels() == 1 );

    // For each rows ...
    for ( decltype( NumRows ) i = 0; i < NumRows; i++ )
    {
        auto RowPtr = Contour.ptr<int>( i );
        for ( decltype( NumCols ) k = 0; k < NumCols; k++ )
        {
            Out[RowPtr[k]] += cv::Point2f { float( k ), float( i ) };
            NumPxls[RowPtr[k]]++;
        }
    }

    for ( size_t i = 0; i < ArrayLen; i++ )
    {
        auto& pt  = Out[i];
        auto  num = NumPxls[i];

        auto& x = pt.x;
        auto& y = pt.y;

        // Calculates mean center position.
        // Output value will be normalized by aspect ratio.
        // x [0, AspectRatio), y [0, 1)
        x /= num * NumRows;
        y /= num * NumRows;
    }

    // done.
}

bool InitializeMeasurementDevice()
{
    bScannerValid = false;
    gScan.Logger  = []( auto str ) { std::cout << str; };

    if ( API_RefreshScannerControl( gScan ) == false )
    {
        LOG_WARNING( "No DepScan device found" );
        return false;
    }

    LOG_INFO( "Successful connected to DepScan device" );
    if ( gScan.Report( 1000 ) == false )
    {
        CV_LOG_ERROR( nullptr, "Report failed" );
        gScan.Shutdown();
        return false;
    }

    LOG_INFO( "Successfully received report." );
    gScan.StopCapture();
    gScan.SetMotorAcceleration( FLAGS_device_x_accel, FLAGS_device_y_accel );
    gScan.SetMotorDriveClockSpeed( 52400 );
    gScan.ConfigSensorDelay( FLAGS_device_sample_delay );
    gScan.ConfigSensorDistMode( false );
    gScan.InitPointMode();
    gScan.QueuePoint( 0, 0, 0 );

    LOG_INFO( "Successfully initialized point mode." );
    bScannerValid = true;
    return true;
}

bool MeasureSampleDepths(
  std::vector<cv::Point2f> const& Points,
  std::vector<depth_t>*           pDepths,
  std::chrono::milliseconds       DeviceTimeout )
{
    if ( !gScan.IsConnected() || bScannerValid == false )
    {
        if ( InitializeMeasurementDevice() == false )
        {
            return false;
        }
    }

    //
    using namespace std;
    using req_id_t    = size_t;
    using path_node_t = pair<req_id_t, cv::Point2f>;
    vector<path_node_t>              CapturePath;
    chrono::time_point<system_clock> TimeoutPivot;

    // Reserve enough space
    auto const NumSpxl = Points.size();
    auto&      Depths  = *pDepths;
    Depths.resize( NumSpxl );

    // Callback to check timeout
    auto TimeoutChecker
      = [&]() { return system_clock::now() - TimeoutPivot > DeviceTimeout; };

    // Copy points into capture path
    CapturePath.resize( NumSpxl );
    for ( size_t i = 0; i < NumSpxl; i++ )
    {
        CapturePath[i].first  = i;
        CapturePath[i].second = Points[i];
    }

    // Sort capture paths.
    // The capture path array will be sorted with higher X position
    // priority, to reduce the mechanical load of the DepScan device.
    {
        auto CapturePathSortPred
          = []( path_node_t const& a, path_node_t const& b ) -> int {
            auto tolerance = FLAGS_path_x_tolerance * 0.5f;

            auto& pa = a.second;
            auto& pb = b.second;

            return std::abs( pa.x - pb.x ) < tolerance ? pa.y < pb.y
                                                       : pa.x < pb.x;
        };

        sort( CapturePath.begin(), CapturePath.end(), CapturePathSortPred );
    }

    //! @todo Calibrate physical offset between camera and DepScan

    // Configure gScan device
    gScan.OnPointRecv = [&]( FPointData const& pd ) {
        auto Range          = pd.V.Distance / (float)Q9_22_ONE_INT;
        Depths[pd.ID].Range = std::max( 0.f, Range );
        Depths[pd.ID].Amp   = pd.V.AMP / (float)UQ12_4_ONE_INT;
        TimeoutPivot        = system_clock::now();
    };

    auto ElapsedTimeBegin = system_clock::now();

    // Capture all samples through path
    for ( size_t i = 0; i < NumSpxl; i++ )
    {
        // Translate normalized point into angular dimension
        auto const& pt = CapturePath[i];

        // Set image center as 0, 0.
        auto const fov    = FLAGS_vertical_fov;
        auto const aspect = AspectRatio;
        float      x      = pt.second.x / aspect - 0.5f;
        float      y      = pt.second.y - 0.5f;
        auto       xc = x, yc = y;
        x = x * aspect;
        // x = powf( fabs( x ), 1.2f ) * ( x > 0 ? 1 : -1 );
        // y                 = powf( fabs( y ), 0.77f ) * ( y > 0 ? 1 : -1 );
        x *= fov, y *= fov;

        // Queue point capture
        TimeoutPivot = system_clock::now();
        while ( gScan.QueuePointAngular( pt.first, x, y ) == false )
        {
            if ( TimeoutChecker() )
            {
                bScannerValid = false;
                CV_LOG_ERROR( nullptr, "DepScan device timeout occurred !" );
                return false;
            }

            // Yield thread if point queue is full.
            this_thread::sleep_for( 5ms );
        }

        if ( i % 64 == 63 )
        {
            auto now = system_clock::now();
            auto speed
              = duration_cast<milliseconds>( now - ElapsedTimeBegin ).count()
                / i;
            LOG_INFO(
              "%lu/%lu ... %lu ms/smpl %.1f seconds left",
              i,
              NumSpxl,
              speed,
              ( ( NumSpxl - i - 1 ) * speed ) / 1000.0 );
        }
    }

    // Wait until all pending point request finished.
    // Timeout is from latest point request
    while ( gScan.GetPendingPointRequestCount() )
    {
        if ( TimeoutChecker() )
        {
            bScannerValid = false;
            LOG_ERROR( "DepScan device timeout occurred !" );
            return false;
        }
    }

    // Clear callback to prevent local data corruption
    gScan.OnPointRecv = {};
    gScan.QueuePoint( 0, 0, 0 );

    // done.
}
