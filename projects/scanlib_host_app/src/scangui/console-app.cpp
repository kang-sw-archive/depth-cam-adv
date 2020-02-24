#include "console-app.hpp"
#include <scanlib/core/scanner_protocol_handler.hpp>
#include <string>
#include <thread>
#include "app.hpp"
using namespace std::chrono_literals;

void InitConsoleApp()
{
    FScannerProtocolHandler scan;
    scan.Logger = []( auto str ) {
        printf( str );
        if ( str[strlen( str ) - 1] == '\n' )
            printf( ">> " );
    };
    scan.OnFinishScan = []( const FScanImageDesc& args ) {
        printf( ":: SAMPLES RECEVIED [ %6d ] :: \n", args.Height * args.Width );
        for ( size_t i = 0; i < args.Width * args.Height; i++ )
        {
            printf(
              "%20.17f\n", args.CData()[i].Distance / double( Q9_22_ONE_INT ) );
        }
    };
    scan.OnPointRecv = []( const FPointData& data ) {
        printf(
          ":: RECV POINT DATA [ %08x ] :: %20.17f\n",
          data.ID,
          data.V.Distance / double( Q9_22_ONE_INT ) );
    };
    scan.OnReport = []( const FDeviceStat& Stat ) {
        char buf[2048];
        double progress = !Stat.bIsIdle * 100.0 * ( ( Stat.CurMotorStepY - Stat.OfstY ) / (double) ( Stat.SizeY * Stat.StepPerPxlY ) );
        sprintf(
          buf,
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
          " NumMaxPointRq [ %17d      ]\n"
          "\n"
          " MeasureDelay  [ %17d   us ]\n"
          " DistanceMode  [ %17s      ]\n"
          " %-13s [ %17.3f    s ]\n"
          "\n"
          "      :::      %04.1f percent done      ::: \n ",
          'x',
          'y',
          Stat.StepPerPxlX,
          Stat.StepPerPxlY,
          Stat.StepPerPxlX * Stat.DegreePerStepX,
          Stat.StepPerPxlY * Stat.DegreePerStepY,
          Stat.SizeX,
          Stat.SizeY,
          Stat.SizeX * Stat.StepPerPxlX * Stat.DegreePerStepX,
          Stat.SizeY * Stat.StepPerPxlY * Stat.DegreePerStepY,
          Stat.OfstX,
          Stat.OfstY,
          Stat.OfstX * Stat.DegreePerStepX,
          (int)Stat.OfstY * Stat.DegreePerStepY,
          Stat.DegreePerStepX,
          Stat.DegreePerStepY,
          Stat.CurMotorStepX,
          Stat.CurMotorStepY,
          Stat.NumMaxPointRequest,
          Stat.DelayPerCapture,
          Stat.bIsPrecisionMode ? "Near" : "Far",
          Stat.bIsIdle ? "TimeLaunch" : "TimeMeasure",
          Stat.TimeAfterLaunch_us / 1e6,
          progress );

        printf( buf );
    };

    for ( ;; )
    {
        if ( API_RefreshScannerControl( scan ) == false )
        {
            printf( "Waiting for connection ... \n" );
            std::this_thread::sleep_for( 500ms );
            continue;
        }
        std::this_thread::sleep_for( 500ms );

        std::string inp;
        for ( ;; )
        {
            std::getline( std::cin, inp );

            // Evaluate connection lazily to handle disconnection during input.
            if ( scan.IsConnected() == false )
                break;

            if ( inp == "scan" )
            {
                scan.BeginCapture();
            }
            else if ( inp == "report" )
            {
                auto v = scan.Report( 1000 );
                printf( "Report result: %d\n", v );
            }
            else if ( inp == "pt" )
            {
                scan.QueuePointAngular( 0, 0.f, 0.f );
            }
            else
            {
                scan.SendString( inp.c_str() );
            }
            printf( ">> " );
        }

        printf( "Connection lost. Retrying ... \n" );
    }
}
