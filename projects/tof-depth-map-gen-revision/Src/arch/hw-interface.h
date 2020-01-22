/*! \brief Several hardware associated interface functionalities.
    \file interface.h
    \author Seungwoo Kang (ki6080@gmail.com)
    \version 0.1
    \date 2019-09-29
    
    \copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

/*! \brief      Checks whether usb connection is present.
    \return     'true' if usb connection is present. */
static inline bool IsUsbActive()
{
    //! @todo.
    // Temporarily, this function returns true always.
    return true;
}

typedef enum {
    PROXIMITY_SENSOR_X,
    PROXIMITY_SENSOR_Y,
} proximity_sensor_t;

/*! \brief      Checks whether proximity sensor is close.
    \param sensorIndex      Index of proximity sensor. 
    \return */
bool IsProximitySensorActive(proximity_sensor_t SensorIndex);
