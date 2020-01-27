#include "app.h"
#include "defs.h"
#include "rw.h"
#include <cmsis_os2.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////
// Host IO communication handler
//
//

extern "C" void AppTask_HostIO( void* nouse_ )
{
    //! @todo. Implement state based command assembler
    //! @todo. Implement token generation
    char                 buf[64];
    size_t               head   = 0;
    transceiver_result_t result = 0;
    for ( ;; ) {
        // Try read from buffer
        result = td_read( gHostConnection, buf + head, sizeof( buf ) - head );

        // If there's nothing to read, sleep for 1 cycle
        if ( result == 0 ) {
            osDelay( 1 );
            continue;
        }
        else if ( result < 0 ) {
            //! @todo. Reset state machine
            osDelay( OS_MS_TO_TICKS( 100u ) );
            continue;
        }

        head += result;

        // @temp. Echo back
        td_write( gHostConnection, buf, head );
        head = 0;
    }
}

extern "C" void AppTask_CmdProc( void* nouse_ )
{
    osThreadExit();
}