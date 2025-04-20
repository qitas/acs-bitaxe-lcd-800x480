// Define the model / interface available for the current build
// debug mode
#define DEBUG_UI 0

#define BitaxeUltra 0
#define BitaxeSupra 1 
#define BitaxeGamma 0

#define BAPPORT 1

#define I2C 0

#define ACSBitaxeTouch 1

#define BlockStreamJade 1
#define SoloSatoshi 1
#define ALTAIR 0
#define SoloMiningCo 0
#define BTCMagazine 1
#define VoskCoin 1

// Firmware version
#define MajorVersion 0
#define MinorVersion 2
#define PatchVersion 2
#define VersionNote "Beta1"

// Firmware build date
#define BuildDate "04-01-2025"

// Model Build Type
#if (BitaxeUltra == 1)
#define ModelType "BM1366"
#elif (BitaxeSupra == 1)
#define ModelType "BM1368"
#elif (BitaxeGamma == 1)
#define ModelType "BM1370"
#endif


