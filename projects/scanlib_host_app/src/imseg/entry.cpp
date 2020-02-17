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

/////////////////////////////////////////////////////////////////////////////
// Core loop
int main( int argc, char* argv[] )
{
    cv::ocl::setUseOpenCL( true );
    cv::VideoCapture Video { FLAGS_cam_index };
    cv::Mat          FrameData;
    cv::cuda::GpuMat CudaFrame;
    if ( Video.isOpened() == false )
    {
        CV_LOG_ERROR( nullptr, "Failed to open camera" );
        return 0;
    }

    // Create Superpixel object
    SlicCuda GpuSLIC;
    Video >> FrameData; // Dummy frame to load metadata
    GpuSLIC.initialize( FrameData );

    // Open named window
    cv::namedWindow( "camera" );

    // Initialize superpixel object
    for ( ;; )
    {
        Video >> FrameData;
        if ( ( cv::waitKey( 1 ) & 0xff ) == 'q' )
            break;

        // Transfer it into actual data - UMat
        auto GpuFrame   = FrameData.getUMat( cv::ACCESS_RW );
        auto Superpixel = cv::ximgproc::createSuperpixelSLIC(
          FrameData, cv::ximgproc::MSLIC, 45 );

        cv::UMat Contour;
        auto     TimeBegin = system_clock::now();
        Superpixel->iterate( 1 );
        auto TimeIterDone = system_clock::now();
        Superpixel->getLabelContourMask( Contour );
        auto TimeContourDone = system_clock::now();

        CV_LOG_INFO(
          nullptr,
          ""
            << "--- Number of Superpixels: "
            << Superpixel->getNumberOfSuperpixels()
            << "\nTime consumed to iterate: "
            << duration_cast<milliseconds>( TimeIterDone - TimeBegin ).count()
            << "\n Time Consumed to Extract Contour: "
            << duration_cast<milliseconds>( TimeContourDone - TimeIterDone )
                 .count() );

        cv::imshow( "camera", FrameData );
    }

    CV_LOG_INFO( nullptr, "Shutting down ... " );
    return 0;
}