import ffi from "ffi"
import ref from "ref"

const int = ref.types.int
const lseDllLoc = "../resources/LScanEssentials-x86.dll"

// /// Retrieve count of connected L SCAN live scanner devices.
// /// @return status code as defined in LScanEssentialsApi_err.h
// int WINAPI LSCAN_Main_GetDeviceCount(
//     int *deviceCount                                ///< [out] Number of connected devices \n
//     ///  Memory must be provided by caller
// );

var intType = ref.types.int
var intPtr = ref.refType(intType);

const lseBinding = ffi.Library(lseDllLoc, {
    "_LSCAN_Main_GetDeviceCount@4": [int, [intPtr]],
})

export default lseBinding