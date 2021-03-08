/// LScanEssentials API definition.
/// @file     LScanEssentialsApi.h
/// @if FILE_HEADER
/// @author   (c) 2006-2012 Cross Match Technologies Inc.
/// @since    2006-05-04
/// 
/// $Id: LScanEssentialsApi.h,v 1.105.2.1 2012-01-10 10:24:22 mf Exp $
/// @endif

#pragma once

#include "LScanEssentialsApi_defs.h"
#include "LScanEssentialsApi_err.h"
#include "ImageData.h"

#ifdef __cplusplus
extern "C" { 
#endif

///@name Main Interface Functions
///@{

/// Get DLL version information.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Main_GetAPIVersion(
      LScanApiVersion *info                           ///< [out] API version information \n
                                                      ///<       Memory must be provided by caller
    );

/// Retrieve count of connected L SCAN live scanner devices.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Main_GetDeviceCount(
      int *deviceCount                                ///< [out] Number of connected devices \n
                                                      ///  Memory must be provided by caller
    );

/// Retrieve detailed device information about particular scanner given by logical index.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Main_GetDeviceInfo(
      const int       deviceIndex,                    ///< [in] Zero-based device index for device to lookup
      LScanDeviceInfo *deviceInfo                     ///< [out] Basic device information \n
                                                      ///<       Memory must be provided by caller
    );

/// Callback to signal operation progress.
/// This registers a global callback that notifies progress for any connected device. 
/// Currently the callback is fired for initialization and infield test progress changes.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Main_RegisterCallbackProgress(
      LSCAN_CallbackProgress callback,                ///< [in] Pointer to the notification function 
      void                   *context                 ///< [in] Pointer to user context; this value is used as parameter for callback
    );

/// Callback to signal device count changes.
/// This registers a global callback that notifies changes in number of connected device. 
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Main_RegisterCallbackDeviceCount(
      LSCAN_CallbackDeviceCount callback,
      void                      *context
    );


/// Check if device is adjusted to meet image quality requirements.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Main_ImageQualityInfieldTest(
      const int deviceIndex,                          ///< [in] Zero-based index to access device (in range as returned by LSCAN_Main_GetDeviceInfo)
      LPCSTR    logFilePath                           ///< [in] Complete path to log file generated in case of test not passed
    );

/// Install licenses from license file to device.
/// LSCAN_Main_GetDeviceCount() together with LSCAN_Main_GetDeviceInfo() can be used 
/// to determine the device to load with the given license file. 
/// @note The device is not allowed to be initialized before.
/// @return status code as defined in LS_DeviceApi_err.h
int WINAPI LSCAN_Main_InstallLicenseFile(
      const int  deviceIndex,                         ///< [in] Zero based device index (range: 0 <= Index < LS_Device_GetDeviceCount() )
      const char *LicenseFileName                     ///< [in] File name of license file to load
    );

/// Initialize device given by device index.
/// An automatic cleanliness check is performed if the device is used the first time after power up
/// or if parameter @e reset is set to TRUE. 
/// @note Any initialized device must be released before hosting module's shutdown!
/// (i.e. call LSCAN_Main_Release() or LSCAN_Main_ReleaseAll()).
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_STATUS_OK -> device is ready to be used
/// @return @ref LSCAN_WRN_ALREADY_INITIALIZED -> indicates that the device was already initialized and can be used
///   (note: device state now as if newly initialized)
/// @return @ref LSCAN_WRN_OPTICS_SURFACE_DIRTY -> capture platen needs to be cleaned; in this case device should be released, 
///   platen cleaned and then initialized again with parameter @e reset= TRUE
int WINAPI LSCAN_Main_Initialize(
      const int  deviceIndex,                         ///< [in]  Zero-based device index for device to init
      const BOOL reset,                               ///< [in]  Perform a device reset prior to initialization.
      int        *handle                              ///< [out] Function returns device handle to be used for subsequent function calls \n
                                                      ///<       Memory must be provided by caller
  );


/// @copybrief LSCAN_Main_Initialize
/// Unlike LSCAN_Main_Initialize(), all subsequent visualization requests will be
/// redirected to an external process through a named pipe.
int WINAPI LSCAN_Main_Initialize_ExternalVisualization(
      const int  deviceIndex,                         ///< [in]  Zero-based device index for device to init
      const BOOL reset,                               ///< [in]  Perform a device reset prior to initialization.
      int        *handle,                             ///< [out] Function returns device handle to be used for subsequent function calls \n
                                                      ///<       Memory must be provided by caller
      const char *pipeName                            ///< [in]  name of the visualization pipe received by Visualization_Create()
  );


/// Release device.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_RESOURCE_LOCKED -> a callback is still active
/// @return @ref LSCAN_ERR_NOT_INITIALIZED -> device(s) in use are identified by index;
///   so either device has aready been released or is unknown
int WINAPI LSCAN_Main_Release(
      const int  handle,                              ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const BOOL sendToStandby                        ///< [in] Set device in standby mode (if set to TRUE)
    );
    
/// Release all currently initialized devices.
/// @note This function should be called on application shutdown to free allocated resources.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_RESOURCE_LOCKED -> a callback is still active
int WINAPI LSCAN_Main_ReleaseAll(
      const BOOL sendToStandby                        ///< [in] If TRUE then set devices in standby mode
    ); 

/// Check if device is initialized.
/// @return @ref LSCAN_STATUS_OK -> device is ready to be used
/// @return @ref LSCAN_ERR_INVALID_PARAM_VALUE -> if handle value is out of valid range
/// @return @ref LSCAN_ERR_NOT_INITIALIZED -> device is not initialized
/// @return @ref LSCAN_ERR_DEVICE_IO -> device is initialized but there was a communication problem 
///   (e.g. disconnection; LSCAN_Main_Release() must be called in order to free allocated resources)
int WINAPI LSCAN_Main_IsInitialized(
      const int  handle                               ///< [in] Device handle obtained by LSCAN_Main_Initialize()
    );

/// Retrieve device property value.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Main_GetProperty(
      const int             handle,                   ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      const LScanPropertyId propertyId,               ///< [in]  Property identifier to get value for
      LPSTR                 propertyValue             ///< [out] String returning property value \n
                                                      ///<       Memory must be provided by caller
                                                      ///<       (needs to be able to hold @ref LSCAN_MAX_STR_LEN characters)
    );

/// Set device property value.
/// @note Only specific property values can be set.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Main_SetProperty(
      const int             handle,                   ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const LScanPropertyId propertyId,               ///< [in] Property identifier to set value for
      LPCSTR                propertyValue             ///< [in] String containing property value
    );

/// Check if scanner surface(s) is/are free of dirt.
/// If a capture is pending then the check is performed only on the active channel.
/// Otherwise the check is performed on all channels.
/// @note This check only recognizes dirt added after the last full device initialization.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_STATUS_OK -> if platen surface(s) is/are clean
/// @return @ref LSCAN_WRN_OPTICS_SURFACE_DIRTY -> if platen surface(s) need(s) to be cleaned
int WINAPI LSCAN_Main_CheckCleanliness(
      const int handle                                ///< [in] Device handle obtained by LSCAN_Main_Initialize()
    );


/// Perform an adjustment of the device. 
/// Calls to this function do not affect the automatic adjustment set by property @ref LSCAN_PROPERTY_AUTOMATIC_ADJUSTMENT.
/// @note If a capture is pending, no adjustment is done.
/// @return @ref LSCAN_STATUS_OK on success
/// @return @ref LSCAN_ERR_CAPTURE_IN_PROGRESS if a capture is active
/// @return @ref LSCAN_WRN_OPTICS_SURFACE_DIRTY if adjustment could not be performed because the platen was dirty
int WINAPI LSCAN_Main_ForceReadjustment(
      const int handle                                ///< [in] Device handle obtained by LSCAN_Main_Initialize()
    );


/// Callback to signal device communication interruption.
/// This function registers a device related callback that notifies device communication breakage.
/// Once the callback is fired all subsequent API functions (except LSCAN_Main_Release())
/// return error code @ref LSCAN_ERR_DEVICE_IO.
/// @note The callback is associated with the device specified by handle and therefore the registration 
/// is automatically deleted on device release.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Main_RegisterCallbackCommunicationBreak(
      const int      handle,                          ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      LSCAN_Callback callback,                        ///< [in] Pointer to the notification function 
      void           *context                         ///< [in] Pointer to user context; this value is used as parameter for callback
    );

///@}

///@name Image Acquisition Related Interface Functions
///@{

/// Check if requested capture mode is supported by the device.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Capture_IsModeAvailable(
      const int                   handle,             ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      const LScanImageType        imageType,          ///< [in]  Image type to verify
      const LScanImageResolution  imageResolution,    ///< [in]  Requested capture resolution
      BOOL                        *isAvailable        ///< [out] Returns TRUE if mode is available \n
                                                      ///<       Memory must be provided by caller
    );

/// Set capture mode.
/// Capture mode setting is a prerequisite for LSCAN_CAPTURE_Start().
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_CAPTURE_IN_PROGRESS -> an aquisition is currently pending and needs to be completed first
/// @return @ref LSCAN_ERR_NOT_SUPPORTED -> certain options (e.g auto capture) require a license which is not available
/// @return @ref LSCAN_ERR_CHANNEL_INVALID_CAPTURE_MODE -> device does not support the requested acquisition mode 
///   (may also depend on installed licenses; e.g. finger print rolling)
int WINAPI LSCAN_Capture_SetMode(
      const int                   handle,             ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      const LScanImageType        imageType,          ///< [in]  Image type to capture
      const LScanImageResolution  imageResolution,    ///< [in]  Requested capture resolution
      const LScanImageOrientation lineOrder,          ///< [in]  Required result image line order
      const DWORD                 captureOptions,     ///< [in]  Bit coded capture options to use (see @ref CaptureOptions)
      int                         *resultWidth,       ///< [out] Returns width of captured result image in pixels \n
                                                      ///<       Memory must be provided by caller (ignored when NULL given)
      int                         *resultHeight,      ///< [out] Returns height of captured result image in pixels \n
                                                      ///<       Memory must be provided by caller (ignored when NULL given)
      int                         *baseResolutionX,   ///< [out] Returns image horizontal base resolution before processing \n
                                                      ///<       Memory must be provided by caller (ignored when NULL given)
      int                         *baseResolutionY    ///< [out] Returns image vertical base resolution before processing \n
                                                      ///<       Memory must be provided by caller (ignored when NULL given)
    );
    
/// Start image acquisition.
/// The acquisition is done asynchronously.
/// LSCAN_Capture_Abort() can be called to abort acquisition.
/// Result image acquisition is done by auto capture functionality (if enabled)
/// or by call to LSCAN_Capture_TakeResultImage().  
/// Result image acquisition completion is signaled by callback LSCAN_CallbackResultImageAvailable(). 
/// @note Once image acquisition is completed then image streaming continues in background
/// to minimize delays when restarting acquisition. In order to stop communication traffic on the PC bus 
/// systems this image streaming can be stopped by setting capture mode to @ref LSCAN_TYPE_NONE.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_CAPTURE_IN_PROGRESS -> an aquisition is currently pending and needs to be completed first
/// @return @ref LSCAN_ERR_INVALID_PARAM_VALUE -> parameter @e numberOfObjects needs to be in range 1..4
/// @return @ref LSCAN_ERR_CHANNEL_INVALID_CAPTURE_MODE -> acquisition mode needs to be set as a prerequisite 
///   -> call LSCAN_Capture_SetMode() first
int WINAPI LSCAN_Capture_Start(
      const int  handle,                              ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const int  numberOfObjects                      ///< [in] Number of expected objects (fingertips + palm areas) in the image.
                                                      ///<      It is used if option @ref LSCAN_OPTION_AUTO_CAPTURE is set 
                                                      ///<      at @ref LSCAN_Capture_SetMode(). Please refer to the image types 
                                                      ///<      @ref enumLScanImageType for details about valid values. 
    );

/// Abort running image acquisition.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_NOT_CAPTURING -> no active acquisition to be aborted
int WINAPI LSCAN_Capture_Abort(
      const int handle                                ///< [in] Device handle obtained by LSCAN_Main_Initialize()
    );

/// Check if image acquisition is in progress.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Capture_IsActive(
      const int handle,                               ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      BOOL      *isActive                             ///< [out] Returns TRUE if acquisition is in progress
                                                      ///<       (preview or result image acquisition) \n
                                                      ///<       Memory must be provided by caller
    );    

/// Initiate result image acquisition.
/// For flat finger or palm image acquisition this function
/// triggers result image acquisition (regardless to auto capture option) \n
/// It overwrites @ref LSCAN_CLEAR_OBJECT_FROM_PLATEN state - a subsequent LSCAN_CallbackClearObjectsFromPlaten()
/// with state @ref LSCAN_PLATED_CLEARED is not sent. \n\n
/// For rolled finger print acquisition it: \n
/// 1. triggers roll start (only for captures without auto capture option) \n
/// 2. accepts result image (if device option @ref LSCAN_PROPERTY_ROLL_ALLOW_RESTART is set to TRUE) \n\n
/// Result image acquisition completion is signaled by AcquisitionComplete() callback, 
/// availability of the result image by ResultImage() callback. 
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_NOT_CAPTURING -> no active acquisition to trigger result image for 
int WINAPI LSCAN_Capture_TakeResultImage(
      const int handle                                ///< [in] Device handle obtained by LSCAN_Main_Initialize()
    );
    
/// Perform a contrast optimization.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_CHANNEL_NOT_ACTIVE -> acquisition channel needs to be selected as a prerequisite 
///   -> call LSCAN_Capture_SetMode() first
/// @return @ref LSCAN_ERR_NO_HAND_FINGER -> finger/hand requires to be placed on platen during contrast optimization
int WINAPI LSCAN_Capture_OptimizeContrast(
      const int handle                                ///< [in] Device handle obtained by LSCAN_Main_Initialize()
    );

/// Get contrast value for selected capture mode selected by LSCAN_Capture_SetMode().
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_CHANNEL_NOT_ACTIVE -> acquisition channel needs to be selected as a prerequisite 
///   -> call LSCAN_Capture_SetMode() first
int WINAPI LSCAN_Capture_GetContrast(
      const int handle,                               ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      int       *contrastValue                        ///< [out] Contrast value (range: 0 <= value <= @ref LSCAN_MAX_CONTRAST_VALUE) \n
                                                      ///<       Memory must be provided by caller
    );

/// Set contrast value for selected capture mode selected by LSCAN_Capture_SetMode().
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_CHANNEL_NOT_ACTIVE -> acquisition channel needs to be selected as a prerequisite 
///   -> call LSCAN_Capture_SetMode() first
int WINAPI LSCAN_Capture_SetContrast(
      const int handle,                               ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const int contrastValue                         ///< [in] Contrast value (range: 0 <= value <= @ref LSCAN_MAX_CONTRAST_VALUE)
    );

/// Set result image clipping area.
/// This function defines the area of result image that is returned with LSCAN_CallbackResultImage(). \n
/// Point of origin is the upper left corner of the complete image. \n\n
/// Depending on given parameters several modes for image clip are supported: \n
/// x>=0; y>=0; width>0; height>0 : fixed clip size and position \n
/// any other invalid parameters  : disable clipping, return full result image. \n\n
/// L SCAN 1000PX and L SCAN 500P support additional mode for capture of flats: \n
/// x=-1; y=-1; width>0; height>0 : fixed clip size, automatic clip position detection; for flats modes only \n
/// @note The clipping does not affect the size of the displayed image.\n
/// Active area is automatically reset when calling any of the following functions: 
/// LSCAN_Main_Release(), LSCAN_Capture_SetMode() \n
/// or if called with invalid parameter value(s). \n
/// @note Width and height have to be dividable by 4 when using this function on L SCAN 1000PX device! \n
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_CAPTURE_IN_PROGRESS -> active area cannot be set while acquisition is pending
/// @return @ref LSCAN_ERR_CHANNEL_INVALID_CAPTURE_MODE -> acquisition mode needs to be set as a prerequisite 
///   -> call LSCAN_Capture_SetMode() first
/// @return @ref LSCAN_ERR_INVALID_PARAM_VALUE -> active area needs to be within result image dimensions
int WINAPI LSCAN_Capture_SetActiveArea(
      const int handle,                               ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const int x,                                    ///< [in] Clipping horizontal start position
      const int y,                                    ///< [in] Clipping vertical start position
      const int width,                                ///< [in] Clipping width
      const int height                                ///< [in] Clipping height
    );

/// Register callback function for preview image availability.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_NOT_SUPPORTED if auto capture license is not available
int WINAPI LSCAN_Capture_RegisterCallbackPreviewImage(
      const int                  handle,               ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      LSCAN_CallbackPreviewImage callback,             ///< [in] Pointer to the notification function 
      void                       *context              ///< [in] Pointer to user context; this value is used as parameter for callback
    );

/// Register callback function for object (finger)finger count change during capture operation. 
/// Assumes empty scanner surface at the beginning.
/// @return status code as defined in LScanEssentialsApi_err.
/// @return @ref LSCAN_ERR_NOT_SUPPORTED if auto capture license is not available
int WINAPI LSCAN_Capture_RegisterCallbackObjectCount(
      const int                 handle,               ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      LSCAN_CallbackObjectCount callback,             ///< [in] Pointer to the notification function
      void                      *context              ///< [in] Pointer to user context; this value is used as parameter for callback
    );

/// Register callback function for fingertip qualities change during capture operation.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Capture_RegisterCallbackObjectQuality(
      const int                   handle,             ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      LSCAN_CallbackObjectQuality callback,           ///< [in] Pointer to the notification function
      void                        *context            ///< [in] Pointer to user context; this value is used as parameter for callback
    );

/// Register result image start callback function.
/// The registered function is then called by the SDK to signal begin of result image acquisition.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Capture_RegisterCallbackTakingResultImage(
      const int      handle,                          ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      LSCAN_Callback callback,                        ///< [in] Pointer to the notification function
      void           *context                         ///< [in] Pointer to user context; this value is used as parameter for callback
    );

/// Register result image acquisition completion callback function.
/// The registered function is then called by the SDK to signal the finish of result image acquisition.
/// Depending on device and image type the notification might be called 
/// immediately after LSCAN_CallbackTakingResultImage(). 
/// @note The result image availability is notified after post process completion 
/// via callback registered by LSCAN_Capture_RegisterCallbackResultImage().
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Capture_RegisterCallbackAcquisitionComplete(
      const int      handle,                          ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      LSCAN_Callback callback,                        ///< [in] Pointer to the notification function
      void           *context                         ///< [in] Pointer to user context; this value is used as parameter for callback
    );

/// Register callback function for result image availability.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Capture_RegisterCallbackResultImage(
      const int                 handle,               ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      LSCAN_CallbackResultImage callback,             ///< [in] Pointer to the notification function 
      void                      *context              ///< [in] Pointer to user context; this value is used as parameter for callback
    );

/// Register callback function for notification to remove finger(s)/hand from scanner surface prior 
/// to beginning of new capture. \n
/// The registration for this callback is not depending on availability of the auto capture license.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Capture_RegisterCallbackClearObjectsFromPlaten(
      const int                            handle,    ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      LSCAN_CallbackClearObjectsFromPlaten callback,  ///< [in] Pointer to the notification function
      void                                 *context   ///< [in] Pointer to user context; this value is used as parameter for callback
    );
///@}

///@name User Controls Related Interface Functions
///@{

/// Get available beeper type.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_GetAvailableBeeper(
      const int        handle,                        ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      LScanBeeperType* beeperType                     ///< [out] Type of beeper \n
                                                      ///<       Memory must be provided by caller
    );

/// Let device beep following a predefined pattern.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_NO_HARDWARE_SUPPORT -> device is not equipped with a beeper
int WINAPI LSCAN_Controls_Beeper(
      const int handle,                               ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const int pattern,                              ///< [in] Beeper pattern (0..7)
      const int volume                                ///< [in] Beeper volume in percent (value >= 0), 0 means off;
                                                      ///<      supported for L SCAN 500P only, otherwise ignored
    );

/// Get available user input keys (including foot switch).
/// @return status code as defined in LScanEssentialsApi_err.h
/// @note The optional USB connected foot switch is also supported by this API. 
int WINAPI LSCAN_Controls_GetAvailableKeys(
      const int       handle,                         ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      LScanKeypadType *keypadType,                    ///< [out] Type of keypad \n
                                                      ///<       Memory must be provided by caller
      int             *keyCount,                      ///< [out] Number of keys on keypad (without foot switch) \n
                                                      ///<       Memory must be provided by caller
      DWORD           *availableKeys                  ///< [out] Bit pattern of available keys including foot switch \n
                                                      ///<       Memory must be provided by caller
    );

/// Set active user input keys (including foot switch).
/// State changes for the keys that are enabled by this function are signaled
/// via callback registered with LSCAN_Controls_RegisterCallbackKeys().
/// @note Keys must be released while or after call to the function before they get
/// signaled by the callback. This is necessary to avoid keybounce.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @note Please call LSCAN_Controls_GetAvailableKeys() to obtain type of 
/// built in keypad and supported keys.
/// The optional USB connected foot switch is also supported by this API.
/// @return @ref LSCAN_ERR_NO_HARDWARE_SUPPORT -> attempted to activate keys that
/// the device is not equipped with
int WINAPI LSCAN_Controls_SetActiveKeys(
      const int   handle,                             ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const DWORD activeKeys                          ///< [in] Bit pattern of active keys including foot switch
    );

/// Get available status LED's.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_GetAvailableLEDs(
      const int    handle,                            ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      LScanLedType *ledType,                          ///< [out] Type of LED's \n
                                                      ///<       Memory must be provided by caller
      int          *ledCount,                         ///< [out] Number of LED's \n
                                                      ///<       Memory must be provided by caller
      DWORD        *availableLEDs                     ///< [out] Bit pattern of available LED's \n
                                                      ///<       Memory must be provided by caller
    );

/// Set active status LED's on device.
/// @return status code as defined in LScanEssentialsApi_err.h
/// @return @ref LSCAN_ERR_NO_HARDWARE_SUPPORT -> device is not equipped with status LEDs
int WINAPI LSCAN_Controls_SetActiveLEDs(
      const int   handle,                             ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const DWORD activeLEDs                          ///< [in] Bit pattern of active LED's
    );

/// Get active status LED's for device.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_GetActiveLEDs(
      const int handle,                               ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      DWORD     *activeLEDs                           ///< [out] Bit pattern of active LED's \n
                                                      ///<       Memory must be provided by caller
    );

/// Register callback for keypad/foot switch events.
/// This function registers/unregisters a callback function for receiving key state change notifications.
/// @note LSCAN_Controls_SetActiveKeys() must also be called to specify active keys. 
/// Per default no keys are active. The calling sequence is not important.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_RegisterCallbackKeys(
      const int          handle,                      ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      LSCAN_CallbackKeys callback,                    ///< [in] Pointer to the notification function
      void               *context                     ///< [in] Pointer to user context; this value is used as parameter for callback
    );

/// Show company logo screen on device display.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_DisplayShowLogoScreen(
      const int                    handle,            ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const LScanDisplayLogoOption logoOption,        ///< [in] option for screen display (see @ref DisplayLogoOptions)
      const int                    progressBarPercent ///< [in] progress bar value in 0%...100%
    );

/// Show operation mode selection screen on device display.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_DisplayShowModeSelectScreen(
      const int handle                                ///< [in] Device handle obtained by LSCAN_Main_Initialize()
    );

/// Show resolution selection screen on device display.
/// @note Function can be used for devices which support more than one native capture resolution (e.g. 500 and 1000ppi).
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_DisplayShowResolutionSelectScreen(
      const int handle                                ///< [in] Device handle obtained by LSCAN_Main_Initialize()
    );

/// Show finger selection screen on device display.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_DisplayShowFingerSelectionScreen(
      const int                       handle,                 ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const LScanDisplaySelectionCtrl ctrlLeft,               ///< [in] icon for left button: finger selection type (e.g. bandaged), (see @ref DisplaySelectionCtrl)
      const LScanDisplayCommonCtrl    ctrlRight,              ///< [in] icon for right button (see @ref DisplayCommonCtrl)
      const LScanDisplayObjectColor   colorLeftPalm,          ///< [in] color state for left palm (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorLeftThenar,        ///< [in] color state for left thenar (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorLeftLowerThenar,   ///< [in] color state for left lower thenar (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorLeftInterDigital,  ///< [in] color state for left interdigital (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorLeftThumb,         ///< [in] color state for left thumb (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorLeftIndex,         ///< [in] color state for left index finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorLeftMiddle,        ///< [in] color state for left middle finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorLeftRing,          ///< [in] color state for left ring finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorLeftSmall,         ///< [in] color state for left small finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorRightPalm,         ///< [in] color state for right palm (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorRightThenar,       ///< [in] color state for right thenar (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorRightLowerThenar,  ///< [in] color state for right lower thenar (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorRightInterDigital, ///< [in] color state for right interdigital (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorRightThumb,        ///< [in] color state for right thumb (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorRightIndex,        ///< [in] color state for right index finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorRightMiddle,       ///< [in] color state for right middle finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorRightRing,         ///< [in] color state for right ring finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor   colorRightSmall         ///< [in] color state for right small finger (see @ref DisplayObjectColor)
    );

/// Display next appropriate finger selection type on finger selection screen.
/// This cycles through bandaged, missing, restricted and unrestricted.
/// Note: finger selection screen must be active (call LSCAN_Controls_DisplayShowFingerSelectionScreen once before!).
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_DisplayShowNextFingerSelection(
      const int                  handle,                      ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      LScanDisplaySelectionCtrl* pNextCtrlLeft                ///< [out] Icon for left button: current selection type which has been activated, 
                                                              ///<       (see @ref DisplaySelectionCtrl) \n
                                                              ///<       Memory must be provided by caller
    );

/// Show capture progress screen on device display.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Controls_DisplayShowCaptureProgressScreen(
      const int                     handle,                   ///< [in] Device handle obtained by LS_Device_InitializeDevice()
      const LScanDisplayCommonCtrl  ctrlLeft,                 ///< [in] icon for left button (see @ref DisplayCommonCtrl)
      const LScanDisplayCommonCtrl  ctrlRight,                ///< [in] icon for right button (see @ref DisplayCommonCtrl)
      const LScanDisplayStatTop     scanStatTop,              ///< [in] scan state icon (e.g. roll direction) (see @ref DisplayStatTop)
      const LScanDisplayStatBottom  scanStatBottom,           ///< [in] icon for scan result (e.g. roll error) (see @ref DisplayStatBottom)
      const LScanDisplayObjectColor colorLeftPalm,            ///< [in] color state for left palm (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorLeftThenar,          ///< [in] color state for left thenar (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorLeftLowerThenar,     ///< [in] color state for left lower thenar (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorLeftInterDigital,    ///< [in] color state for left interdigital (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorLeftThumb,           ///< [in] color state for left thumb (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorLeftIndex,           ///< [in] color state for left index finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorLeftMiddle,          ///< [in] color state for left middle finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorLeftRing,            ///< [in] color state for left ring finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorLeftSmall,           ///< [in] color state for left small finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorRightPalm,           ///< [in] color state for right palm (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorRightThenar,         ///< [in] color state for right thenar (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorRightLowerThenar,    ///< [in] color state for right lower thenar (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorRightInterDigital,   ///< [in] color state for right interdigital (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorRightThumb,          ///< [in] color state for right thumb (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorRightIndex,          ///< [in] color state for right index finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorRightMiddle,         ///< [in] color state for right middle finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorRightRing,           ///< [in] color state for right ring finger (see @ref DisplayObjectColor)
      const LScanDisplayObjectColor colorRightSmall           ///< [in] color state for right small finger (see @ref DisplayObjectColor)
    );
///@}

///@name Image Visualization Related Interface Functions
///@{

/// Create a Visualization Object used for the current process. 
/// @return The name of a pipe to be used for communication between device and 
///         visualization, which may work in different processes. This pipe name 
///         has to be passed to LSCAN_Main_Initialize_ExternalVisualization().
/// @note This function must be called from the process hosting the GUI.
const char* WINAPI LSCAN_Visualization_Create();

/// Destroy a Visualization Object
void WINAPI LSCAN_Visualization_Destroy(
    const char* visPipeName   ///< [in] pipeName, obtained from LSCAN_Visualization_Create()
  );




/// Set active visualization mode.
/// @note LSCAN_Visualization_SetWindow() must be called also to set area to display image in
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_SetMode(
      const int          handle,                      ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const LScanVisMode mode,                        ///< [in] Visualization mode to use
      const DWORD        options                      ///< [in] Bit coded visualization options to use (see @ref VisualizationOptions)
    );

/// Set Window and window area for display operations.
/// Preview and result images are automatically scaled to fit into this area. 
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_SetWindow(
      const int  handle,                              ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const HWND hWnd,                                ///< [in] window handle of window to draw to
      const RECT drawRect                             ///< [in] client rectangle to draw to\n
                                                      ///<      Valid values: coordinates >= 0 and right >= left and bottom >= top\n
                                                      ///<      If invalid then whole client area is used
    );

/// Get the scale factor of the displayed image in relation to the result image size.
/// The scale factor depends on:\n
/// - The active visualization area (see LSCAN_Visualization_SetWindow())\n
/// - The active capture area (if activated by not setting option @ref LSCAN_OPTION_VIS_FULL_IMAGE with
///   LSCAN_Visualization_SetMode())\n
/// - The active capture mode (see LSCAN_Capture_SetMode())
///
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_GetScaleFactor(
      const int handle,                               ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      double    *scaleFactor                          ///< [out] The currently active scale factor \n
                                                      ///<       Memory must be provided by caller
    );    

/// Set background color of visualization area.
/// The default color after initialization is 0xe00e0e0e0.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_SetBackgroundColor(
      const int      handle,                          ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const COLORREF color                            ///< [in] Color to use (format: 0x00bbggrr)
    );

/// Remove an overlay from visualization.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_RemoveOverlay(
      const int   handle,                             ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const DWORD overlayHandle                       ///< [in] Handle to overlay to be removed from visualization  
    );
    
/// Remove all overlays from visualization.
/// @note The internally generated overlay to mark active user area is not removed.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_RemoveAllOverlays(
      const int handle                                ///< [in] Device handle obtained by LSCAN_Main_Initialize()
    );    

/// Show/hide an overlay.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_ShowOverlay(
      const int   handle,                             ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const DWORD overlayHandle,                      ///< [in] Handle to overlay to show/hide
      const BOOL  show                                ///< [in] Show overlay if TRUE (hide if FALSE)
    );

/// Show/hide all existing overlays.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_ShowAllOverlays(
      const int  handle,                              ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const BOOL show                                 ///< [in] Show overlays if TRUE (hide if FALSE)
    );

/// Add a text overlay to visualization.
/// set by LSCAN_Visualization_SetWindow() (exception: belongsToImage is set to TRUE)
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_AddOverlayText(
      const int      handle,                          ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      const char     *text,                           ///< [in]  Text to display
      const int      posX,                            ///< [in]  Horizontal text position
      const int      posY,                            ///< [in]  Vertical text position
      const COLORREF color,                           ///< [in]  Text color (format: 0x00bbggrr)
      const char     *fontName,                       ///< [in]  Text font
      const int      fontSize,                        ///< [in]  Text font size 
      const BOOL     belongsToImage,                  ///< [in]  Specify if overlay is related to image\n 
                                                      ///<       TRUE: position is set relative to image top left corner;
                                                      ///<       overlay is visible while image available\n
                                                      ///<       FALSE: position is set relative to the top left corner of drawing area;
                                                      ///<       overlay is visible while visualization is active
      DWORD          *overlayHandle                   ///< [out] Handle to the generated overlay \n
                                                      ///<       Memory must be provided by caller
    );

/// Modify an existing text overlay.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_ModifyOverlayText(
      const int   handle,                             ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const DWORD overlayHandle,                      ///< [in] Handle to text overlay to be modified  
      const char  *text,                              ///< [in] Text to display
      const int   posX,                               ///< [in] Horizontal text position
      const int   posY                                ///< [in] Vertical text position
    );

/// Add a quadrangle overlay to visualization.
/// set by LSCAN_Visualization_SetWindow() (exception: belongsToImage is set to TRUE)
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_AddOverlayQuadrangle(
      const int      handle,                          ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      const int      x1,                              ///< [in]  X coordinate of starting point
      const int      y1,                              ///< [in]  Y coordinate of starting point
      const int      x2,                              ///< [in]  X coordinate of first corner
      const int      y2,                              ///< [in]  Y coordinate of first corner
      const int      x3,                              ///< [in]  X coordinate of second corner
      const int      y3,                              ///< [in]  Y coordinate of second corner
      const int      x4,                              ///< [in]  X coordinate of third corner
      const int      y4,                              ///< [in]  Y coordinate of third corner
      const COLORREF color,                           ///< [in]  Text color (format: 0x00bbggrr)
      const int      lineWidth,                       ///< [in]  Quadrangle line width
      const BOOL     belongsToImage,                  ///< [in]  Specify if overlay is related to image\n 
                                                      ///<       TRUE: position is set relative to image top left corner;
                                                      ///<       overlay is visible while image available\n
                                                      ///<       FALSE: position is set relative to the top left corner of drawing area;
                                                      ///<       overlay is visible while visualization is active
      DWORD          *overlayHandle                   ///< [out] Handle to the generated overlay \n
                                                      ///<       Memory must be provided by caller
    );

/// Modify an existing quadrangle overlay.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_ModifyOverlayQuadrangle(
      const int   handle,                             ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const DWORD overlayHandle,                      ///< [in] Handle to quadrangle overlay to be modified 
      const int   x1,                                 ///< [in] X coordinate of starting point
      const int   y1,                                 ///< [in] Y coordinate of starting point
      const int   x2,                                 ///< [in] X coordinate of first corner
      const int   y2,                                 ///< [in] Y coordinate of first corner
      const int   x3,                                 ///< [in] X coordinate of second corner
      const int   y3,                                 ///< [in] Y coordinate of second corner
      const int   x4,                                 ///< [in] X coordinate of third corner
      const int   y4                                  ///< [in] Y coordinate of third corner
    );

/// Add a line overlay to visualization.
/// set by LSCAN_Visualization_SetWindow() (exception: belongsToImage is set to TRUE)
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_AddOverlayLine(
      const int      handle,                          ///< [in]  Device handle obtained by LSCAN_Main_Initialize()
      const int      x1,                              ///< [in]  X coordinate of starting point
      const int      y1,                              ///< [in]  Y coordinate of starting point
      const int      x2,                              ///< [in]  X coordinate of ending point
      const int      y2,                              ///< [in]  Y coordinate of ending point 
      const COLORREF color,                           ///< [in]  Text color (format: 0x00bbggrr)
      const int      lineWidth,                       ///< [in]  Quadrangle line width
      const BOOL     belongsToImage,                  ///< [in]  Specify if overlay is related to image\n 
                                                      ///<       TRUE: position is set relative to image top left corner;
                                                      ///<       overlay is visible while image available\n
                                                      ///<       FALSE: position is set relative to the top left corner of drawing area;
                                                      ///<       overlay is visible while visualization is active
      DWORD          *overlayHandle                   ///< [out] Handle to the generated overlay \n
                                                      ///<       Memory must be provided by caller
    );

/// Modify an existing line overlay.
/// @return status code as defined in LScanEssentialsApi_err.h
int WINAPI LSCAN_Visualization_ModifyOverlayLine(
      const int   handle,                             ///< [in] Device handle obtained by LSCAN_Main_Initialize()
      const DWORD overlayHandle,                      ///< [in] Handle to line overlay to be modified  
      const int   x1,                                 ///< [in] X coordinate of starting point
      const int   y1,                                 ///< [in] Y coordinate of starting point
      const int   x2,                                 ///< [in] X coordinate of ending point
      const int   y2                                  ///< [in] Y coordinate of ending point 
    );
///@}

#ifdef __cplusplus
} // extern "C"
#endif
