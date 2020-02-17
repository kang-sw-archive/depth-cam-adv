#include <gflags/gflags.h>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

DEFINE_int32( cam_index, 1, "Specify camera index to use" );

int main( int argc, char* argv[] )
{
    cv::VideoCapture Video { FLAGS_cam_index };
    cv::Mat          FrameData;

    if ( Video.isOpened() == false )
    {
        CV_LOG_ERROR( nullptr, "Failed to open camera %d", FLAGS_cam_index );
        return 0;
    }

    // Open named window
    cv::namedWindow( "camera" );

    // Initialize superpixel object

    for ( ;; )
    {
        Video >> FrameData;
        cv::imshow( "camera", FrameData );
        if ( ( cv::waitKey( 33 ) & 0xff ) == 'q' )
            break;

        // Transfer it into actual data - UMat
        auto Data = FrameData.getUMat(cv::ACCESS_RW);


    }

    CV_LOG_INFO( nullptr, "Shutting down ... " );
    return 0;
}