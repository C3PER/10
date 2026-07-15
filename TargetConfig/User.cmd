
MEMORY
{
    /* --- System Flags --- */
    User_ADLen          : origin = 0xC6FFFFF0, length = 0x2
    User_ADEn           : origin = 0xC6FFFFF2, length = 0x1
    User_ADFlag         : origin = 0xC6FFFFF3, length = 0x1
    User_ADPingPong     : origin = 0xC6FFFFF4, length = 0x1

    User_DALen          : origin = 0xC6FFFFF8, length = 0x2
    User_DAEn           : origin = 0xC6FFFFFA, length = 0x1
    User_DAFlag         : origin = 0xC6FFFFFB, length = 0x1
    User_DAPingPong     : origin = 0xC6FFFFFC, length = 0x1
    User_DASel          : origin = 0xC6FFFFFD, length = 0x1

    /* --- AD Buffers --- */
    AD_Ch1Buf0          : origin = 0xC7000000, length = 0x4000
    AD_Ch2Buf0          : origin = 0xC7004000, length = 0x4000
    AD_Ch3Buf0          : origin = 0xC7008000, length = 0x4000
    AD_Ch4Buf0          : origin = 0xC700C000, length = 0x4000
    AD_Ch5Buf0          : origin = 0xC7010000, length = 0x4000
    AD_Ch6Buf0          : origin = 0xC7014000, length = 0x4000
    AD_Ch7Buf0          : origin = 0xC7018000, length = 0x4000
    AD_Ch8Buf0          : origin = 0xC701C000, length = 0x4000

    AD_Ch1Buf1          : origin = 0xC7020000, length = 0x4000
    AD_Ch2Buf1          : origin = 0xC7024000, length = 0x4000
    AD_Ch3Buf1          : origin = 0xC7028000, length = 0x4000
    AD_Ch4Buf1          : origin = 0xC702C000, length = 0x4000
    AD_Ch5Buf1          : origin = 0xC7030000, length = 0x4000
    AD_Ch6Buf1          : origin = 0xC7034000, length = 0x4000
    AD_Ch7Buf1          : origin = 0xC7038000, length = 0x4000
    AD_Ch8Buf1          : origin = 0xC703C000, length = 0x4000

    /* --- DA Buffers --- */
    DA_Ch1Buf0          : origin = 0xC7040000, length = 0x4000
    DA_Ch2Buf0          : origin = 0xC7044000, length = 0x4000
    DA_Ch3Buf0          : origin = 0xC7048000, length = 0x4000
    DA_Ch4Buf0          : origin = 0xC704C000, length = 0x4000
    DA_Ch5Buf0          : origin = 0xC7050000, length = 0x4000
    DA_Ch6Buf0          : origin = 0xC7054000, length = 0x4000
    DA_Ch7Buf0          : origin = 0xC7058000, length = 0x4000
    DA_Ch8Buf0          : origin = 0xC705C000, length = 0x4000

    DA_Ch1Buf1          : origin = 0xC7060000, length = 0x4000
    DA_Ch2Buf1          : origin = 0xC7064000, length = 0x4000
    DA_Ch3Buf1          : origin = 0xC7068000, length = 0x4000
    DA_Ch4Buf1          : origin = 0xC706C000, length = 0x4000
    DA_Ch5Buf1          : origin = 0xC7070000, length = 0x4000
    DA_Ch6Buf1          : origin = 0xC7074000, length = 0x4000
    DA_Ch7Buf1          : origin = 0xC7078000, length = 0x4000
    DA_Ch8Buf1          : origin = 0xC707C000, length = 0x4000

    /* --- User Buffers --- */
    User_Buf1           : origin = 0x80000000, length = 0x8000
    User_Buf2           : origin = 0x80008000, length = 0x8000
}

SECTIONS
{
    /* --- System Sections --- */
    User_ADEnFile       : > User_ADEn
    User_ADFlagFile     : > User_ADFlag
    User_ADPingPongFile : > User_ADPingPong
    User_ADLenFile      : > User_ADLen

    User_DAEnFile       : > User_DAEn
    User_DAFlagFile     : > User_DAFlag
    User_DAPingPongFile : > User_DAPingPong
    User_DALenFile      : > User_DALen
    User_DASelFile      : > User_DASel

    /* --- AD Sections --- */
    AD_Ch1Buf0_File     : > AD_Ch1Buf0
    AD_Ch2Buf0_File     : > AD_Ch2Buf0
    AD_Ch3Buf0_File     : > AD_Ch3Buf0
    AD_Ch4Buf0_File     : > AD_Ch4Buf0
    AD_Ch5Buf0_File     : > AD_Ch5Buf0
    AD_Ch6Buf0_File     : > AD_Ch6Buf0
    AD_Ch7Buf0_File     : > AD_Ch7Buf0
    AD_Ch8Buf0_File     : > AD_Ch8Buf0

    AD_Ch1Buf1_File     : > AD_Ch1Buf1
    AD_Ch2Buf1_File     : > AD_Ch2Buf1
    AD_Ch3Buf1_File     : > AD_Ch3Buf1
    AD_Ch4Buf1_File     : > AD_Ch4Buf1
    AD_Ch5Buf1_File     : > AD_Ch5Buf1
    AD_Ch6Buf1_File     : > AD_Ch6Buf1
    AD_Ch7Buf1_File     : > AD_Ch7Buf1
    AD_Ch8Buf1_File     : > AD_Ch8Buf1

    /* --- DA Sections --- */
    DA_Ch1Buf0_File     : > DA_Ch1Buf0
    DA_Ch2Buf0_File     : > DA_Ch2Buf0
    DA_Ch3Buf0_File     : > DA_Ch3Buf0
    DA_Ch4Buf0_File     : > DA_Ch4Buf0
    DA_Ch5Buf0_File     : > DA_Ch5Buf0
    DA_Ch6Buf0_File     : > DA_Ch6Buf0
    DA_Ch7Buf0_File     : > DA_Ch7Buf0
    DA_Ch8Buf0_File     : > DA_Ch8Buf0

    DA_Ch1Buf1_File     : > DA_Ch1Buf1
    DA_Ch2Buf1_File     : > DA_Ch2Buf1
    DA_Ch3Buf1_File     : > DA_Ch3Buf1
    DA_Ch4Buf1_File     : > DA_Ch4Buf1
    DA_Ch5Buf1_File     : > DA_Ch5Buf1
    DA_Ch6Buf1_File     : > DA_Ch6Buf1
    DA_Ch7Buf1_File     : > DA_Ch7Buf1
    DA_Ch8Buf1_File     : > DA_Ch8Buf1

    /* --- User Sections --- */
    User_Buf1File       : > User_Buf1
    User_Buf2File       : > User_Buf2
}
