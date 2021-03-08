import lseBinding from "./lse-binding"
import ref from "ref"

let result = null

var outInt = ref.alloc('int'); // allocate a 4-byte (32-bit) chunk for the output data
result = lseBinding["_LSCAN_Main_GetDeviceCount@4"](outInt);
var deviceCount = outInt.deref();

console.log("device count: ", deviceCount);