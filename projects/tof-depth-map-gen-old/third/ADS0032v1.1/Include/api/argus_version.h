/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		This file contains current Argus API version number.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_VERSION_H
#define ARGUS_VERSION_H

/*!***************************************************************************
 * @defgroup	version API Version
 * @ingroup		argusapi
 * @brief		Argus API Code Version.
 * @details		Provides a version number for the Argus Firmware.
 * @addtogroup 	version
 * @{
 *****************************************************************************/

/*! Major version number of the Argus API. */
#define ARGUS_API_VERSION_MAJOR	0

/*! Minor version number of the Argus API. */
#define ARGUS_API_VERSION_MINOR	9

/*! Bugfix version number of the Argus API. */
#define ARGUS_API_VERSION_BUGFIX 5


/*! Construct the version number for drivers. */
#define MAKE_VERSION(major, minor, bugfix) \
	(((major) << 24) | ((minor) << 16) | (bugfix))

/*! Version number of the API. */
#define ARGUS_API_VERSION MAKE_VERSION((ARGUS_API_VERSION_MAJOR), \
									   (ARGUS_API_VERSION_MINOR), \
									   (ARGUS_API_VERSION_BUGFIX))

/*! @} */
#endif /* ARGUS_VERSION_H */
