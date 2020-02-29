#ifdef _WIN32
#    include "../utility.hpp"
// #pragma comment( linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup" )

#    include <Windows.h>

#    include <SetupAPI.h>
#    include <initguid.h>
#    include <scanlib/arch/win32/com.hpp>
#    include <winusb.h>
#    pragma comment( lib, "setupapi.lib" )

bool API_RefreshScannerControl( FScannerProtocolHandler& S )
{
    char Port[20];
    memset( Port, 0, sizeof Port );
    API_FindConnection( Port );

    if ( strlen( Port ) == 0 )
    {
        return false;
    }

    API_SystemCreateScannerControl( S, Port );
    return true;
}

bool API_MappToRGB(
  intptr_t                     context,
  std::function<void( void* )> draw_cb,
  void*                        buffer )
{
    using namespace std;
    // This implementation requires buffer.
    if ( buffer == nullptr )
    {
        return false;
    }

    auto hDC = (HDC)context;
    if ( hDC == NULL )
    {
        return false;
    }

    // Open bitmap
    HBITMAP    hBitmap = ( HBITMAP )::GetCurrentObject( hDC, OBJ_BITMAP );
    BITMAPINFO bmp;
    bmp.bmiHeader.biSize     = sizeof( bmp );
    bmp.bmiHeader.biBitCount = 0;
    GetDIBits( hDC, hBitmap, 0, 0, NULL, &bmp, DIB_RGB_COLORS );
    auto sz = bmp.bmiHeader.biWidth * bmp.bmiHeader.biHeight;

    draw_cb( buffer );
    ::SetBitmapBits( hBitmap, 4 * sz, buffer );

    return true;
}

using namespace std;
void API_SystemShowConsole( bool bShow )
{
    ::ShowWindow( ::GetConsoleWindow(), bShow ? SW_SHOW : SW_HIDE );
}

static void SearchCOM( TCHAR* pszComePort, TCHAR* vid, TCHAR* pid );

void API_SystemCreateScannerControl(
  FScannerProtocolHandler& S,
  char const*              COMSTR )
{
    using namespace std;
    //! Initialize based on com port
    S.Shutdown();

    auto ComOpener
      = [COMSTR]( FScannerProtocolHandler& s ) -> unique_ptr<comstreambuf_t> {
        s.ClearConnection();
        auto         ret = make_unique<comstreambuf_t>( COMSTR );
        COMMTIMEOUTS to  = { MAXDWORD, 1, 1, 1, 1 };
        ret->set_timeout( &to );

        if ( *ret == false )
            return nullptr;
        return move( ret );
    };

    FCommunicationProcedureInitStruct init = {};
    init.ConnectionRetryCount              = 50;
    init.ConnectionRetryIntervalMs         = 500;
    init.TimeoutMs                         = 1000;

    S.Activate( ComOpener, init );
}

struct serialportinfo
{
    bool   isUSBdev;
    string devpath;
    string friendlyname;
    string description;
    string portname;
};

static int SearchCOM2()
{
    GUID*    guidDev  = (GUID*)&GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR;
    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DETAIL_DATA* pDetData = NULL;
    hDevInfo                                  = SetupDiGetClassDevs(
      guidDev, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
    string           errorstring;
    constexpr size_t DEVICE_INFO_SZ = 256;

    if ( hDevInfo == INVALID_HANDLE_VALUE )
    {
        printf( TEXT( "SetupDiGetClassDevs return INVALID_HANDLE_VALUE" ) );
        return -1;
    }

    BOOL                     bOk = TRUE;
    SP_DEVICE_INTERFACE_DATA ifcData;
    DWORD                    dwDetDataSize
      = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA ) + DEVICE_INFO_SZ;
    pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)new char[dwDetDataSize];
    if ( pDetData == NULL )
    {
        return -1;
    }

    ifcData.cbSize   = sizeof( SP_DEVICE_INTERFACE_DATA );
    pDetData->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );
    for ( int cnt = 0; cnt < 256; cnt++ )
    {
        bOk = SetupDiEnumDeviceInterfaces(
          hDevInfo, NULL, guidDev, cnt, &ifcData );
        if ( bOk == TRUE )
        {
            // Got a device.Get the details.
            SP_DEVINFO_DATA devdata = { sizeof( SP_DEVINFO_DATA ) };
            bOk                     = SetupDiGetDeviceInterfaceDetail(
              hDevInfo, &ifcData, pDetData, dwDetDataSize, NULL, &devdata );
            if ( bOk == TRUE )
            {
                string  strDevPath = pDetData->DevicePath;
                wchar_t fname[256] = { 0 };
                wchar_t desc[256]  = { 0 };
                BOOL    bSuccess   = SetupDiGetDeviceRegistryProperty(
                  hDevInfo,
                  &devdata,
                  SPDRP_FRIENDLYNAME,
                  NULL,
                  (PBYTE)fname,
                  256,
                  NULL );
                bSuccess = bSuccess
                           && SetupDiGetDeviceRegistryProperty(
                             hDevInfo,
                             &devdata,
                             SPDRP_DEVICEDESC,
                             NULL,
                             (PBYTE)desc,
                             256,
                             NULL );
                bool bUsbDevice = false;
                if (
                  ( wcsstr( desc, L"COM" ) != NULL )
                  || ( wcsstr( fname, L"COM" ) != NULL ) )
                {
                    bUsbDevice = true;
                }
                if ( bSuccess == TRUE )
                {
#    ifdef _DEBUG
                    wprintf(
                      L"%S[%03d] %S (%S)\n",
                      L"COM Port",
                      cnt,
                      (wchar_t*)fname,
                      (wchar_t*)desc );
#    endif // DEBUG
                    serialportinfo si;
                    si.isUSBdev = bUsbDevice;
                    si.devpath  = strDevPath.c_str();
                }
            }
            else
            {
                errorstring = TEXT( "SetupDiGetDeviceInterfaceDetail failed." );
                throw errorstring;
            }
        }
        else
        {
            DWORD err = GetLastError();
            if ( err != ERROR_NO_MORE_ITEMS )
            {
                errorstring = TEXT( "SetupDiEnumDeviceInterfaces failed." );
                throw errorstring;
            }
        }
    }

    if ( pDetData != NULL )
        delete[]( char* ) pDetData;
    if ( hDevInfo != INVALID_HANDLE_VALUE )
        SetupDiDestroyDeviceInfoList( hDevInfo );

    return 0;
    // from https://rageworx.pe.kr/1589 [자유로운 그날을 위해]
}

static void SearchCOM( TCHAR* pszComePort, TCHAR* vid, TCHAR* pid )
{
    HDEVINFO        DeviceInfoSet;
    DWORD           DeviceIndex = 0;
    SP_DEVINFO_DATA DeviceInfoData;
    PCSTR           DevEnum              = "USB";
    CHAR            ExpectedDeviceId[80] = { 0 }; // Store hardware id
    size_t          DevIdLen;
    BYTE            szBuffer[1024] = { 0 };
    DEVPROPTYPE     ulPropertyType;
    DWORD           dwSize = 0;
    DWORD           Error  = 0;
    enum
    {
        BUFF_LEN = 20
    };

    // create device hardware id
    strcpy_s( ExpectedDeviceId, "vid_" );
    strcat_s( ExpectedDeviceId, vid );
    strcat_s( ExpectedDeviceId, "&pid_" );
    strcat_s( ExpectedDeviceId, pid );
    DevIdLen = strlen( ExpectedDeviceId );

    // SetupDiGetClassDevs returns a handle to a device information set
    DeviceInfoSet = SetupDiGetClassDevs(
      NULL, DevEnum, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT );

    if ( DeviceInfoSet == INVALID_HANDLE_VALUE )
        return;

    // Fills a block of memory with zeros
    ZeroMemory( &DeviceInfoData, sizeof( SP_DEVINFO_DATA ) );
    DeviceInfoData.cbSize = sizeof( SP_DEVINFO_DATA );

    // Receive information about an enumerated device
    while (
      SetupDiEnumDeviceInfo( DeviceInfoSet, DeviceIndex, &DeviceInfoData ) )
    {
        DeviceIndex++;

        // Retrieves a specified Plug and Play device property
        if ( SetupDiGetDeviceRegistryProperty(
               DeviceInfoSet,
               &DeviceInfoData,
               SPDRP_HARDWAREID,
               &ulPropertyType,
               (BYTE*)szBuffer,
               sizeof( szBuffer ), // The size, in bytes
               &dwSize ) )
        {
            // Compare if VID&PID equals
            // To compare as case-insensitive
            for ( char* head = (char*)szBuffer; *head; ++head )
            {
                *head = tolower( *head );
            }
            // Compare ...
            bool bFound = false;
            for ( size_t i = 0, length = strlen( (char*)szBuffer ) - DevIdLen;
                  i < length;
                  i++ )
            {
                char* bf = (char*)szBuffer + i;
                if ( strncmp( bf, ExpectedDeviceId, DevIdLen ) == 0 )
                {
                    bFound = true;
                    break;
                }
            }

            if ( bFound == false )
                continue;

            HKEY hDeviceRegistryKey;

            // Get the key
            hDeviceRegistryKey = SetupDiOpenDevRegKey(
              DeviceInfoSet,
              &DeviceInfoData,
              DICS_FLAG_GLOBAL,
              0,
              DIREG_DEV,
              KEY_READ );
            if ( hDeviceRegistryKey == INVALID_HANDLE_VALUE )
            {
                Error = GetLastError();

                break; // Not able to open registry
            }
            else
            {
                // Read in the name of the port
                char  pszPortName[BUFF_LEN];
                DWORD dwSize = sizeof( pszPortName );
                DWORD dwType = 0;

                if (
                  ( RegQueryValueEx(
                      hDeviceRegistryKey,
                      "PortName",
                      NULL,
                      &dwType,
                      (LPBYTE)pszPortName,
                      &dwSize )
                    == ERROR_SUCCESS )
                  && ( dwType == REG_SZ ) )
                {
                    // Check if it really is a com port
                    if ( strncmp( pszPortName, "COM", 3 ) == 0 )
                    {
                        int nPortNr = atoi( pszPortName + 3 );
                        if ( nPortNr != 0 )
                        {
                            strcpy_s( pszComePort, BUFF_LEN, pszPortName );
                        }
                    }
                }

                // Close the key now that we are finished with it
                RegCloseKey( hDeviceRegistryKey );
                break;
            }
        }
    }

    if ( DeviceInfoSet )
    {
        SetupDiDestroyDeviceInfoList( DeviceInfoSet );
    }

    // from [https://aticleworld.com/get-com-port-of-usb-serial-device/]
}

void API_FindConnection( char* PortName )
{
    SearchCOM( PortName, SICO_VENDOR_ID, SICO_SCANNER_PRODUCT_ID );
}

static int SearchCOM3()
{
    HDEVINFO hDevInfo = NULL; //하드웨어 정보 핸들
    SP_DEVINFO_DATA
    DeviceInfoData; //디바이스 정보 구조체? 안 낭 핳 엏 긍응 엏난
    DWORD i = 0;
    string strBuffer; //하드웨어 레지스트리에서의 이름을 복사할 스트링
    int nFindStart = 0; //검색한 문자열의 시작위치

    // Create a HDEVINFO with all present devices.
    hDevInfo = SetupDiGetClassDevs(
      NULL,
      0, // Enumerator
      0,
      DIGCF_PRESENT | DIGCF_ALLCLASSES ); //여기를 수정함으로 검색할 장치 변경이
                                          //가능

    if ( hDevInfo == INVALID_HANDLE_VALUE )
    {
        // Insert error handling here.
        return -1;
    }

    // Enumerate through all devices in Set.

    DeviceInfoData.cbSize = sizeof( SP_DEVINFO_DATA );
    for ( i = 0; SetupDiEnumDeviceInfo( hDevInfo, i, &DeviceInfoData ); i++ )
    {
        DWORD  DataT;
        LPTSTR buffer     = NULL;
        DWORD  buffersize = 0;
        int    i          = SPDRP_DEVICEDESC;

        //
        // Call function with null to begin with,
        // then use the returned buffer size (doubled)
        // to Alloc the buffer. Keep calling until
        // success or an unknown failure.
        //
        //  Double the returned buffersize to correct
        //  for underlying legacy CM functions that
        //  return an incorrect buffersize value on
        //  DBCS/MBCS systems.
        //
        /// for(i=SPDRP_DEVICEDESC;i<SPDRP_MAXIMUM_PROPERTY;i++)
        ///   {
        while ( !SetupDiGetDeviceRegistryProperty(
          hDevInfo,
          &DeviceInfoData,
          SPDRP_FRIENDLYNAME,
          &DataT,
          (PBYTE)buffer,
          buffersize,
          &buffersize ) )
        {
            if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER )
            {
                // Change the buffer size.
                if ( buffer )
                    LocalFree( buffer );
                // Double the size to avoid problems on
                // W2k MBCS systems per KB 888609.
                buffer = (char*)LocalAlloc( LPTR, buffersize * 2 );
            }
            else
            {
                // Insert error handling here.
                break;
            }
        } // while

        if ( !buffer )
            continue;

        // printf( "%s\n", buffer ); // Print all searched devices.
        strBuffer = buffer;

        // Search for 'COM' in the string.
        if ( strBuffer.find( "COM" ) == string::npos )
            continue;

        DWORD nSize = 0;
        SetupDiGetDeviceInstanceId(
          hDevInfo, &DeviceInfoData, buffer, sizeof( buffer ), &nSize );
        buffer[nSize] = '\0';
        printf( "%s ... %d\n", buffer, nSize );
        return false;
    }

    if ( GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS )
    {
        // Insert error handling here.
        return -1;
    }

    //  Cleanup
    SetupDiDestroyDeviceInfoList( hDevInfo );
    return 0;
}

#endif
