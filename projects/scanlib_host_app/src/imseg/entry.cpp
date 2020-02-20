//! @file       entry.cpp
//! @brief      File brief description
//!
//! @author     Seungwoo Kang (ki6080@gmail.com)
//! @copyright  Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
//!             File detailed description
//! @todo       Find center of each super pixels
//! @todo       Calibrate motor and camera centor position
//! @todo
#include <chrono>
#include <gflags/gflags.h>
#include <iostream>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc/lsc.hpp>
#include <opencv2/ximgproc/seeds.hpp>
#include <opencv2/ximgproc/slic.hpp>

#include <SlicCudaHost.h>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

/////////////////////////////////////////////////////////////////////////////
// GLOBAL FLAGS
DEFINE_int32( cam_index, 1, "Specify camera index to use" );
DEFINE_int32( desired_pixel_cnt, int( 2e5 ), "Number of intended pixels" );
DEFINE_double( slic_compactness, 100.f, "Number of intended pixels" );

/////////////////////////////////////////////////////////////////////////////
// Static method decls
static void FindCenters( cv::Mat Contour, std::vector<cv::Point2f>& Out );

/////////////////////////////////////////////////////////////////////////////
// Core lop
int main( int argc, char* argv[] )
{
    cv::ocl::setUseOpenCL( true );
    cv::VideoCapture         Video { FLAGS_cam_index };
    cv::Mat                  FrameData;
    cv::cuda::GpuMat         CudaFrame;
    std::vector<cv::Point2f> Centers;

    if ( Video.isOpened() == false )
    {
        CV_LOG_ERROR( nullptr, "Failed to open camera" );
        return 0;
    }

    // Configure video resolution ... Set pixel count as configuration
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
        CV_LOG_INFO( nullptr, "Video resolution is set to " << w << " " << h );
    }
    // Create Superpixel object
    SlicCuda GpuSLIC;
    Video >> FrameData; // Dummy frame to load metadata
    GpuSLIC.initialize( FrameData, 250, SlicCuda::SLIC_NSPX, FLAGS_slic_compactness );

    // Open named window
    cv::namedWindow( "camera" );

    // Initialize superpixel object
    for ( ;; )
    {
        Video >> FrameData;

        if ( cv::waitKey( 1 ) != -1 )
            break;

        // Scale image for optimization
        // cv::resize( FrameData, FrameData, DesiredImageSize );

        // Transfer it into actual data - UMat
        auto GpuFrame  = FrameData.getUMat( cv::ACCESS_RW );
        auto TimeBegin = system_clock::now();

        // Apply SLIC algorithms
        GpuSLIC.segment( FrameData );
        GpuSLIC.enforceConnectivity();
        auto TimeIterDone = system_clock::now();

        CV_LOG_INFO(
          nullptr,
          "Time consumed to iterate: "
            << duration_cast<milliseconds>( TimeIterDone - TimeBegin )
                 .count() );

        cv::Mat Contour = GpuSLIC.getLabels();
#if 1 // For debugging purpose ...
        {
            cv::UMat GpuContour = Contour.getUMat( cv::ACCESS_RW );
            cv::Laplacian( GpuContour, GpuContour, -1 );
            cv::compare( GpuContour, 0, GpuContour, cv::CMP_NE );
            cv::cvtColor( GpuContour, GpuContour, cv::COLOR_GRAY2BGR );
            GpuContour.convertTo( GpuContour, CV_8UC3 );
            cv::add( GpuContour, GpuFrame, FrameData );
        }
#endif
        auto TimeContourDone = system_clock::now();
        CV_LOG_INFO(
          nullptr,
          "Time Consumed to Extract Contour: "
            << duration_cast<milliseconds>( TimeContourDone - TimeIterDone )
                 .count() );

        // Find center of each super pixels
        FindCenters( Contour.clone(), Centers );
        auto TimeCenterLookupDone = system_clock::now();

        //

        cv::imshow( "camera", FrameData );
    }

    CV_LOG_INFO( nullptr, "Shutting down ... " );
    return 0;
}

static void FindCenters( cv::Mat Contour, std::vector<cv::Point2f>& Out )
{
    Out.clear();
    auto NumRows = Contour.rows;
    auto NumCols = Contour.cols;
    {
        double Max;
        cv::minMaxLoc( Contour, nullptr, &Max );
        Out.resize( static_cast<size_t>( Max + 0.5 ) );
        CV_LOG_INFO( nullptr, "Number of labels is: " << Out.size() );
    }
    Contour.convertTo( Contour, CV_32S );
    CV_Assert( Contour.channels() == 1 );

    // For each rows ...
    for ( decltype( NumRows ) i = 0; i < NumRows; i++ )
    {
        auto RowPtr = Contour.ptr<int>( i );
    }
}
