#ifndef RAYCOMPOSER_DEVICE_API_H
#define RAYCOMPOSER_DEVICE_API_H

/** \file rcdev.h
  *  \brief RayComposer public device API
  *  \version 1.06
  *
  *  This file contains the API definition for the public
  *  RayComposer devices API.
  * \mainpage RayComposer public device API
  *
  * \section Introduction
  *
  * This API (Application Programming Interface) was designed to allow low-level
  * hardware access to the RayComposer ILDA DAC devices (RayComposer USB and RayComposer NET)
  * from your custom (or third-party) application software. The API functions only use simple
  * data types to enable compatibility with a large number of programming languages.
  *
  * Have fun and be creative!
  *
  *  <a href="http://www.raycomposer.de">Copyright &copy;2015-2017 RayComposer - R. Adams</a>.
  *
  * \section Code Example
  * Quick Example in plain C. This code enumerates all devices and displays a list.
  * If a device is found the device will be opened, started and 1000 frames containing
  * a circle will be display. Then the device will be stoped, closed and the program
  * will exit.
  * \include "demo.c"

  * \section License
  * Permission is granted to use this API, the provided samples and documentation free of
  * charge als long as they are used for controling RayComposer devices.
  *
  * Last Update: 03.08.2017
  * - XY output signals were inverted wrt the specification; fixed since version 1.05
  * - input string and data pointers declared as "const" now
  * - check for mingw32 in windows as well
  */

#ifdef __cplusplus
extern "C" {
#endif

/** API Version */
#define RCAPI_VERSION 0x0106

#if defined(_MSC_VER) || defined(__MINGW32__)
  /** Calling Convention of function pointers */
  #define RCCALL __stdcall
  //  Microsoft
  #ifdef RCAPI_BUILD
    /** Calling Convention / visibility of API functions */
    #define RCAPI __declspec(dllexport) __stdcall
  #else
    /** Calling Convention / visibility of API functions */
    #define RCAPI __declspec(dllimport) __stdcall
  #endif
#else
  /** Calling Convention of function pointers */
  #define RCCALL
  #if __GNUC__ >= 4
    /** Calling Convention / visibility of API functions */
    #define RCAPI __attribute__ ((visibility ("default")))
  #else
    /** Calling Convention / visibility of API functions */
    #define RCAPI
  #endif
#endif

/** \brief Error Codes
 *
 * Error codes returned by RayComposer Device API functions.
 */

enum RCReturnCode {
  /** Success */
  RCOk = 0,
  /** Error: the API has not been initialised */
  RCErrorNotInitialised = -1,
  /** Error: devices have not been enumerated */
  RCErrorNotEnumerated = -2,
  /** Error: the device handle is invalid */
  RCErrorInvalidHandle = -3,
  /** Error: output was not started */
  RCErrorNotStarted =-4,
  /** Error: communication with the device failed */
  RCErrorIO = -5,
  /** Error: function parameter is out of range */
  RCErrorParameterOutOfRange = -6,
  /** Error: function parameter is invalid */
  RCErrorParameterInvalid = -7,
};

/** \brief Universe Direction
 *
 * Tells wheter a universe is an input or output.
 */

enum RCUniverseDirection {
  /** Output Universe */
  RCOutput = 0,
  /** Input Universe */
  RCInput = 1
};

/**
 * @brief Vector Data Structure
 *
 * This data structure is used to pass point data to
 * the RayComposer Interfaces
 *
 */

struct RCPoint {
  /** X (horizontal) position signal; range -32768 to 32767 results in -10V to +10V on the ILDA connector */
  signed short x;
  /** Y (vertical) position signal; range -32768 to 32767 results in -10V to +10V on the ILDA connector */
  signed short y;
  /** Red color signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
  unsigned short red;
  /** Green color signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
  unsigned short green;
  /** Blue color signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
  unsigned short blue;
  /** Intensity signal, range 0 to 65535 results in 0V to +5V on the ILDA connector */
  unsigned short intensity;
  /** User 1 (Cyan) signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
  unsigned short user1;
  /** User 2 (Magenta) signal; range 0 to 65535 results in 0V to +5V on the ILDA connector */
  unsigned short user2;
};

/** @} */

/** \defgroup init Initialisation
 *
 * \brief Initialise API functions
 *
 *  @{
 *
 */

/** \brief Initialise DLL
  *
  * This function must be called before all other API calls can be used.
  *
  * \return Returns the DLL API Version on success. Currently this is 0x0106.
  */
int RCAPI RCInit();
/** Function type definition for RCInit() */
typedef int (RCCALL *TRCInit)();

/** \brief De-Initialise DLL
  *
  * This function must be before the program exits to de-initialise the DLL.
  *
  */
int RCAPI RCExit();
/** Function type definition for RCExit() */
typedef int (RCCALL *TRCExit)();

/** @} */

/** \defgroup enum Device Enumeration
 * \brief Detect devices and query their IDs.
 *  @{
 *
 */

/** \brief Find devices
  *
  * Looks for RayComposer devices and update the internal device list.
  * The number of devices in the device list is returned.
  * Use RCDeviceID() afterwards to query the device list.
  *
  * For RayComposer NET devices please allow a small amount of time to
  * pass after RCInit() and before calling RCEnumerateDevices(). Usually
  * a delay of 10ms is sufficient for devices to be discovered on the network.
  *
  * \return The number of devices found. If an error occured, a negative
  * value indicating one of the RCReturnCode error codes is returned.
  *
  */

int RCAPI RCEnumerateDevices();
/** Function type definition for RCEnumerateDevices() */
typedef int (RCCALL *TRCEnumerateDevices)();

/** \brief Read Device ID
 *
 * Read the device ID string for the device list entry specified by index.
 * RCEnumerateDevices() must have been called before this function can be
 * used.
 *
 * \param index The device list index. Range is 0 to the number reported
 * by RCEnumerateDevices() - 1.
 * \param deviceId Pointer to a buffer where the device ID will be stored.
 * The Device ID is an UTF-8 encoded printable string like "RayComposer USB 1234"
 * \param maxLength Size of buffer passed for deviceId. At most deviceIdMaxLength bytes will
 * be written to the buffer, including terminating nul character.
 * If deviceIdMaxLength is 0 the required buffer length will be returned, but nothing will be written
 * to the deviceId buffer.
 *
 * \return The number of charactes written to deviceId, including
 * terminating nul character. If an error occured, a negative value
 * indicating one of the RCReturnCode error codes is returned.
 */
int RCAPI RCDeviceID(unsigned int index, char *deviceId, unsigned int maxLength);
/** Function type definition for RCEnumerateDevices() */
typedef int (RCCALL *TRCDeviceID)(unsigned int, char *, unsigned int);

/** @} */


/** \defgroup access Device Access
 *
 * \brief Open and Close devices. Read and Set the device label.
 *  @{
 */

/** \brief Open Device
 *
 * Open the device specified by the device ID string
 * as returned by RCDeviceID().
 *
 * \param deviceId Device ID string as read with by RCDeviceID().
 * This is an UTF-8 encoded printable string with terminating nul character.
 *
 * \return On success, a handle to the device. If the return value
 * is negative an error has occured and the device has not been
 * opened. The return value indicates one of the RCReturnCode error codes.
 *
 */
int RCAPI RCOpenDevice(const char *deviceId);
/** Function type definition for RCOpenDevice() */
typedef int (RCCALL *TRCOpenDevice)(const char *);


/** \brief Close Device
 *
 * Close the device identified by its device handle.
 *
 * \param handle Device handle as obtained by RCOpenDevice()
 *
 * \return RCOk on success. If an error occured, a negative value
 * indicating one of the RCReturnCode error codes.
 */
int RCAPI RCCloseDevice(int handle);
/** Function type definition for RCCloseDevice() */
typedef int (RCCALL *TRCCloseDevice)(int);

/** \brief Read the device label.
  *
  * This function reads the device label for the device specified by handle.
  * The device label is a UTF-8 encoded string that the user can set to further
  * describe the device. For instance the user can set the device label "Projector
  * left of stage" for the device controlling the projector located left of stage.
  * The device label can be set using RCSetDeviceLabel()
  *
  * \param handle Device handle as obtained by RCOpenDevice();
  * \param deviceLabel pointer
  * \param maxLength Length of the buffer passes as deviceLabel
  *
  * \return The number of characters written to deviceLabel.
  * If an error occured, a negative value indicating one of the
  * RCReturnCode error codes is returned.
  */
int RCAPI RCDeviceLabel(int handle, char *deviceLabel, unsigned int maxLength);
/** Function type definition for RCDeviceLabel() */
typedef int (RCCALL *TRCDeviceLabel)(int, char *, unsigned int);

/** \brief Set the device label.
  *
  * This function sets the device label to the nul-terminated string
  * passend in deviceLabel.
  *
  * \param handle Device handle as obtained by RCOpenDevice()
  * \param deviceLabel UTF-8 encoded string containing the device label.
  * The string must be end with a terminating nul character.
  *
  * \return RCOk on success. If an error occured, a negative
  * value indicating one of the RCReturnCode error codes is returned.
  */
int RCAPI RCSetDeviceLabel(int handle, const char *deviceLabel);
/** Function type definition for RCSetDeviceLabel() */
typedef int (RCCALL *TRCSetDeviceLabel)(int, const char *);

/** @} */

/** \defgroup output Laser Output
 *
 * \brief Start and Stop laser output. Write frames and query buffer status.
 *
 *  @{
 */

/** \brief Start Laser Output
 *
 * This function initialises the device and activates
 * the shutter signal.
 *
 * \param handle Device handle as obtained by RCOpenDevice();
 * \return RCOk on success. If an error occured, a negative
 * value indicating one of the RCReturnCode error codes is returned.
 *
 */
int RCAPI RCStartOutput(int handle);
/** Function type definition for RCStartOutput() */
typedef int (RCCALL *TRCStartOutput)(int);

/** \brief Stop Laser Output
 *
 * This function stops the laser output and deactivates the shutter signal.
 *
 * \param handle Device handle as obtained by RCOpenDevice();
 * \return RCOk on success. If an error occured, a negative
 * value indicating one of the RCReturnCode error codes is returned.
 */
int RCAPI RCStopOutput(int handle);
/** Function type definition for RCStopOutput() */
typedef int (RCCALL *TRCStopOutput)(int);

/** \brief Wait for Buffer
  *
  * \param handle Device handle as obtained by RCOpenDevice();
  * \param timeout The maximum time to wait for a buffer to become free
  *  in milliseconds.
  *
  *  If timeOut is is zero, this function does not wait und returns the
  *  number of free buffers immediately.
  *  If timeOut is negative, this function will wait for a buffer to
  *  become free without timing out.
  *
  * \return The number of free buffers. If an error occured, a negative
  * value indicating one of the RCReturnCode error codes is returned.
  *
  */

int RCAPI RCWaitForReady(int handle, int timeout);
/** Function type definition for RCWaitForReady() */
typedef int (RCCALL *TRCWaitForReady)(int, int);

/** \brief Query maximum sampling rate of the device.
  *
  * This function queries the maximum sampling rate of the device in Hz.
  * \param handle Device handle as obtained by RCOpenDevice(int handle);
  * \return The maximum sampling rate of the device in Hz. If an error occured,
  * a negative value indicating one of the RCReturnCode error codes is returned.
  */
int RCAPI RCMaxSpeed(int handle);
/** Function type definition for RCMaxSpeed() */
typedef int (RCCALL *TRCMaxSpeed)(int);

/** \brief Write Frame
 *
 * Write a frame to the device.
 *
 * \param handle Device handle as obtained by RCOpenDevice()
 * \param points pointer to an array of RCPoint structures containing the vector data;
 * \param count number of points that should be written
 * \param speed sampling rate in Hz that should be used for this frame
 * \param repeat how often this frame should be repeated. If 0, the frame
 * will be repeated continuously until a new frame is written.
 * \return RCOk on success. If an error occured, a negative value indicating
 * one of the RCReturnCode error codes is returned.
 *
 */

int RCAPI RCWriteFrame(int handle, const struct RCPoint *points, unsigned int count, unsigned int speed, unsigned int repeat);
/** Function type definition for RCWriteFrame() */
typedef int (RCCALL *TRCWriteFrame)(int, const struct RCPoint *, unsigned int, unsigned int, unsigned int);

/** @} */

/** \defgroup universes DMX Input and Output
 *
 * \brief Query DMX universe information. Read and write DMX data.
 *
 *  @{
 */

/** \brief Query universe count of device.
  *
  * This function returns how many universes exist for the
  * device specified by devicehandle.
  *
  * \param handle Device handle as obtained by RCOpenDevice()
  *
  * \return The number of universes present on the device. If an error
  * occured, a negative value indicating one of the RCReturnCode error
  * codes is returned.
  */
int RCAPI RCUniverseCount(int handle);
/** Function type definition for RCUniverseCount() */
typedef int (RCCALL *TRCUniverseCount)(int);


/** \brief Query universe information.
  *
  * This function returns the universe name, if it is an
  * input or an output and the channel count.
  *
  * \param handle Device handle as obtained by RCOpenDevice()
  * \param universeIndex Universe Index. Range is 0 to the number
  * returned by RCUniverseCount() -1
  * \param universeName Pointer to the buffer where the universe
  * name will be copied
  * \param maxLength Length of buffer passed in deviceLabel
  * \param pUniverseDirection pointer to universe Direction variable will be set to
  * either RCOutput or RCInput
  * \param pChannelCount Pointer to Channel count. Will be set the the channel
  * count of the universe
  *
  * \return The number of characters written to universeName. If an error
  * occured, a negative value indicating one of the RCReturnCode error
  * codes is returned.
  */
int RCAPI RCUniverseQuery(int handle, unsigned int universeIndex, char *universeName, unsigned int maxLength, enum RCUniverseDirection *pUniverseDirection, unsigned int *pChannelCount);
/** Function type definition for RCUniverseQuery() */
typedef int (RCCALL *TRCUniverseQuery)(int, unsigned int, char *, unsigned int, enum RCUniverseDirection *, unsigned int *);

/** \brief Write to output universe
  *
  * \param handle Device handle as obtained by RCOpenDevice()
  * \param universeIndex Universe Index. Range is 0 to the number
  * returned by RCUniverseCount() -1
  * \param startChannel First channel to write. startChannel 0 is the first DMX channel.
  * \param data Pointer to dmx value buffer
  * \param count Number of channels to write
  *
  * \return RCOk on success. If an error occured, a negative value indicating
  * one of the RCReturnCode error codes is returned.
  */
int RCAPI RCUniverseWrite(int handle, unsigned int universeIndex, unsigned int startChannel, const unsigned char *data, unsigned int count);
/** Function type definition for RCUniverseWrite() */
typedef int (RCCALL *TRCUniverseWrite)(int, unsigned int, unsigned int, const unsigned char *, unsigned int);

/** \brief Read from input universe
  *
  * \param handle Device handle as obtained by RCOpenDevice()
  * \param universeIndex Universe Index. Range is 0 to the number
  * returned by RCUniverseCount() -1
  * \param startChannel First channel to write. 0 is the first channel.
  * \param data Pointer to dmx value buffer
  * \param count Number of channels to read
  *
  * \return RCOk on success. If an error occured, a negative value indicating
  * one of the RCReturnCode error codes is returned.
  */
int RCAPI RCUniverseRead(int handle, unsigned int universeIndex, unsigned int startChannel, unsigned char *data, unsigned int count);
/** Function type definition for RCUniverseRead() */
typedef int (RCCALL *TRCUniverseRead)(int, unsigned int, unsigned int, unsigned char *, unsigned int);

/** \brief Flush Universe
  *
  * \param handle Device handle as obtained by RCOpenDevice()
  * \param universeIndex Universe Index. Range is 0 to the number
  * returned by RCUniverseCount() -1
  *
  * \return RCOk on success. If an error occured, a negative value indicating
  * one of the RCReturnCode error codes is returned.
  */
int RCAPI RCUniverseUpdate(int handle, unsigned int universeIndex);
/** Function type definition for RCUniverseUpdate() */
typedef int (RCCALL *TRCUniverseUpdate)(int, unsigned int);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* RAYCOMPOSER_DEVICE_API_H */
