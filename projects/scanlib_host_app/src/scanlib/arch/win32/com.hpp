/*! \brief
    \file
    \author Seungwoo Kang (ki6080@gmail.com)
    \version 0.1
    \date
    
    \copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
    \details 
*/
#pragma once
#include <iostream>

/*! \breif      Forward declaration for Windows structure. */
struct _DCB;
struct _COMMTIMEOUTS;

/*! /brief      A class that implements simple read/write functionality for com device */
class comstreambuf_t : public std::streambuf {
public:
    using strmbuf_t = std::streambuf;

public:
    /*! \breif      Open com port via com number.
        \param com_port     Port name. like COMx */
    comstreambuf_t( char const* comPort );

    /*! \breif      Destructor for safe closing handle.
        \return */
    ~comstreambuf_t();

    /*! \breif      Check if this com stream is valid.
        \return */
    operator bool() const;

    /*! \breif      Reports current COM status
        \return */
    bool get_dcb( _DCB* dcbOut ) const;

    /*! \breif      Sets current COM status
        \return */
    bool set_dcb( _DCB* dcbIn );

    /*! \breif      Sets COM timeout */
    void set_timeout( _COMMTIMEOUTS* timeout );

protected:
    strmbuf_t::int_type overflow( strmbuf_t::int_type c ) override;
    int                 sync() override;

    strmbuf_t::int_type underflow() override;

    virtual std::streamsize xsputn( const char* _Ptr, std::streamsize _Count ) override;
    virtual std::streamsize xsgetn( char* _Ptr, std::streamsize _Count ) override;

private:
    // Handle of COM port object. Expressed as void ptr to reduce dependency on windows header.
    void* m_hCom;

    // Input buffer
    char ibuf[1];
};
