/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus API.
 * @details		This file provides an interface for the required S2PI module.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_S2PI_H
#define ARGUS_S2PI_H

/*!***************************************************************************
 * @defgroup	argus_s2pi S2PI: Serial Peripheral Interface
 * @ingroup		argus_platform
 * @brief		Argus S2PI: SPI incl. GPIO Hardware Layer Module
 * @details		The S2PI module consists of a standard SPI interface plus a
 *				single GPIO interrupt line. Furthermore, the SPI pins are
 *				accessible via GPIO control to allow a software emulation of
 *				additional protocols using the same pins.
 *
 *				**SPI interface:**
 *
 *				The SPI interface is based on a single function:
 *				#S2PI_TransferFrame. This function transfers a specified number
 *				of bytes via the interfaces MOSI line and simultaneously reads
 *				the incoming data on the MOSI line. The read can also be skipped.
 *				The transfer happen asynchronously, e.g. via a DMA request. After
 *				finishing the transfer, the provided callback is invoked with
 *				the status of the transfer and the provided abstract parameter.
 *				Furthermore, the functions receives a slave parameter that can
 *				be used to connect multiple slaves, each with its individual
 *				chip select line.
 *				The interface also provides functionality to change the SPI
 *				baud rate. An additional abort method is used to cancel the
 *				ongoing transfer.
 *
 *				**GPIO interface:**
 *
 *				The GPIO interface handles the measurement finished interrupt
 *				from the device. Therefore it simply invokes a callback whenever
 *				this interrupt the pending. The #S2PI_SetIrqCallback method is
 *				used to install the callback for a specified slave. Each slave
 *				will have its own interrupt line. An additional callback
 *				parameter can be set that would be passed to the callback function.
 *
 *				In addition to the interrupt, all SPI pins need to be accessible
 *				as GPIO pins through the interface. One basic operation would
 *				be to cycle the chip select pin which resets the device.
 *				Additional, the device contains an EEPROM that is connected to
 *				the SPI pins but requires a different protocol that is not
 *				compatible to any standard SPI interface. Therefore, the
 *				interface provides the possibility to switch to GPIO control
 *				that allows to emulate the EEPROM protocol via software bit
 *				banging. Two methods are provided to switch forth and back
 *				between SPI and GPIO control. In GPIO mode, several functions
 *				are used to read and write the individual GPIO pins.
 *
 *
 * @addtogroup 	argus_s2pi
 * @{
 *****************************************************************************/

#include "api/argus_def.h"

/*!***************************************************************************
 * @brief 	S2PI layer callback function type for the SPI transfer completed event.
 *
 * @param	status : The \link #status_t status\endlink of the completed
 *                   transfer (#STATUS_OK on success).
 * @param	param : The provided (optional, can be null) callback parameter.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
typedef status_t (*s2pi_callback_t)(status_t status, void * param);

/*!***************************************************************************
 * @brief 	S2PI layer callback function type for the GPIO interrupt event.
 *
 * @param	param : The provided (optional, can be null) callback parameter.
 * @return 	-
 *****************************************************************************/
typedef void (*s2pi_irq_callback_t)(void * param);

/*! The S2PI slave identifier. Basically an user defined enumerable type that
 *  can be used to identify the slave within the SPI module. */
typedef int32_t s2pi_slave_t;

/*!	The enumeration of S2PI pins. */
typedef enum S2PI_Pins
{
	/*! The SPI clock pin. */
	S2PI_CLK,

	/*! The SPI chip select pin. */
	S2PI_CS,

	/*! The SPI MOSI pin. */
	S2PI_MOSI,

	/*! The SPI MISO pin. */
	S2PI_MISO,

	/*! The IRQ pin. */
	S2PI_IRQ

} s2pi_pin_t;

/*! Basic return \link #status_t status\endlink for the S2PI drivers. */
enum StatusS2PI
{
	/*! SPI is disabled and pins are used in GPIO mode. */
	STATUS_S2PI_GPIO_MODE = 51,

	/*! SPI pins disabled, i.e. all pins are pulled to low.
	 *  No SPI transfers going on. */
	STATUS_S2PI_DISABLED = 52,

	/*! Error occurred on the Rx line. */
	ERROR_S2PI_RX_ERROR = -51,

	/*! Error occurred on the Tx line. */
	ERROR_S2PI_TX_ERROR = -52,

	/*! Called a function at a wrong driver state. */
	ERROR_S2PI_INVALID_STATE = -53,

	/*! The specified baud rate is not valid. */
	ERROR_S2PI_INVALID_BAUD_RATE = -54,

	/*! The specified slave identifier is not valid. */
	ERROR_S2PI_INVALID_SLAVE = -55,

};

/*!***************************************************************************
 * @brief	Gets the current SPI baud rate in bps.
 * @return 	Returns the current baud rate.
 *****************************************************************************/
uint32_t S2PI_GetBaudRate(void);

/*!***************************************************************************
 * @brief	Sets the SPI baud rate in bps.
 * @param	baudRate_Bps : The default SPI baud rate in bauds-per-second.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 * 			- #STATUS_OK on success
 * 			- #ERROR_S2PI_INVALID_BAUD_RATE on invalid baud rate value.
 *****************************************************************************/
status_t S2PI_SetBaudRate(uint32_t baudRate_Bps);

/*!***************************************************************************
 * @brief 	Returns the status of the SPI module.
 *
 * @return 	Returns the \link #status_t status\endlink:
 * 			 - #STATUS_IDLE: No SPI transfer or GPIO access is ongoing.
 *         	 - #STATUS_BUSY: An SPI transfer is in progress.
 *         	 - #STATUS_S2PI_GPIO_MODE: The module is in GPIO mode.
 *****************************************************************************/
status_t S2PI_GetStatus(void);

/*!***************************************************************************
 * @brief 	Transfers a single SPI frame asynchronously.
 * @details Transfers a single SPI frame in asynchronous manner. The Tx data
 * 			buffer is written to the device via the MOSI line.
 * 			Optionally the data on the MISO line is written to the provided
 * 			Rx data buffer. If null, the read data is dismissed.
 * 			The transfer of a single frame requires to not toggle the chip
 * 			select line to high in between the data frame.
 * 			An optional callback is invoked when the asynchronous transfer
 * 			is finished. Note that the provided buffer must not change while
 * 			the transfer is ongoing. Use the slave parameter to determine
 * 			the corresponding slave via the given chip select line.
 *
 * @param	slave : The specified S2PI slave.
 * @param 	txData : The 8-bit values to write to the SPI bus MOSI line.
 * @param 	rxData : The 8-bit values received from the SPI bus MISO line
 *                   (pass a null pointer if the data don't need to be read).
 * @param 	frameSize : The number of 8-bit values to be sent/received.
 * @param	callback : A callback function to be invoked when the transfer is
 * 					   finished. Pass a null pointer if no callback is required.
 * @param	callbackData : A pointer to a state that will be passed to the
 *                         callback. Pass a null pointer if not used.
 *
 * @return 	Returns the \link #status_t status\endlink:
 * 			 - #STATUS_OK: Successfully invoked the transfer.
 *         	 - #ERROR_INVALID_ARGUMENT: An invalid parameter has been passed.
 *         	 - #ERROR_S2PI_INVALID_SLAVE: A wrong slave identifier is provided.
 *         	 - #STATUS_BUSY: An SPI transfer is already in progress. The
 *         	                 transfer was not started.
 *         	 - #STATUS_S2PI_GPIO_MODE: The module is in GPIO mode. The transfer
 *         	                           was not started.
 *****************************************************************************/
status_t S2PI_TransferFrame(s2pi_slave_t slave,
							uint8_t const * txData,
							uint8_t * rxData,
							size_t frameSize,
							s2pi_callback_t callback,
							void * callbackData);

/*!***************************************************************************
 * @brief	Terminates a currently ongoing asynchronous SPI transfer.
 * @details	When a callback is set for the current ongoing activity, it is
 * 			invoked with the #ERROR_ABORTED error byte.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t S2PI_Abort(void);

/*!***************************************************************************
 * @brief 	Set a callback for the GPIO IRQ for a specified S2PI slave.
 *
 * @param	slave : The specified S2PI slave.
 * @param	callback : A callback function to be invoked when the specified
 * 					   S2PI slave IRQ occurs. Pass a null pointer to disable
 * 					   the callback.
 * @param	callbackData : A pointer to a state that will be passed to the
 *                         callback. Pass a null pointer if not used.
 *
 * @return 	Returns the \link #status_t status\endlink:
 * 			 - #STATUS_OK: Successfully installation of the callback.
 *         	 - #ERROR_S2PI_INVALID_SLAVE: A wrong slave identifier is provided.
 *****************************************************************************/
status_t S2PI_SetIrqCallback(s2pi_slave_t slave,
							 s2pi_irq_callback_t callback,
							 void * callbackData);

/*!***************************************************************************
 * @brief	Cycles the chip select line.
 * @details In order to cancel the integration on the ADS0032, a fast toggling
 * 			of the chip select pin of the corresponding SPI slave is required.
 * 			Therefore, this function toggles the CS from high to low and back.
 * 			The SPI instance for the specified S2PI slave must be idle,
 * 			otherwise the status #STATUS_BUSY is returned.
 * @param	slave : The specified S2PI slave.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t S2PI_CycleCsPin(s2pi_slave_t slave);

/*!*****************************************************************************
 * @brief	Captures the S2PI pins for GPIO usage.
 * @details	The SPI is disabled (module status: #STATUS_S2PI_GPIO_MODE) and the
 * 			pins are configured for GPIO operation. The GPIO control must be
 * 			release with the #S2PI_ReleaseGpioControl function in order to
 * 			switch back to ordinary SPI functionality.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t S2PI_CaptureGpioControl(void);

/*!*****************************************************************************
 * @brief	Releases the S2PI pins from GPIO usage and switches back to SPI mode.
 * @details	The GPIO pins are configured for SPI operation and the GPIO mode is
 * 			left. Must be called if the pins are captured for GPIO operation via
 * 			the #S2PI_CaptureGpioControl function.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t S2PI_ReleaseGpioControl(void);

/*!*****************************************************************************
 * @brief	Writes the output for a specified SPI pin in GPIO mode.
 * @details This function writes the value of an SPI pin if the SPI pins are
 * 			captured for GPIO operation via the #S2PI_CaptureGpioControl previously.
 * @param	slave : The specified S2PI slave.
 * @param	pin : The specified S2PI pin.
 * @param	value : The GPIO pin state to write (0 = low, 1 = high).
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t S2PI_WriteGpioPin(s2pi_slave_t slave, s2pi_pin_t pin, uint32_t value);

/*!*****************************************************************************
 * @brief	Reads the input from a specified SPI pin in GPIO mode.
 * @details This function reads the value of an SPI pin if the SPI pins are
 * 			captured for GPIO operation via the #S2PI_CaptureGpioControl previously.
 * @param	slave : The specified S2PI slave.
 * @param	pin : The specified S2PI pin.
 * @param	value : The GPIO pin state to read (0 = low, 1 = high).
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t S2PI_ReadGpioPin(s2pi_slave_t slave, s2pi_pin_t pin, uint32_t * value);

/*! @} */
#endif // ARGUS_S2PI_H
