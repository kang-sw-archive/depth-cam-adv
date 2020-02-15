/*! \brief Parses protocol
    \file event_handler.hpp
    \author Seungwoo Kang (ki6080@gmail.com)
    \version 0.1
    \date

    \copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
    \details */
#pragma once
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include "../common/protocol.h"

/*! \brief          Handles protocol */
class ICommunicationHandlerBase
{
public:
    //! Initialize stream with given stream buffer.
    void InitializeStream(
      std::unique_ptr<std::streambuf> strm,
      size_t                          RecvBuffSize );

    //! Process single packet.
    //! @returns false if timeout occurred or disconnected.
    enum EPacketProcessResult
    {
        PACKET_OK                   = 0,
        PACKET_ERROR_DISCONNECTED   = -1,
        PACKET_ERROR_TIMEOUT        = -2,
        PACKET_ERROR_INVALID_HEADER = -3,
    } ProcessSinglePacket( size_t TimeoutMs );

    //! Sends binary to device synchronously.
    //! @returns false if stream is not readied yet.
    bool SendBinary( char const* bin, size_t len );

    //! Sends text to device synchronously.
    //! @returns false if stream is not readied yet.
    bool SendString( char const* str );

    //! Shutdown stream
    void ClearConnection() noexcept;

protected:
    //! \brief      Called for incoming string */
    virtual void OnString( char const* str ) { printf( str ); }

    //! \brief      Called for incoming data */
    virtual void OnBinaryData( char const* data, size_t len );

    //! \brief      Error handling. When receive invalid header.
    //! \return     True if correctly handled. false to abort procedure. */
    virtual bool InvalidHeaderException( packetinfo_t const* packet )
    {
        if ( packet )
        {
            printf(
              "Invalid header received. \n"
              "HEX: 0x%08x\n"
              "DEC: 0x%u\n"
              "IS_PACKET? %s || LENGTH %d || IS BIN? %s\n",
              *packet,
              *packet,
              PACKET_IS_PACKET( *packet ) ? "yes" : "no",
              PACKET_LENGTH( *packet ),
              PACKET_IS_STR( *packet ) ? "no" : "yes" );
        }
        else
        {
            printf( "error ... \n" );
        }
        return false;
    };

private:
    //! Holds ostream with given streambuf internally.
    std::unique_ptr<std::ostream>   m_os;
    std::mutex                      m_oslck;
    std::unique_ptr<char[]>         m_buff;
    std::unique_ptr<std::streambuf> m_strmbuf;
    std::mutex                      m_shutdown_lock;
    size_t                          m_buffSize;
};