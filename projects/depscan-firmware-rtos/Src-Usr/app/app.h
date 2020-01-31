#pragma once
#include <FreeRTOS.h>

#include <stdbool.h>
#include <task.h>
#include <uEmbedded/transceiver.h>
#include "../defs.h"
#include "../platform/mem.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//! @addtogroup Depscan
//! @{
//! @defgroup Depscan_API
//! @{
//! @defgroup Depscan_API_HostIO
//! @brief Host I/O Functionalities.
//! @{

//! Send host binary data. It'll automatically append header before transmit.
void API_SendHostBinary( void const* data, size_t len );

//! @brief Send host binary data in a row. 
//! @details 
//!     Length of data array must be larger than cnt.
//!     This function bundles the data and sends it in one packet.
//! @param data: Array of data to transmit
//! @param len: Array of lengths corresponding to the same indexed value of 'data'
//! @param cnt: Number of transmit requests
void API_SendHostBinaries( void const* const data[], size_t const len[], size_t cnt );

//! @brief Send host string data.
void API_SendHostString( void const* data, size_t len );

//! @brief Send host raw data. 
//! @details 
//!     Since it does not append any packet header within the data transfer, you must transmit 
//!      the packet first that contains size information before transferring main data 
void API_SendHostRaw( void const* data, size_t len ); 

//! @}
//! @defgroup Depscan_API_Log
//! @brief Logging functionalities.
//! @{

//! @brief Simply puts string. It is format text version of @ref API_SendHostString
void API_Putf( char const* fmt, ... ); //!< Print text without time header

//! @brief Transmit log string. It'll automatically append time information in front.
//! @return nothing. Don't use.
int  API_Log( char const* txt );

//! Formatted version of @ref API_Log
void API_Logf( char const* fmt, ... ); //!< Print text out to host.
//! @}

//! @defgroup Depscan_API_Exports
//! @details
//!     These functions allow the Host to access memory points exported by the device using the get command.
//!     If this exported memory becomes invalid, you must delete the export to avoid exporting invalid values.
//! @{

//! @brief Export memory location and its size.
//! @param id: ID value must be generated via fnv1a_32.
//!     It'll override existing information if given id is duplicated.
//! @param mem: This must sustain until overwritten or removed.
//! @param len: Memory length.
void API_ExportBin( uint32_t id, void const* mem, size_t len );

//! Remove exported memory information.
//! @param id: @ref API_ExportBin
//! @param ptr: This is used to prevent erasing overwritten export.
void API_RemoveExport( uint32_t id, void const* ptr );

//! @}
//! @defgroup Depscan_API_Timers
//! @{

//! @brief Get time in microseconds
//! @returns total time from launch.
usec_t         API_GetTime_us();

//! @brief Set timer.
//! @param delay: Delay in microseconds.
//! @param obj: Object that delievered into callback when triggering timer.
//! @param cb: Timer callback. Should NOT be null.
//! @returns timer handle to control timer.
timer_handle_t API_SetTimer( usec_t delay, void* obj, void ( *cb )( void* ) );

//! @brief Set timer. ISR safe version.
//! @ref API_SetTimer
timer_handle_t API_SetTimerFromISR( usec_t delay, void* obj, void ( *cb )( void* ) );

//! @brief Abort timer.
//! @param h: Timer handle that was return from @ref API_SetTimer, @ref API_SetTimerFromISR
void           API_AbortTimer( timer_handle_t h );

//! @brief Check timer validity.
//! @param[out] usLeft: Time left to trigger in microseconds. Not substituted when timer handle is not valid.
//! @returns true if given timer handle is valid.
bool           API_CheckTimer( timer_handle_t h, usec_t* usLeft );

//! @}
//! @defgroup Depscan_API_Procedures
//! @warning DO NOT CALL THESE FUNCTIONS
//! @{
_Noreturn void AppProc_HostIO( void* nouse_ );
bool           AppHandler_CaptureCommand( int argc, char* argv[] );
bool           AppHandler_CaptureBinary( char* data, size_t len );
//! @}

//! @}
//! @}
#ifdef __cplusplus
}
#endif // __cplusplus

