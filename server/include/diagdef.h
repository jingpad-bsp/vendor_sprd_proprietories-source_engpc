#ifndef _DIAGDEF_H_0CA046AA_63EB_4581_8D73_0A07F1EE7A6D_
#define _DIAGDEF_H_0CA046AA_63EB_4581_8D73_0A07F1EE7A6D_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "global_def.h"
//#include "CommonDef.h"
#include "NrDef.h"
#include "C2kDef.h"
#include "LteDef.h"

#pragma pack(push, 4)

#define ZERO_SUBTYPE        ( 0 )
#define ZERO_LENGTH         ( 0 )

/// ASSERT sub type
typedef enum
{
    DUMP_ASSERT_INFO   = 0,
    DUMP_MEM_DATA,
    DUMP_MEM_END,
    DUMP_SME_BUFF_MEM_END,
    DUMP_ALL_ASSERT_INFO_END,   // \t
    DUMP_WCDMA_TRACE_POOL_END,
    DUMP_EXDSP_MEM_END,
    DUMP_PPP_HW_REG_END,
    DUMP_MAX_TYPE
} ASSERT_DATA_TYPE_E;

//
// DIAG command definition
//
typedef enum
{
    // Query commands
    DIAG_SWVER_F                    = 0,  // Information regarding MS software
    DIAG_SOFTKEY_F                  = 1,  // Soft key command
    DIAG_LOG_F                      = 2,
    DIAG_AT_F                       = 3,
    DIAG_GET_SYS_INFO_F             = 4,  // Get system information.
    DIAG_SYSTEM_F                   = 5,
    DIAG_CHIP_TEST_F                = 6,  // Direct chip test operation 
    DIAG_POWER_F                    = 7,  // Power on/off MS
    DIAG_SIM_F                      = 8,  // Sim card command
    DIAG_PS_F                       = 9,
    DIAG_RF_F                       = 10,
    DIAG_MCU_F                      = 11,
    DIAG_CHANGE_MODE_F              = 12, // Set MCU mode: normal, test or calibration
    DIAG_NVITEM_F                   = 13,
    DIAG_AT_CHLSRV_F                = 14, // AT Command from channel server
    DIAG_ADC_F                      = 15, // read ADC result      
    DIAG_PIN_TEST_F                 = 16, // for Auto pin-test      
    DIAG_CURRENT_TEST_F             = 17,
    DIAG_SIM_CARD_TEST_F            = 18, // Test sim card
    DIAG_AUDIO_TEST_F               = 19, // test audio part
    DIAG_KEYPAD_TEST_F              = 20, // test keypad
    DIAG_LCM_TEST_F                 = 21, // test lcd module
    DIAG_VIBRATOR_TEST_F            = 22, // test vibrator
    DIAG_RPC_EFS_F                  = 23,
    DIAG_RPC_DC_F                   = 24,
    DIAG_PRODUCTION_TEST_EXTEND_F   = 25, // Product Test Extend Command.
    DIAG_FORCE_ON_CELLS_F           = 26,
    DIAG_RPC_FFS_F                  = 27,
    DIAG_GET_MCU_MODE_F             = 28, // query mcu mode
    DIAG_CALIBRATION_NOISE_F        = 29, // calibration noise check
    DIAG_POWER_SUPPLY_F             = 30, 
    DIAG_RF_QUERY_F                 = 31,
    DIAG_CALIBRATION                = 32,  
    DIAG_CALIBRATION_BT_F           = 33, // Bluetooth test
    DIAG_TP_F                       = 34, 
    DIAG_GPS_F                      = 35,
    DIAG_TD_NV                      = 36, // DIAG_TD_NV=36, read/write the TD calibration parameter
    DIAG_TV_PARAM                   = 37, // add for TV chip 08-03-07
    DIAG_MODEM_SYSTEM_F             = 39, // Bug 948780, modem system command of platform
    DIAG_READ_ASSERT_F              = 40, // read assert info from NV item.
    DIAG_RF_WCDMA_F                 = 42, // 0x2A WCDMA CALIBRATION 
    DIAG_WCDMA_NV                   = 43, // 0x2B WCDMA NV
    DIAG_WCDMA_NS_TEST              = 44, // 0x2C WCDMA NST
    DIAG_LTE_RF                     = 45, // DIAG_LTE_RF 
    DIAG_LTE_NV                     = 46,
	DIAG_DMR_RF						= 0x2D, // DMR
	DIAG_DMR_NV						= 0x2E, // DMR
    DIAG_LTE_OTA                    = 47,
	DIAG_MODEM_RF_E                 = 48,
    DIAG_BOCA_F				        = 48, //only for BOCA
    DIAG_LAYER1_F                   = 50, // Read or write parameter by Layer1
    DIAG_READ_MAINBUF_F             = 51, // read data of main buffer
    DIAG_MSENSOR_F                  = 52, // msenosr simulator    
    DIAG_SDCARD_TEST_F              = 53, // SD simulator
    DIAG_WIFI_TEST_F                = 54, // wifi test
    DIAG_SIMCARD_F                  = 55, // sim card plug simulator
    DIAG_DEVICE_AUTOTEST_F          = 56, // BB AutoTest(lcd，camera)
    DIAG_READ_TRACE_IMAGE_F         = 57, // @ming.tang add for TraceImage
    DIAG_FM_TEST_F                  = 65, // FM 0x41
	DIAG_TINNO_CMD                  = 85, // will add for guanglu
    DIAG_SENSOR_REG_F               = 90, // sensor register w/r tool
    DIAG_NEW_AUDIOTESTER_F          = 91,
    DIAG_FUN_CALL_F                 = 92,
    DIAG_EXTEND_CMD                 = 93, // 通用DIAG命令扩展，后续都将基于这个DIAG CMD扩展子命令
    DIAG_DIRECT_NV                  = 94,
    DIAG_DIRECT_PHSCHK              = 95,
    DIAG_DIRECT_RDVER		        = 96,
    DIAG_DIRECT_IMEINUM             = 97,
    DIAG_AP_F                       = 98 

} diag_cmd_code_enum_type;

typedef enum
{
    DIAG_PQ                         = 0x01,  // Picture Quality
    // subcmd (uint32): 
    // 0x03: Check Google Attestition Keybox deploy status
    // 0x04: Check Widevine L1 keybox deploy status
    // 0x05: Read  Widevine L1 keybox device id from UE
    DIAG_SECURITY_DEPLOY_VERIFY     = 0x02,  // TEE IFAA or SOTER deploy verify and check
    DIAG_FIXNV_DUMP                 = 0x03,  // Bug 652794 Dump l_fixnv1 binary
    DIAG_TRACE_DUMP                 = 0x04   // Bug 816984 Dump uboot/kernel/android log

} diag_cmd_tool_subtype;   // subtype of DIAG_CMD_TOOL

#define MSG_TYPE_ASSERT             ( 255)
#define PPP_PACKET_R                (0x68)
#define PPP_PACKET_A                (0x9C)   

#define RM_RUNMODE_REQ_COMMAND      (0x01)
#define RM_COMMAND_T                (0xFE)
#define	RM_U0_AS_DIAG               (0x80) // uart0 used as DIAG port(debug)


//#//[cmd] type=0x00 subtype=0xFF
typedef struct  
{
    uint8* lpVersion;

} DIAG_VERSION;

//#//[cmd] type=0x05 subtype=0x10
typedef struct
{
    uint32 nID;

} DIAG_CHIP_ID;


/* Log filter command sets  */
typedef enum
{
    //LOG_GET_INFO,
    LOG_GET_FILTER = 1,
    LOG_SET_FILTER,
    LOG_SET_FLOW_LOG,
    ARM_LOG_ENABLE,
    ARM_LOG_DISABLE,
    DSP_LOG_ENABLE,
    DSP_LOG_DISABLE

} log_cmd_set_enum_type;

//
typedef enum
{
    NVITEM_READ   = 0,          /*0*/
    NVITEM_WRITE,               /*1*/
    NVITEM_PRODUCT_CTRL_READ,   /*2*/
    NVITEM_PRODUCT_CTRL_WRITE,  /*3*/
    NVITEM_PRODUCT_CTRL_ERASE,  /*4*/
    NVITEM_GETLENGTH,           /*5*/
    NVITEM_DELETE,              /*6*/
    NVITEM_USB_AUTO_MODE,       /*7*/

    NVITEM_DUMPFILE   = 0x0A,
    NVITEM_SYNC_FLASH = 0x0B,    // Bug: 462478/467382

    NVITEM_PRODUCT_CTRL_READ_EX   = 0x12,  // Bug779941 
    NVITEM_PRODUCT_CTRL_WRITE_EX  = 0x13   // Bug779941 

} NVITEM_ACCESS_E;

//#//[cmd] type=0x0B subtype=0x00
typedef struct
{
    uint16  SignalCode;
    uint16  Size;
    uint32  mode;
    uint32  addr;           // In request packet, it is the access address                        
    uint32  access_size;    // Number of BYTE, WORD or DWORD according to mode.
                            // In response packet, it is error code.
} DIAG_MCU_READ_T;

typedef struct
{
    uint16  SignalCode;
    uint16  Size;
    uint32  mode;
    uint32  addr;           // In request packet, it is the access address
    uint32  access_size;    // Number of BYTE, WORD or DWORD according to mode.
                            // In response packet, it is error code.
} DIAG_MCU_WRITE_T;

typedef enum
{
    AT_CMD = 0xA000,
    SOFTKEY,  
    SERVING_CELL_INFO,
    MCU_READ,
    MCU_WRITE,
    COMM_MAX_NUM

} COMM_STRCUT_ID_ENUM_T;

//#//[cmd] type=0x01 subtype=0x00
typedef struct
{
    uint16  SignalCode;
    uint16  Size;
    uint32  key_code;  

} COMM_SOFTKEY_T;


typedef enum
{
    DIAG_NORMAL_KEY_MSG_INFO_F,		// both send key down and up msg
    DIAG_KEY_MSG_DOWN_INFO_F,		// only send key down msg
    DIAG_KEY_MSG_UP_INFO_F			// only send key up msg
} softkey_cmd_set_enum_type;

//
//
typedef enum
{
    CURRENT_TEST_STOP                   = 0x00,
    CURRENT_TEST_RF_CLOSED              = 0x01,
    CURRENT_TEST_DEEP_SLEEP             = 0x02,
    CURRENT_TEST_LED_ON                 = 0x03,
    CURRENT_TEST_VIBRATOR_ON            = 0x04,
    CURRENT_TEST_RX_MIN_PEAK_VALUE      = 0x05,
    CURRENT_TEST_TX_MAX_PEAK_VALUE      = 0x06,
    CURRENT_TEST_CHARGING               = 0x07,
    CURRENT_TEST_LED_OFF                = 0x08,
    CURRENT_TEST_VIBRATOR_OFF           = 0x09,
    CURRENT_TEST_DEEP_GET_SLEEP_FLAG    = 0x0A,
    CURRENT_TEST_DEEP_SLEEP_FLAG_ENABLE = 0x0B,
    CURRENT_TEST_DEEP_SLEEP_FLAG_DISABLE= 0x0C,
    CURRENT_TEST_UART_ENABLESLEEP       = 0x0D,
    CURRENT_TEST_POWER_OFF              = 0x0E,// NEWMS00177608
    CURRENT_TEST_DEEP_SLEEP_WAKEUP      = 0x0F // Be careful that the higer 8 bit is used for parameter of how long it sleeps

}CURRENT_TEST_E;

//#//[cmd] type=1E subtype=0x00
typedef struct  
{
    uint32 voltage;

} DIAG_VOLTAGE;

//#//[cmd] type=0x0D subtype=0x02
typedef struct  
{
    uint16 offset;
    uint16 length;

} DIAG_SP05_LOAD_REQ;

//#//[cmd] type=0x0D subtype=0x02
typedef struct  
{
    uint16 offset;
    uint16 length;
    uint8* data;

} DIAG_SP05_LOAD_ACK;

//#//[cmd] type=0x0D subtype=0x12
typedef struct  
{
    uint32 offset;
    uint32 length;

} DIAG_MISCDATA_REQ_HEADER;

//#//[cmd] type=0x0D subtype=0x12
typedef struct  
{
    uint32 offset;
    uint32 length;
    uint8* data;

} DIAG_MISCDATA_LOAD_ACK;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// --------------------------------------      GSM RF CAL & NST     --------------------------------------     

// Layer1 param   
#define L1_WRITE                    (0x00)
#define L1_READ                     (0x01)
#define L1_FLASH                    (0x01)
#define L1_RAM                      (0x00)

typedef struct
{
    uint16 is_nv;	    // 00 01: NV, 00 00: RAM
    uint16 band;	    // GSM Band: CALI_MS_BAND_E
    uint16 type;		// Command type: CALIBRATION_PARAM_E
    uint16 index;	    // Index of NV item
    uint16 length;	    // Length of 'DATA'

} TOOL_L1_DIAG_CALI_PARAM_T;

typedef struct
{
    uint16 parameter_type;	
    uint16 operation_status;	
    uint16 length;		

} DIAG_TOOL_CNF_T;

typedef struct
{
    uint16					gain_ind; 	    // gain index for rx
    uint16					gain_val; 	    // gain value for rx
    uint32 /*DSP_TX_TYPE_E*/data_type;      // data type  for tx
    uint16					sample_couter;  // how much sample, to calculate
    uint8/*BOOLEAN*/		on_off;
    uint16                  coding_scheme;
    uint16                  training_sequence;
    uint16                  puncture_type;  
    uint8/*BOOLEAN*/		is_dcvoltage_meas;

} L1_TX_RX_REQ_T;

/// GSM FDT
typedef struct
{
    uint32 band;                            // Band 
    uint16 arfcn;                           // ARFCN
    uint8  slot_mask;                       // (BIT MAP) which slot of 0-7 is used, (Slot < MAX_USED_SLOT_COUNT)
    uint8  slot_num;                        // How many slots is used (slot_num < MAX_USED_SLOT_COUNT)
    uint16 factor[GSM_MAX_FDT_SLOT];        // Factor table of used slots
    uint16 pcl[GSM_MAX_FDT_SLOT];           // PCL, No used
    uint16 factor_index[GSM_MAX_FDT_SLOT];  // Index of factor table [0-79] 

} L1_FDT_TX_FRAME;

typedef struct
{
    uint32 band;                               // Band 
    uint16 arfcn;                              // ARFCN
    uint8  slot_mask;                          // (BIT MAP) which slot of 0-7 is used, (Slot < MAX_USED_SLOT_COUNT)
    uint8  slot_num;                           // How many slots is used (slot_num < MAX_USED_SLOT_COUNT)
    uint16 gain_value[GSM_MAX_FDT_SLOT];
    uint16 gain_index[GSM_MAX_FDT_SLOT];
    uint32 slot_type [GSM_MAX_FDT_SLOT];

} L1_FDT_RX_FRAME;

typedef struct
{
    uint16 nCmdType;  // og: nCmd
    uint16 nReserv1;  // p1: reserved - Set Rx - 'Dummy' Frame count of normal sync cycle (Default = 21)
    uint16 nReserv2;  // p2: reserved - Set Rx - Sync_timeout (60ms as unit)
    uint16 nReserv3;  // p3: reserved - Set Rx - 'Dummy' Frame count of last sync cycle (Default = 15)
    uint16 nReserv4;  // p4: reserved 
    uint16 nFmCount;  // og: nFrameCount

} L1_FDT_REQ;

typedef struct
{
    uint16 nState;
    uint16 nDataCount;

} L1_FDT_ACK;

// Non-Signal Final Test Command
typedef struct 
{
    uint16 arfcn;      
    uint16 traffic_tn;
    uint16 tsc;
    uint16 power_level;
    uint16 mode;
    uint16 command;    
    uint16 band;  
    uint16 dummy0; // SEBER
    uint16 dummy1; // SEBER

} CALI_L1_RXTXLOOP_PARAM_T;

// SE-BER (Single Ended BER)
typedef struct  
{
    uint32 total_bits;
    uint32 reserved;

} CALI_L1_NST_SEBER_REQ_T;

typedef struct  
{
    uint32 status;      // 1: ready, 0: still testing, others: error code
    uint32 error_bits;  // error bits
    uint32 total_bits;  // total bits, it should be equal to total_bits of CALI_L1_NST_SEBER_REQ_T.

} CALI_L1_NST_SEBER_CNF_T;


//GSM LMT start
typedef  struct 
{
	uint16	nCmd;
	uint16  tsc;
	uint16  LoopMode;           //Loopmode a/b/c
	uint16  nBerMode;          //speech mode ¡¢burst mode
	uint16  nTxCfgNum;
	uint16  nRxCfgNum;
	uint16	reserved[2];
} CALI_L1_LMT_PARAM_T;

typedef struct _tagCALI_L1_LMT_CNF_T
{ 
	uint16  nOperation_status;    // L1_RXTXLOOP_RESULT_E
	uint16  nDataCount;
} CALI_L1_LMT_CNF_T, *LPCALI_L1_LMT_CNF_T;

typedef struct
{
	uint16 adc1;
	uint16 adc0;
	uint16 k;
	uint16 reserved0;
	uint16 c0;
	uint16 c1;
	uint16 reserved1;
	uint16 reserved2;
}L1_TRANSCEIVER_TEMP_T;

//////////////////////////////////////////////////////////////////////////
///

#define CALI_RF_LO_CAL  (0x41)
#define CALI_RF_LO_INIT (0x42)
#define CALI_RF_LO_END  (0x43)

typedef struct
{
	uint32 	rf_ic; 	// 0: RF IC #1 	1: RFIC #2
} TOOL_L1_DIAG_RFIC_T;

typedef struct  
{
	unsigned short flag;
	unsigned short link_path;
	unsigned short gainIndex_TX;
	unsigned short gainIndex_RX;
	unsigned short reg_273;
	unsigned short reg_274;
	unsigned short LNA;
	unsigned short reserved;
}TOOL_L1_DIAG_LO_PARAM_T;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// FM
///
typedef enum
{
    FM_CMD_STATE		= 0x00,	// Open|Close FM
    FM_CMD_VOLUME		= 0x01,	// Set volume
    FM_CMD_MUTE			= 0x02,	// Set volume to mute
    FM_CMD_TUNE			= 0x03,	// Retrieve the signal information of specified frequency
    FM_CMD_SEEK			= 0x04,	// Seek the next valid channel in specified frequency
    FM_CMD_READ_REG		= 0x05,	// Read the FM register
    FM_CMD_WRITE_REG	= 0x06,	// Write the FM register
    FM_CMD_MAX

} FM_SUB_CMD_TYPE_ENUM;

typedef struct
{
    uint8	nOperInd;			// Tune:	0: tune successful; 
                                //			1: tune failure;
                                // Seek:	0: seek out valid channel successful; 
                                //			1: seek out valid channel failure;
    uint8	nStereoInd;			// 0: Stereo; Other: Mono;
    uint16	nRssi;				// RSSI Value
    uint32	nFrequency;			// Frequency, Unit:KHz
    uint32	nPowerIndicator;	// Power indicator
    uint32	nFreqOffset;		// Frequency offset
    uint32	nPilotDet;			// pilot_Det
    uint32	nNoDacLpf;			// No_Dac_lpf

} DIAG_FM_SIGNAL_PARAM_T;

typedef struct  
{
    uint32	nErrorCode;
    uint32	nStartAddr;
    uint32	nUnitCount;

} DIAG_FM_RW_REG_T;

typedef enum
{
    FM_REG_SUCCESS		= 0,
    FM_REG_ADDR_ERR		= 1,
    FM_REG_SIZE_ERR		= 2,
    FM_REG_RANGE_ERR	= 3,
    FM_REG_OTHER_ERR	= 4

} DIAG_FM_REG_ERR_E;

//////////////////////////////////////////////////////////////////////////
/// AP operation
typedef struct 
{
    uint16  cmd;        // DIAG_AP_CMD_E
    uint16  length;     // Length of structure
} TOOLS_DIAG_AP_REQ_T;

// Using for charge current test
typedef struct 
{
    uint16  cmd;        // DIAG_AP_CMD_E
    uint16  length;     // Length of structure
    uint32  current;
} TOOLS_DIAG_AP_REQ_T_V2;

typedef struct  
{
    uint32 on_off;
} TOOLS_DIAG_AP_CHARGE_CMD_T;

typedef struct  
{
    uint32 charging;
    uint32 battery;
} TOOLS_DIAG_AP_CHARGE_CURRENT_T;

typedef struct 
{
    uint16 status;      
    uint16 length;
} TOOLS_DIAG_AP_CNF_T;

typedef struct 
{
    uint32 operate; // 0: Get ADC   1: Load ADC    2: Save ADC 
    uint32 data[12];
} TOOLS_DIAG_AP_ADC_T;

typedef struct 
{
	uint16 nTime;
}TOOLS_DIAG_AP_BKLIGHT_T;

typedef struct 
{
	uint16 nMode;
}TOOLS_DIAG_AP_PWRMODE_T;

typedef enum
{
    DIAG_AP_CMD_ADC              = 0x0001,
    DIAG_AP_CMD_FILE_OPER 	     = 0x0003,		// AP file read/write operation
    DIAG_AP_CMD_SWITCH_CP        = 0x0004,
    DIAG_AP_CMD_BKLIGHT          = 0x0005,
    DIAG_AP_CMD_PWRMODE          = 0x0007,
    DIAG_AP_CMD_PROGRAM_KEY      = 0x0009,
    DIAG_AP_CMD_EXIT_PROGRAM_KEY = 0x000a,
	DIAG_AP_CMD_SYNC             = 0x000b,
	DIAG_AP_CMD_MEM_READ		 = 0x000c,
	DIAG_AP_CMD_MEM_WRITE		 = 0x000d,
    DIAG_AP_CMD_CHARGE_ONOFF     = 0x0010,      // Bug 361963
    DIAG_AP_CMD_CHARGE_CURRENT   = 0x0011,
	DIAG_AP_CMD_GET_MODULE_INFO	 = 0x0012,
    DIAG_AP_CMD_READ_MMI         = 0x0013,
    DIAG_AP_CMD_WRITE_MMI        = 0x0014,
	DIAG_AP_CMD_GET_CPU_TYPE	 = 0x0015,
	DIAG_AP_CMD_GET_ROM_SIZE	 = 0x0016,
	DIAG_AP_CMD_GET_RAM_SIZE	 = 0x0017,
	DIAG_AP_CMD_OTP_IMEI		 = 0x0018,
    DIAG_AP_CMD_TSX_DATA	     = 0x0019,		// 读写TSX数据
	DIAG_AP_CMD_SECURITY_SCHEME	 = 0x001d,
	DIAG_AP_CMD_GET_LCS				= 0x001e,
	DIAG_AP_CMD_SET_RMA				= 0x001f,
	DIAG_AP_CMD_GET_SOCID			= 0x0020,
    DIAG_AP_CMD_GET_SECURESCHEME_FLAG = 0x0021,
    DIAG_AP_CMD_CHARGE_CURRENT_CHANGE = 0x0022,
	DIAG_AP_CMD_TSX_OSC_DATA      	= 0x0023,
	DIAG_AP_CMD_WCN_RAW_DATA      	= 0x0024,
	DIAG_AP_CMD_SET_PMIC_AFC_CDAC	= 0x0025,
	DIAG_AP_CMD_PMIC_AFC_AMP_CALI	= 0x0026,
	DIAG_AP_CMD_LOAD_PMIC_AFC_DATA	= 0x0027,
	DIAG_AP_CMD_SAVE_PMIC_AFC_DATA	= 0x0028,
	DIAG_AP_CMD_SET_PMIC_AFC_MODE	= 0x0029,
    DIAG_AP_CMD_GET_BATTERY_CAPACITY  = 0x0030, // Bug896218电池电量 
    MAX_DIAG_AP_CMD
} DIAG_AP_CMD_E;

typedef struct
{
    uint32 	file_cmd;			// 0: Query file status 1: Read file, 2: Write file
    uint8 	file_name[260];	    // Specifies the file name
} TOOLS_DIAG_AP_FILEOPER_REQ_T;

typedef struct
{
    uint32 file_size;	        // Specifies the queried file size
} TOOLS_DIAG_AP_FILE_STATUS_T; 


#define MAX_DIAG_TRANSMIT_FILE_LEN 			(8192)
typedef struct
{
    uint32 	status; 	// 0: finished,  1: under reading or writing
    uint32 	data_len;   // Specifies the data length to read/write
    uint8 	data[MAX_DIAG_TRANSMIT_FILE_LEN];		// file content 
} TOOLS_DIAG_AP_FILE_DATA_T; 


typedef struct 
{
    uint32  cp_no;
} TOOLS_DIAG_AP_SWITCH_CP_T;

typedef struct
{
	uint16 CDAC;
	uint16 HP_Mode_Amp_26M;
	uint16 LP_Mode_Amp_26M;
	uint16 LP_Mode_Drift_32K;
	uint16 reserved[4];
}TOOLS_DIAG_AP_PMIC_AFC_CALI_DATA_T;

typedef struct
{
	uint16 Amp_26M;
	uint16 reserved;
}TOOLS_DIAG_AP_PMIC_AMP_DATA_T;

typedef struct
{
	uint16 mode; 		//0: low mode; 1:High mode
	uint16 reserved;
}TOOLS_DIAG_AP_PMIC_POWER_MODE_T;

typedef struct
{
	uint16 CDAC; 		
	uint16 reserved;
}TOOLS_DIAG_AP_SET_PMIC_CDAC_T;

// Bug896218
typedef struct 
{
    uint32 capacity;      // 电池百分比，比如32 代表 32%  小数四舍五入
    uint32 reserved;     // 保留
} TOOLS_DIAG_BATTERY_CAPACITY_CNF_T;


//////////////////////////////////////////////////////////////////////////
///                             TDSCDMA
typedef enum
{
	TOOL_MPH_ACTIVE_REQ     = 0xA101,
	TOOL_MPH_DEACTIVE_REQ,
	TOOL_MPH_CLASSMARK_REQ,
	TOOL_MPH_RXPOWER_SWEEP_REQ,
	TOOL_MPH_RXPOWER_SWEEP_CNF,     	     
	TOOL_MPH_BCCH_DECODE_REQ,
	TOOL_MPH_BCCH_INFO_IND,            
	TOOL_MPH_BCCH_STOP_REQ,
	TOOL_MPH_BSIC_DECODE_REQ,     
	TOOL_MPH_BSIC_DECODE_CNF,    
	TOOL_MPH_BSIC_DECODE_FAIL,  
	TOOL_MPH_DSP_MEMORY_WRITE_REQ,
	TOOL_MPH_DSP_MEMORY_READ_REQ,
	TOOL_MPH_DSP_VERSION_REQ,
	TOOL_MPH_DSP_MEMORY_READ_CNF,
	TOOL_MPH_DSP_VERSION_CNF,
	TOOL_MPH_DSP_TXRX_REQ,
	TOOL_MPH_STOP_DSP_TXRX_REQ,
	TOOL_MPH_DSP_RXLEV_CNF,
	TOOL_MPH_DAC_REQ,
	TOOL_MPH_STOP_DAC_REQ,
	TOOL_MPH_AFC_REQ,
	TOOL_MPH_AGC_OFFSET_REQ,
	TOOL_MPH_POWER_RAMP_REQ,
	TOOL_MPH_POWER_STEP_FACTOR_REQ,
	TOOL_MPH_PA_PARA_REQ,
	TOOL_MPH_BATTERY_REQ,
	TOOL_MPH_BATTERY_CNF,
	TOOL_MPH_RF_CONTROL_REQ,
	TOOL_MPH_DSP_RX_REQ,
	TOOL_MPH_STOP_DSP_RX_REQ,	
	TOOL_MPH_DSP_RXLEV_QUERY,
	TOOL_MPH_DSP_TXLEV_QUERY,
	TOOL_MPH_DAC_QUERY,
	TOOL_MPH_CLASSMARK_QUERY,

	//add for new afc algorithm
	TOOL_MPH_DSP_START_TSDATA_REQ,     
	TOOL_MPH_DSP_STOP_TSDATA_REQ,      
	TOOL_MPH_DSP_TSDATA_CFN,            
	TOOL_MPH_DSP_TSDATA_IND,         

	//add for new apc para
	TOOL_MPH_DSP_APC_CONTROL_REQ,
	TOOL_MPH_DSP_APC_SPE_BURST_CONTROL_REQ,

	//add for group apc control
	TOOL_MPH_DSP_APC_GRP_CONTROL_REQ,

	//add for non signaling test
	TOOL_MPH_NONSIGNAL_TD_TEST_REQ,
	TOOL_MPH_NONSIGNAL_TD_TEST_CNF,

	// Change cal. mode
	TOOL_MPH_CALI_G2T_REQ,
	TOOL_MPH_CALI_G2T_CNF,
	TOOL_MPH_CALI_G2T_INIT_REQ,
	TOOL_MPH_CALI_G2T_INIT_CNF,
	TOOL_MPH_FDT_REQ,                  //TD FDT 命令
	TOOL_MPH_FDT_AGC_RESULT_CNF,        //TD RDT agc result

	TOOL_MPH_NONSIGNAL_TD_RSCP_CNF = 0xA13B, // for Td rscp 0xa13b
	TOOL_MPH_NONSIGNAL_TD_BER_CNF, // for Td ber
	/* add for SR3592 TD IRR calibration */
	TOOL_MPH_CALI_TD_IRR_REQ,
	TOOL_MPH_CALI_TD_IRR_CNF,
	//TX RX FDT
	TOOL_MPH_FDT_TX_V2_REQ, 
	TOOL_MPH_FDT_TX_V2_CNF, 
	TOOL_MPH_FDT_RX_V2_REQ,
	TOOL_MPH_FDT_RX_V2_CNF,

} TOOL_MPH_MSG_ID_E;


// Signal head structure.
// Signal vars used when send signals from one task to anther.
// The pointer is a structure whose head is SIGNAL_VARS.

typedef uint32          BLOCK_ID;
#define _SIGNAL_VARS \
	uint16 SignalCode; \
	uint16 SignalSize; \
	xSignalHeader  Pre; \
	xSignalHeader  Suc; \
	BLOCK_ID       Sender;

#ifndef _BSSIM
#define SIGNAL_VARS \
	_SIGNAL_VARS
#else
#define SIGNAL_VARS \
	_SIGNAL_VARS \
	void  *SigP;
#endif // End of _BSSIM

// Signal head structure.
typedef struct xSignalHeaderStruct *xSignalHeader;
typedef struct xSignalHeaderStruct {
	SIGNAL_VARS
} xSignalHeaderRec;

typedef struct
{
	SIGNAL_VARS
} L1_TOOL_MPH_ACTIVE_REQ;

typedef struct
{
	SIGNAL_VARS
} L1_TOOL_MPH_DEACTIVE_REQ;

// TD FDT
#define MAX_CHANNEL_NUM  13
#define MAX_FDT_RX_REPEAT_NUM 5
#define MAX_FDT_AGC_RESULT_NUM  ((MAX_CHANNEL_NUM) * (MAX_FDT_RX_REPEAT_NUM) * (10))  // from zhongping

typedef struct
{
	uint16  nStartFactor;	        
	uint8  	nFram;	                           
	uint8  	cal_apc_step;	            
}L1_ARR_TX_T,*pL1_ARR_TX_T;

#define MAX_MODE_NUMBER 4
typedef struct
{ 
	uint16  	nArfcn;	          
	uint16	    nApcArrNum;    
	L1_ARR_TX_T         ApcTxArr[MAX_MODE_NUMBER]; 
}L1_TD_ARFCN,*pL1_TD_ARFCN;

typedef struct
{
	uint32 nChanelNum;         
	L1_TD_ARFCN Arfcn[MAX_CHANNEL_NUM];	
}L1_TD_FDT_TX_PARAM,*pL1_TD_FDT_TX_PARAM;

typedef struct
{
	SIGNAL_VARS
		uint16      nTrigNum;          
	uint16      nTxFlag;  
	L1_TD_FDT_TX_PARAM txFDTpara;             
}L1_TD_FDT_TX_REQ;

typedef struct 
{
	SIGNAL_VARS
		uint16    nSuccess;//1 = success other fail
} L1_TD_FDT_TX_CNF;

typedef struct
{
	uint16  nChanelNum;          
	uint16  Arfcn[MAX_CHANNEL_NUM];
	uint16	nRxGainNum;        
	uint16  nRxGainData[10];    

}L1_TD_FDT_RX_PRAM,*pL1_TD_FDT_RX_PRAM;

typedef struct
{
	SIGNAL_VARS
		unsigned short      nTrigNum;     
	L1_TD_FDT_RX_PRAM   rxFDTpara;               
}L1_TD_FDT_RX_REQ;

typedef struct
{
	uint16   nArfcn;
	uint16   nRxGain;
	int16    nRssi;
}L1_TD_FDT_AGC_RESULT,*pL1_TD_FDT_AGC_RESULT;

typedef struct
{
	SIGNAL_VARS
	uint16    nSuccess;//1 = success other fail
	uint16   nReultNum;
	L1_TD_FDT_AGC_RESULT     cali_FDT_AGC_result_arr[MAX_FDT_AGC_RESULT_NUM];
} L1_TD_FDT_RX_CNF, *LPL1_TD_FDT_RX_CNF;

typedef struct
{
    SIGNAL_VARS
    uint32         band;           // band
    uint16         arfcn;	        // arfcn 
    uint16         gain_ind;       // gain index for rx
    uint16         gain_val;       // gain value for rx
    uint16         txpwr_level;    // tx power level for tx
    DSP_TX_TYPE_E  data_type;      // data type for tx
    uint16         sample_couter;  // how much sample , to calculate 
    uint16         mode;
} L1_TOOL_MPH_DSP_TXRX_REQ;

typedef struct
{
    SIGNAL_VARS
} L1_TOOL_MPH_STOP_DSP_TXRX_REQ;


typedef struct 
{
	uint16 	nFactor;	
	uint16 	nTxNumber;	        // 表示连续发送的点数

} L1_FACTOR_TX_T;

typedef struct
{
    SIGNAL_VARS

	uint32              freq;			// directly use freq, changed by alex li
	uint16              factor_step; 	// 表示发送功率的步径
	uint16              trigger_level; 
	uint16              factor_number;
	L1_FACTOR_TX_T      factor_arr[8];  // actual max number is 3

} L1_TOOL_TD_MPH_APC_GRP_CONTROL_REQ;


typedef struct 
{
    SIGNAL_VARS
    uint32   is_valid;
    uint16   rxlev;
    int16    rssi;
} L1_TOOL_MPH_DSP_RXLEV_CNF;

#define MAX_AGC_RESULT_NUM   40
typedef struct 
{
    SIGNAL_VARS
    uint32    is_valid;
    uint16    rxlev;
    int16     rssi;
    uint32    rssi_number;
    uint16    rssi_state[MAX_AGC_RESULT_NUM];
    int16     rssi_val[MAX_AGC_RESULT_NUM];
} L1_TOOL_MPH_DSP_AGC_RXLEV_CNF;

// R/W TD NV 
enum
{
	TD_NV_READ = 0,
	TD_NV_WRITE,
	TD_NV_SAVETOFLASH
};

typedef struct  
{   
	uint32 /* TD_NV_TYPE_E */   eNvType;                    // data type
	uint16                      nDataNum;                   // data len	
	uint16                      nData[TD_MAX_NV_DATA_NUM];  // data

} L1_TD_NV_DATA_WRITE_T;

typedef struct	 
{
	uint32 /* TD_NV_TYPE_E */   eNvType;                    // data type
	uint16                      nDataNum;                   // data len

} L1_TD_NV_DATA_READ_T;

/// TD NST

typedef struct
{
	uint32	ul_chan_code[16];
	uint32	dl_chan_code[16];
	uint16  arfcn;
	uint16  cell_param_id;   
	uint16  ul_slot ;
	uint16  ul_midamble_alloc_mode;
	uint16  ul_midamble;
	uint16  ul_midamble_shift ;
	uint16  ul_kcell ;
	uint16  dl_slot ;
	uint16  dl_midamble_alloc_mode;
	uint16  dl_midamble;
	uint16  dl_midamble_shift ;
	uint16  dl_kcell ;
	int8    prx_up_dpch_des;
	uint8   pccpch_tx_power;
	uint8   ul_chan_code_num;
	uint8   dl_chan_code_num;
	uint16  reserved[7];

} L1_TD_NONSIGNAL_REQ_T;


typedef struct
{
	SIGNAL_VARS

		uint32  nonsignal_cmd;
	L1_TD_NONSIGNAL_REQ_T td_param;

} L1_TOOL_MPH_TDNONSIGNAL_PARAM_REQ;

typedef struct 
{
	SIGNAL_VARS

		uint32  nonsignal_cmd;
	uint8 /*BOOLEAN*/ uRet; //0 means success according to tools

} L1_TOOL_MPH_NONSIGNAL_TD_TEST_CNF;

typedef struct
{
	SIGNAL_VARS

		uint16 is_result_valid;
	uint16 rscp_level;
} L1_TOOL_MPH_NONSIGNAL_RSCP_CNF;

typedef struct
{
	SIGNAL_VARS

		uint8  reserver[3];
	uint8  nBer_is_ready;	
	uint16 nBer_bit_total_num;
	uint16 nBer_bit_error_num;

} L1_TOOL_MPH_NONSIGNAL_BER_CNF;


typedef struct  
{
	SIGNAL_VARS
		uint32 reserved;

} L1_TOOL_MPH_CALI_G2T_REQ;

typedef struct  
{
	SIGNAL_VARS
		uint32 is_success;

} L1_TOOL_MPH_CALI_G2T_CNF;

typedef struct  
{
	SIGNAL_VARS
		uint32 reserved;

} L1_TOOL_MPH_CALI_G2T_INIT_REQ;

typedef struct  
{
	SIGNAL_VARS
		uint32 is_success;

} L1_TOOL_MPH_CALI_G2T_INIT_CNF;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                WCDMA
typedef enum
{
    CALI_WCDMA_ACTIVE_MODE_REQ          = 0xA501,
    CALI_WCDMA_DEACTIVE_MODE_REQ        = 0xA502,
    CALI_WCDMA_TXRX_SEQ_REQ             = 0xA505,
    CALI_WCDMA_RX_SEQ_RSSI_REQ          = 0xA506,
    CALI_WCDMA_TX_REQ                   = 0xA507,
    CALI_WCDMA_RX_REQ                   = 0xA508,
    CALI_WCDMA_RX_RSSI_REQ              = 0xA509,
    CALI_WCDMA_CAL_IQ_REQ               = 0xA50A,
    CALI_WCDMA_SET_AFC_REQ              = 0xA50B,
    CALI_WCDMA_TXRX_SEQ_V2_REQ          = 0xA50C,
    CALI_WCDMA_RX_SEQ_RSSI_V2_REQ       = 0xA50D,
    CALI_WCDMA_TX_HDT_REQ               = 0xA50E,
    CALI_WCDMA_TX_HDT_RESULT_REQ        = 0xA50F,
    CALI_WCDMA_RXAGC_TEST_REQ           = 0xA510,
    CALI_WCDMA_RXAGC_RSSI_REQ           = 0xA511,
    CALI_WCDMA_TX_SEQ_HDT_V2_REQ		= 0xA512,
    CALI_WCDMA_SET_DCDC_REQ             = 0xA513,
    CALI_WCDMA_RX_SET_GAIN_REQ          = 0xA514,
    CALI_WCDMA_TX_SET_GAIN_REQ          = 0xA515,
    CALI_WCDMA_TX_READ_HDT_REQ          = 0xA516,
    CALI_WCDMA_TX_DYNAMIC_PWR_V2_REQ    = 0xA517,
    CALI_WCDMA_READ_CONFIG_INFOR        = 0xA518,
    CALI_WCDMA_RX_CAPTURE_DATE_REQ      = 0xA519,
    CALI_WCDMA_RX_READ_CAPTURED_DATE_REQ= 0xA51A,
    CALI_WCDMA_WRITE_CONFIG_INFOR       = 0xA51B,
    CALI_WCDMA_SAVE_CONFIG_INFOR        = 0xA51C,
    CALI_WCDMA_IQ_IMBALANCE_REQ			= 0xA521,
    CALC_WCDMA_DEBUG_COMMAND            = 0xA522,
    CALI_WCDMA_SET_HDT_RANGE_REQ        = 0xA523,
    CALI_WCDMA_TX_SEQ_HDT_V3_REQ		= 0xA524,
    CALI_WCDMA_TX_DYNAMIC_PWR_V3_REQ    = 0xA525,
    CALI_WCDMA_TXRX_SEQ_V4_REQ          = 0xA528,
    CALI_WCDMA_TX_SEQ_HDT_V4_REQ        = 0xA526, 
    CALI_WCDMA_RX_SEQ_RSSI_V4_REQ       = 0xA529,
    CALI_WCDMA_RX_ICI_COMP_REQ          = 0xA52A,
    CALI_WCDMA_RX_RSSI_REQ_V2           = 0xA52E,

	CALI_WCDMA_TXRX_SEQ_V6_REQ          = 0xA531,
	CALI_WCDMA_TX_SEQ_HDT_V6_REQ        = 0xA532, 
	CALI_WCDMA_RX_SEQ_RSSI_V6_REQ       = 0xA533,
	CALI_WCDMA_TX_SEQ_RSSI_V6_REQ       = 0xA534,
	CALI_WCDMA_READ_CONFIG_INFOR_V2     = 0xA535,
	CALI_WCDMA_WRITE_CONFIG_INFOR_V2    = 0xA536,

	CALI_WCDMA_INTCAL_INIT_V1_REQ       = 0xA537,
	CALI_WCDMA_INTCAL_START_V1_REQ      = 0xA538,
	CALI_WCDMA_INTCAL_STOP_V1_REQ       = 0xA539,
    CALI_WCDMA_RX_RSSI_AUS_REQ          = 0xA53A,
	CALI_WCDMA_TX_DEBUG_RSSI_V6_REQ     = 0xA540,
	CALI_WCDMA_TX_SEQ_DEBUG_REQ         = 0xA545,
    CALI_WCDMA_AUTO_BAND_QUERY          = 0xA546,

    WCDMA_NS_TEST_INIT_REQ              = 0xA601,
    WCDMA_NS_TEST_START_REQ             = 0xA602,
    WCDMA_NS_TEST_RECONFIG_REQ          = 0xA603,
    WCDMA_NS_TEST_STOP_REQ              = 0xA604,
    WCDMA_NS_TEST_GET_RSCP              = 0xA606,
    WCDMA_NS_TEST_GET_SEBER             = 0xA607,

	WCDMA_NS_TEST_GET_DIV_RSCP          = 0xA60A,
	WCDMA_NS_TEST_GET_DIVSEBER			= 0xA60B,
	WCDMA_NS_TEST_START_DIVSEBER        = 0xA60C,
	WCDMA_NS_TEST_RECV_DIVSEBER         = 0xA60D,

	WCDMA_FAST_NS_TEST_INIT_REQ	        = 0xA701,
	WCDMA_FAST_NS_TEST_START_REQ	    = 0xA702,
	WCDMA_FAST_NS_TEST_STOP_REQ	        = 0xA703,
	WCDMA_FAST_NS_TEST_GET_RESULT_REQ	= 0xA704,

    


} TOOL_WCDMA_MPH_MSG_ID_E;

typedef struct  
{
    SIGNAL_VARS
    uint32 result;      // RF_OP_RESULT_E

} L1_CALI_WCDMA_RLT_T;

typedef struct 
{
    SIGNAL_VARS
} L1_CALI_WCDMA_REQ_T;


// CALI_WCDMA_TX_REQ
typedef struct
{
    SIGNAL_VARS

    uint16       band;                
    uint16       path;                
    uint16     on_off;             
    uint16     uarfcn;
    uint16       mode;                 
    uint16   cmd_mask;               
    uint16 dcdc_value;
    uint16 gain_index;
    uint16  index_src;            
    uint16     iq_com;

} L1_CALI_WCDMA_TX_REQ_T;

// CALI_WCDMA_SET_AFC_REQ
typedef struct
{
    SIGNAL_VARS

    uint32    type;   // 0: TCXO  1: DCXO
    uint32    cdac;
    uint32    cafc;
    uint32 params3;
    uint32 params4;
    uint32 params5;   

} L1_CALI_WCDMA_SET_AFC_REQ_T;


// CALI_WCDMA_SET_DCDC_REQ
typedef struct
{
    SIGNAL_VARS

    uint32 index_or_value;
    uint32        params1;
    uint32        params2;

} L1_CALI_WCDMA_SET_DCDC_REQ_T;

// CALI_WCDMA_TX_SET_GAIN_REQ
typedef struct
{
    SIGNAL_VARS

    uint16       band;
    uint16       path;      //0: primary 1: diversity
    uint16       mode;      //0: high; 1: mid; 2: low
    uint16   cmd_mask;      //bit 0:hdt; bit 1:dcdc; bit 2: dcdc type
    uint16 dcdc_value;
    uint16 gain_index;
    uint16  index_src;      //0: from nv; 1: from spec
    uint16     iq_com;

} L1_CALI_WCDMA_TX_SET_GAIN_REQ_T;

typedef struct 
{   
    SIGNAL_VARS

    uint32     result;   //0:success 1:fail
    uint32  gain_code;
    uint32  hdt_value;
    uint32 dcdc_value;
    uint32     value2; 
    uint32     value3;
    uint32     value4;  

} L1_CALI_WCDMA_TX_SET_GAIN_RLT_T;

// CALI_WCDMA_SET_HDT_RANGE_REQ
typedef struct
{
    SIGNAL_VARS

    uint32 pwrdect_range;/*0 low,1 mid ,2 highpower*/

} L1_CALI_WCDMA_SET_HDT_RANGE_REQ_T;

// CALI_WCDMA_TX_READ_HDT_REQ
typedef struct
{
    SIGNAL_VARS

    uint32 path;   //0: primary 1: diversity

} L1_CALI_WCDMA_TX_READ_HDT_REQ_T;

typedef struct 
{
    SIGNAL_VARS

    uint32    result;   //0??success 1??fail
    uint32 hdt_value;

} L1_CALI_WCDMA_TX_READ_HDT_RLT_T;


typedef struct 
{
    SIGNAL_VARS

    uint32    result;   
    uint32    band_num;

} L1_CALI_WCDMA_QUERY_BAND_RLT_T;

// CALI_WCDMA_RX_REQ
typedef struct
{
    SIGNAL_VARS

    uint16       band;                  
    uint16       path;                   
    uint16     on_off;                 
    uint16     uarfcn; 
    uint16 gain_index;
    uint16  index_src;           

} L1_CALI_WCDMA_RX_REQ_T;


// CALI_WCDMA_RX_SET_GAIN_REQ
typedef struct
{
    SIGNAL_VARS

    uint16 band;
    uint16 path;      //0: primary 1: diversity
    uint16 gain_index;
    uint16 index_src; //0: from nv; 1: from spec

} L1_CALI_WCDMA_RX_SET_GAIN_REQ_T;

typedef struct 
{   
    SIGNAL_VARS

    uint32 result;
    uint32 gain_code;
    uint32 rssi;
    uint32 value1;
    uint32 value2; 
    uint32 value3;
    uint32 value4;  

} L1_CALI_WCDMA_RX_SET_GAIN_RLT_T;


// CALI_WCDMA_RX_RSSI_REQ
typedef struct
{
    SIGNAL_VARS

    uint32 path;                   

} L1_CALI_WCDMA_RX_RSSI_REQ_T;

typedef struct
{
    SIGNAL_VARS
    uint32  read_count;
    uint32  path;         
} L1_CALI_WCDMA_RX_RSSI_REQ_AUS_T;

typedef struct 
{
    SIGNAL_VARS

    uint32 result;                
    uint32   rssi;

} L1_CALI_WCDMA_RX_RSSI_RLT_T;


// CALI_WCDMA_CAL_IQ_REQ
typedef struct
{
    SIGNAL_VARS

    uint16     path;                 
    uint16 reserved;

} L1_CALI_WCDMA_CAL_IQ_REQ_T;

typedef struct 
{	
    SIGNAL_VARS

    uint32 result;              
    int32   cof_i;   
    int32   cof_q;

} L1_CALI_WCDMA_CAL_IQ_RLT_T;

//////////////////////////////////////////////////////////////////////////
// FDT V2
typedef struct
{
    uint16 start_gain_index;        //tx 起始设置gain index
    uint16  gain_index_step;        //步长
    uint16         step_num;        //step 个数.
    uint16        mode_dcdc;        //[15:8]mode [7:0] dcdc value
    uint16             band;        //band
    uint16           uarfcn;        //freq

} L1_CALI_WCDMA_TX_ENTRY_V2_T;

typedef struct
{
    uint32 seg_num;                 //seg num 
    L1_CALI_WCDMA_TX_ENTRY_V2_T seg[MAX_WCDMA_TX_SEG_NUMBER_V2];

} L1_CALI_WCDMA_TX_SEQ_V2_T;

typedef struct
{
    uint16               dft_mode;           
    uint16         dft_dcdc_value;      
    uint16         dft_gain_index;      
    uint16             dft_iq_com;          
    uint16                   path;               
    uint16    inter_cnt_change_ch; 
    uint16               cmd_mask;           
    uint16                seq_num;      
    L1_CALI_WCDMA_TX_SEQ_V2_T seq[MAX_WCDMA_SEQ_NUM_V2];

} L1_CALI_WCDMA_TX_CONFIG_V2_T;

typedef struct
{
    uint16 gain_index;    // gain index 15:8 for diversity 7:0 for primary
    uint16     uarfcn;

} L1_CALI_WCDMA_RX_ENTRY_V2_T;

typedef struct
{
    uint16 array_num;
    uint16      band;
    L1_CALI_WCDMA_RX_ENTRY_V2_T array[MAX_WCDMA_ENTRY_PER_SEQ_NUM_V2];

} L1_CALI_WCDMA_RX_SEQ_V2_T;

typedef struct
{
    uint16         dft_gain_index;
    uint16    inter_cnt_change_ch;     
    uint16                   path;                  
    uint16                seq_num;       
    L1_CALI_WCDMA_RX_SEQ_V2_T seq[MAX_WCDMA_SEQ_NUM_V2];

} L1_CALI_WCDMA_RX_CONFIG_V2_T;


typedef struct 
{
    SIGNAL_VARS

    uint32 interval;                
    L1_CALI_WCDMA_TX_CONFIG_V2_T tx;
    L1_CALI_WCDMA_RX_CONFIG_V2_T rx;

} L1_CALI_WCDMA_TXRX_SEQ_REQ_V2_T;

typedef struct 
{	
    SIGNAL_VARS
    uint32 result; 

} L1_CALI_WCDMA_TXRX_RLT_V2_T;


typedef struct
{
    uint32 array_num;
    uint32 rssi[MAX_WCDMA_ENTRY_PER_SEQ_NUM_V2]; //31:16 for diversity 15:0 for primary

} L1_CALI_WCDMA_RX_RSSI_SEQ_V2_T;

typedef struct
{
    uint32 seq_num;        
    L1_CALI_WCDMA_RX_RSSI_SEQ_V2_T channel[MAX_WCDMA_SEQ_NUM_V2];

} L1_CALI_WCDMA_RX_RSSI_RESULT_V2_T;

typedef struct
{
    SIGNAL_VARS

    uint32 state;  
    L1_CALI_WCDMA_RX_RSSI_RESULT_V2_T result;

} L1_CALI_WCDMA_RX_SEQ_RSSI_V2_RLT_T;

//FDT V6
typedef struct
{
	uint16 uarfcn_num;
	uint16 uarfcn[20];
	uint16 gain_num;
	uint16 gain_index[10];    // gain index 15:8 for diversity 7:0 for primary
} L1_CALI_WCDMA_RX_ENTRY_V6_T;

typedef struct
{
	uint16 dft_gain_index;
	uint16 path;                    //参见RF_PATH_e定义
	L1_CALI_WCDMA_RX_ENTRY_V6_T array[5];
} L1_CALI_WCDMA_RX_CONFIG_V6_T;

typedef struct
{
	uint16 start_gain_index;       //tx 起始设置gain index    .
	uint8 gain_index_step;        //步长
	uint8 step_num;               //step 个数
	uint8 mode;
	uint8 dcdc;
	uint16 uarfcn;                 //freq
} L1_CALI_WCDMA_TX_ENTRY_V6_T;

typedef struct
{
	uint8 path;
	uint8 dft_mode;            //参见RF_TX_MODE_e定义  
	uint16 dft_dcdc_value;      //默认dcdc值
	uint16 dft_gain_index;      //默认gain index             
	uint16 cmd_mask;           
	uint16 totalcount;    
	uint16 arraycountperband[5];
	L1_CALI_WCDMA_TX_ENTRY_V6_T array[1000];//数量可变
} L1_CALI_WCDMA_TX_CONFIG_V6_T;

typedef struct 
{
	uint32 interval;                //step间隔，单位ms
	uint16 band[5];
	uint16 inter_cnt_change_ch;
	L1_CALI_WCDMA_RX_CONFIG_V6_T rx;
	L1_CALI_WCDMA_TX_CONFIG_V6_T tx;
} L1_CALI_WCDMA_TXRX_SEQ_REQ_DATA;

typedef struct 
{
	SIGNAL_VARS
	L1_CALI_WCDMA_TXRX_SEQ_REQ_DATA data;
} L1_CALI_WCDMA_TXRX_SEQ_V6_REQ_T;

typedef struct 
{	
	SIGNAL_VARS
	uint32 result;   
} L1_CALI_WCDMA_TXRX_SEQ_RLT_V6_T;

typedef struct
{
	uint32 rxrssi_num;
	uint32 rxrssi[1000]; //31:16 for diversity 15:0 for primary //数量可变
} L1_CALI_WCDMA_RX_RSSI_SEQ_V6_T;

typedef struct
{
	SIGNAL_VARS
	uint32 state;  
	L1_CALI_WCDMA_RX_RSSI_SEQ_V6_T data;
} L1_CALI_WCDMA_RX_SEQ_RSSI_RLT_V6_T;

typedef struct
{
	uint32 hdt_num;
	uint16 hdt[1000][3]; //数量可变
}L1_CALI_WCDMA_TX_HDT_SEQ_V6_T;

typedef struct
{
	SIGNAL_VARS
	uint32 state;  
	L1_CALI_WCDMA_TX_HDT_SEQ_V6_T data;
} L1_CALI_WCDMA_TX_HDT_SEQ_RLT_V6_T;

typedef struct
{
	uint32 txdb_num;
	int16  txdbQ16[10000]; //数量可变
}L1_CALI_WCDMA_TX_RSSI_SEQ_V6_T;

typedef struct
{
	SIGNAL_VARS
	uint32 state;  
	L1_CALI_WCDMA_TX_RSSI_SEQ_V6_T data;
} L1_CALI_WCDMA_TX_RSSI_SEQ_RLT_V6_T;


typedef struct
{
	uint8  band;
	uint8  tx_gain_mode;
	uint16 tx_dcdc_index;
	uint16 tx_gain_index;
	uint16 tx_uarfcn;
	uint8  rx_pri_gain_index;
	uint8  rx_pri_gain_index_previous;
	uint8  rx_pri_gain_index_next;
	uint8  reserved;
	uint32 rx_pri_rssi_gain_index;
	uint32 rx_pri_rssi_gain_index_previous; 
	uint32 rx_pri_rssi_gain_index_next; 
	uint16 rx_pri_gain_index_db_delta;
	uint16 rx_pri_gain_code;	
	uint16 rx_pri_gain_code_previous;	
	uint16 rx_pri_gain_code_next;
	int16  tx_pri_rssi_db;
	uint16 reserved1;
}L1_CALI_WCDMA_TX_RSSI_DEBUG_SEG_V6_T;

typedef struct
{
	uint32 array_num;
	L1_CALI_WCDMA_TX_RSSI_DEBUG_SEG_V6_T array[2000]; //数量可变
}L1_CALI_WCDMA_TX_RSSI_DEBUG_V6_T;

typedef struct
{
	SIGNAL_VARS
	uint32 result;
	L1_CALI_WCDMA_TX_RSSI_DEBUG_V6_T data;
}L1_CALI_WCDMA_TX_RSSI_DEBUG_RLT_V6_T;


//WCDMA INTERNAL CAL
typedef struct
{
	uint8  ref_mode;
	uint8  ref_dcdc;
	uint16 ref_gain_index; //tx gain index
	int16  ref_powerQ16; //txpower * 16
	uint16 ref_hdt[3];      //hdet value
} L1_CALI_WCDMA_TX_INTCAL_REF_T;

typedef struct
{
	uint16 band;
	uint16 uarfcn_num;
	uint16 uarfcn[15];    //freq
	uint16 ref_num;  //
	L1_CALI_WCDMA_TX_INTCAL_REF_T refinfo[15][10];
	int16  endpower; //
	uint16 pwrdelta; //
	uint16 idxdelta;
	int16  hdetstartpower;
	int16  hdetendpower;
	int16  maxpowerQ16;  //
	uint16 start_gain_index_num;
	uint16 start_gain_index[1000];//
} L1_CALI_WCDMA_TX_INTCAL_DATA_T;

//req
typedef struct
{
	SIGNAL_VARS
	L1_CALI_WCDMA_TX_INTCAL_DATA_T txdata;
} L1_CALI_WCDMA_INTCAL_START_V1_REQ_T;


//rlt
typedef struct
{
	SIGNAL_VARS
	uint32 result;  
	uint8  message[128];
} L1_CALI_WCDMA_INTCAL_START_V1_RLT_T;

typedef struct
{
	SIGNAL_VARS
		uint32 result;
} L1_CALI_WCDMA_INTCAL_INIT_V1_RLT_T;

typedef struct
{
	SIGNAL_VARS
		uint32 is_save_nv;
} L1_CALI_WCDMA_INTCAL_STOP_V1_REQ_T;

typedef struct
{
	SIGNAL_VARS
		uint32 result;
} L1_CALI_WCDMA_INTCAL_STOP_V1_RLT_T;


typedef struct
{
	SIGNAL_VARS
		uint16 type;
	uint16 reserve1;
	uint16 reserve2;
} L1_CALI_WCDMA_TX_SEQ_DEBUG_PS_REQ_T;

typedef struct
{
	SIGNAL_VARS
	uint16 type;
	uint16 count;
	uint32 length;
	uint8 buffer[20000];
} L1_CALI_WCDMA_TX_SEQ_DEBUG_PS_RLT_T;


// HDT
typedef struct
{
    uint32 array_num;           //seg num 
    L1_CALI_WCDMA_TX_ENTRY_V2_T array[MAX_HDT_ENTRY_PER_SEQ_NUM];

} L1_CALI_WCDMA_TX_HDT_T;

typedef struct 
{
    SIGNAL_VARS
    
    uint16            dft_mode;           
    uint16      dft_dcdc_value;      
    uint16      dft_gain_index;      
    uint16          dft_iq_com;         
    uint16                path;                
    uint16 inter_cnt_change_ch; 
    uint16            cmd_mask;           
    uint16            interval;            
    L1_CALI_WCDMA_TX_HDT_T seq;

} L1_CALI_WCDMA_TX_HDT_REQ_T;

typedef struct 
{	
    SIGNAL_VARS
    uint32 result; 

} L1_CALI_WCDMA_TX_HDT_RLT_T;

typedef struct
{
    uint32 array_num;
    uint32 result[MAX_HDT_ENTRY_PER_SEQ_NUM]; 

} L1_CALI_WCDMA_TX_HDT_RESULT_T;

typedef struct
{
    SIGNAL_VARS
    
    uint32 state;  
    L1_CALI_WCDMA_TX_HDT_RESULT_T result;

} L1_CALI_WCDMA_TX_HDT_RESULT_RLT_T;

typedef struct
{
    uint32   array_num;
    uint16 result[100];

}L1_WCDMA_TX_HDT_SEQ_V2_T;

typedef struct
{
    uint32 seq_num;
    L1_WCDMA_TX_HDT_SEQ_V2_T seq_result[MAX_HDT_SEQ_V2_NUM];

} L1_CALI_WCDMA_TX_HDT_RESULT_V2_T;

typedef struct
{
    SIGNAL_VARS

    uint32 state;  
    L1_CALI_WCDMA_TX_HDT_RESULT_V2_T hdt;

} L1_CALI_WCDMA_TX_HDT_RESULT_RLT_V2_T;

// HDET_V3
// Get three hdet values meanwhile.
typedef struct
{
    uint32 array_num;
    uint16 result[100][3];

} L1_WCDMA_TX_HDT_SEQ_V3_T;

typedef struct
{
    uint32 seq_num;
    L1_WCDMA_TX_HDT_SEQ_V3_T seq_result[MAX_HDT_SEQ_V2_NUM];

} L1_CALI_WCDMA_TX_HDT_RESULT_V3_T;

typedef struct
{
    SIGNAL_VARS
    
    uint32 state;  
    L1_CALI_WCDMA_TX_HDT_RESULT_V3_T hdt;

} L1_CALI_WCDMA_TX_HDT_RESULT_RLT_V3_T;

typedef struct 
{
    SIGNAL_VARS

    uint32 interval;      
    L1_CALI_WCDMA_TX_CONFIG_V2_T tx;

} L1_CALI_WCDMA_TX_DYNAMIC_PWR_REQ_V2_T;

typedef struct
{
    uint32 array_num;
    uint16 result[MAX_HDT_ENTRY_PER_SEQ_V4_NUM][3];

} L1_WCDMA_TX_HDT_SEQ_V4_T;

typedef struct
{
    uint32 seq_num;
    L1_WCDMA_TX_HDT_SEQ_V4_T seq_result[MAX_HDT_SEQ_V4_NUM]; 

} L1_CALI_WCDMA_TX_HDT_RESULT_V4_T;

typedef struct
{
    SIGNAL_VARS
    
    uint32 state;  
    L1_CALI_WCDMA_TX_HDT_RESULT_V4_T hdt;

} L1_CALI_WCDMA_TX_HDT_RESULT_RLT_V4_T;

/// NV
#define WCDMA_NV_READ               (0x00)
#define WCDMA_NV_WRITE              (0x01)
#define WCDMA_NV_SAVE_TO_FLASH      (0x02)
#define WCDMA_NV_SAVE_AFC_TO_FLASH  (0x07)

typedef struct	 
{
    uint16   eNvType;           
    uint16 nDataSize;  

} L1_WCDMA_NV_DATA_READ_T;

typedef struct	 
{
    uint16  eNvType;                 
    uint16  nDataSize;              
    uint8   nData[WCDMA_MAX_NV_DATA_SIZE];  

} L1_WCDMA_NV_DATA_WRITE_T;

typedef struct  
{
    uint32 result;      // RF_OP_RESULT_E

} L1_CALI_WCDMA_SAVETOFLASH_RLT_T;

// CALI_WCDMA_READ_CONFIG_INFOR
#define MAX_WCDMA_DL_NV_SIZE    ( 3000 )
typedef struct
{
    SIGNAL_VARS

    uint32 infor_type;
    uint32  infor_len;
    uint32    params1;
    uint32    params2;

} L1_CALI_WCDMA_READ_CONFIG_INFOR_REQ_T;

typedef struct 
{   
    SIGNAL_VARS

    uint32     result;   
    uint32 infor_type;
    uint32  infor_len;
    uint8  infor_data[MAX_WCDMA_DL_NV_SIZE];

} L1_CALI_WCDMA_READ_CONFIG_INFOR_RLT_T;


// CALI_WCDMA_WRITE_CONFIG_INFOR
typedef struct
{
    SIGNAL_VARS

    uint32  infor_type;
    uint32   infor_len;
    uint8   infor_data[3000];

} L1_CALI_WCDMA_WRITE_CONFIG_INFOR_REQ_T;

typedef struct 
{   
    SIGNAL_VARS

    uint32      result;   
    uint32  infor_type;
    uint32   infor_len;

} L1_CALI_WCDMA_WRITE_CONFIG_INFOR_RLT_T;


// CALI_WCDMA_SAVE_CONFIG_INFOR
typedef struct 
{   
    SIGNAL_VARS;
    uint32 result;   

} L1_CALI_WCDMA_SAVE_CONFIG_INFOR_RLT_T;


// RX IQ
typedef struct
{
    SIGNAL_VARS
    uint32 path;   //0: primary 1: diversity

} L1_CALI_WCDMA_RX_CAPTURE_DATA_REQ_T;

typedef struct 
{
    SIGNAL_VARS

    uint32 result;   //0:success 1:fail
    uint32    len;      //data len

} L1_CALI_WCDMA_RX_CAPTURE_DATA_RLT_T;


#define MAX_WCDMA_CAPTURED_IQ_SIZE      (2000)
typedef struct
{
    SIGNAL_VARS

    uint32    len;   
    uint32 offest;   

} L1_CALI_WCDMA_RX_READ_CAPTURED_DATA_REQ_T;

typedef struct 
{
    SIGNAL_VARS

    uint32 result;   //0:success 1:fail
    uint32    len;      //data len
    uint8    data[MAX_WCDMA_CAPTURED_IQ_SIZE];

} L1_CALI_WCDMA_RX_READ_CAPTURED_DATA_RLT_T;

// IQ Imbalance
typedef struct
{
    SIGNAL_VARS

    uint32 type;   

} L1_CALI_WCDMA_IQ_IMBALANCE_REQ_T;

typedef struct 
{
    SIGNAL_VARS

    uint32     result;
    uint32    res_I2Q;
    int32  Phdeg_best;  

} L1_CALI_WCDMA_IQ_IMBALANCE_RLT_T;

// CALI_WCDMA_DEBUG_COMMAND
typedef enum
{
    DBG_CMD_READ_CALI_VER = 99,
    DBG_CMD_TYPE_INVALID  = 0xFFFF

} L1_CALI_WCDMA_DBG_CMD;

typedef struct  
{
    SIGNAL_VARS

    uint32    type;
    uint32 para[5];

}L1_CALI_WCDMA_DBG_CMD_REQ_T;

typedef struct
{
    SIGNAL_VARS

    uint32  state;
    uint32   type;
    uint32 rlt[5];

}L1_CALI_WCDMA_DBG_CMD_RLT_T;

// CALI_WCDMA_RX_ICI_COMP_REQ
typedef struct
{
    SIGNAL_VARS

    uint16       path;    
    uint16       band; 
    uint16  dl_uarfcn;    
    uint16 gain_index;

} L1_CALI_WCDMA_RX_ICI_COMP_REQ_T;

typedef struct 
{
    SIGNAL_VARS

    uint32          result; 
    uint32            rssi;
    uint32  real_coeff[33];
    uint32 image_coeff[33];

} L1_CALI_WCDMA_RX_ICI_COMP_RLT_T;


// CALC_WCDMA_DEBUG_COMMAND
typedef struct
{
    SIGNAL_VARS

    uint32     type;   
    uint32 param[5]; 

} L1_CALI_WCDMA_DEBUG_COMMAND_REQ_T;

typedef struct 
{
    SIGNAL_VARS
    uint32   result; //0：success； 1：fail
    uint32     type;   
    uint32 param[5]; 

} L1_CALI_WCDMA_DEBUG_COMMAND_RLT_T;


// WCDMA NST

typedef struct 
{	
    SIGNAL_VARS
    uint32 result;  

} L1_WCDMA_NS_TEST_INIT_RLT_T;


typedef struct
{
    uint32 dummy;

} WNS_Dummy_t;

typedef WNS_Dummy_t WNS_InitMode_t;
typedef WNS_Dummy_t WNS_StopTest_t;


typedef struct
{
    uint32 DL_UARFCN;
    uint32 Band;
    uint32 Slot_Format;
    uint32 PGC;
    uint32 Chnl_Code;
    uint32 TPC_Alg;
    uint32 TPC_Step_Size;
    uint32 Scrambling_Code_Num;
    uint32 Reseved[92];

} WNS_StartTest_t;

typedef struct
{
    uint32 Slot_Format;
    uint32 Chnl_Code;
    uint32 TPC_Alg;
    uint32 TPC_Step_Size;
    uint32 Scrambling_Code_Num;
	uint32 TPC_Type;
	int32  Power;
    uint32 Reseved[93];

} WNS_Reconfig_t;

typedef struct 
{
    SIGNAL_VARS

    WNS_StartTest_t start;

} L1_WCDMA_NS_TEST_START_REQ_T;

typedef struct 
{	
    SIGNAL_VARS

    uint32 result;   

} L1_WCDMA_NS_TEST_START_RLT_T;

typedef struct 
{
    SIGNAL_VARS
    WNS_Reconfig_t reconfig;

} L1_WCDMA_NS_TEST_RECONFIG_REQ_T;

typedef struct 
{	
    SIGNAL_VARS

    uint32 result;   

} L1_WCDMA_NS_TEST_RECONFIG_RLT_T;


typedef struct 
{
    SIGNAL_VARS

    WNS_StopTest_t stop;

} L1_WCDMA_NS_TEST_STOP_REQ_T;

typedef struct 
{	
    SIGNAL_VARS

    uint32 result;  

} L1_WCDMA_NS_TEST_STOP_RLT_T;

typedef struct 
{
    SIGNAL_VARS

    uint32  total_bits;

} L1_WCDMA_NS_TEST_SEBER_REQ_T;

typedef struct 
{	
    SIGNAL_VARS

    uint32    result;
    uint32 fail_bits;

} L1_WCDMA_NS_TEST_SEBER_RLT_T;

typedef struct 
{	
    SIGNAL_VARS

    uint32 result; 
    int32    rscp; 
    uint32   rssi;

} L1_WCDMA_NS_TEST_RSCP_RLT_T;

typedef struct 
{	
    SIGNAL_VARS

    uint32   result; 
    int32      rscp; 
    uint32     rssi;
    int32  rscp_div; 
    uint32 rssi_div;

} L1_WCDMA_NS_TEST_RSCP_RLT_V2_T;

/// WCDMA LMT
typedef struct
{
	uint32 DL_UARFCN;
	uint32 Band;
	uint32 seg_length;
	uint32 tx_offset;
	uint32 tx_length;
	int32  tx_power_Q16;
	uint32 total_SeBer_num;
	uint32 measure_type_mask;
} W_FAST_NS_Star_Seg_t;

typedef struct
{
	uint32 segment_num;
	uint32 Slot_Format;
	uint32 PGC;
	uint32 Scrambling_Code_Num;
	uint32 Chnl_Code;
	uint32 TPC_Alg;
	uint32 TPC_Step_Size;
	W_FAST_NS_Star_Seg_t seg[MAX_WCDMA_LMT_SEG_NUMBER];
}W_FAST_NS_Star_t;

typedef struct
{
	uint32 req_type;
	uint32 reserved[10];
} W_FAST_NS_Get_Result_Req_t;


typedef struct
{
	uint32 res_type;
	uint32 length;
	uint32 data[MAX_WCDMA_LMT_SEG_NUMBER][MAX_WCDMA_LMT_RLT_DATA_PER_SEG];
}  W_FAST_NS_Get_Result_Res_t;

typedef struct
{
	SIGNAL_VARS
	uint32 result;
} L1_CALI_WCDMA_FAST_NS_INIT_RLT_T;


typedef struct
{
	SIGNAL_VARS
	W_FAST_NS_Star_t data; 
} L1_CALI_WCDMA_FAST_NS_START_REQ_T;

typedef struct
{
	SIGNAL_VARS
	uint32 result;
} L1_CALI_WCDMA_FAST_NS_START_RLT_T;

typedef struct
{
	SIGNAL_VARS
	uint32 result;
} L1_CALI_WCDMA_FAST_NS_STOP_RLT_T;

typedef struct
{
	SIGNAL_VARS
	W_FAST_NS_Get_Result_Req_t data; 
} L1_CALI_WCDMA_FAST_NS_GETRLT_REQ_T;

typedef struct
{
	SIGNAL_VARS
	uint32 result; 
	W_FAST_NS_Get_Result_Res_t data; 
} L1_CALI_WCDMA_FAST_NS_GETRLT_RLT_T;

///LTE
///define
#define MAX_L1_LTE_APC_CHANNEL_NUM     (40) 
#define MAX_L1_LTE_APC_APT_NUM         (10)

#define MAX_L1_LTE_AGC_GAIN_NUM        (10)
#define MAX_L1_LTE_AGC_RSSI_NUM        (800)

#define MAX_L1_LTE_IRR_REG_NUMBER      (200)

#define MAX_L1_LTE_PDT_CHANNEL_NUM     (90)
#define MAX_L1_LTE_PDT_POINT_NUM       (6)
#define MAX_L1_LTE_PDT_VALUE_NUM       (1620)

#define MAX_L1_LTE_TX_DC_ARFCN_NUM     (20)

#define MAX_LTE_L1_NV_LEN              (18200)
#define MAX_LTE_L1_NV_LEN_V2           (18200)
#define MAX_LTE_L1_NV_LEN_V3           (4000)

#define MAX_L1_LTE_NST_ARFCN_NUM       (20)
#define MAX_L1_LTE_NST_TX_NUM          (20)

typedef enum
{
    TOOL_MPH_LTE_DSP_DEACTIVE       = 0xA102,
    TOOL_MPH_LTE_DSP_ACTIVE         = 0xA136,

    /// LTE FDT
    TOOL_MPH_LTE_AFC_FDT_REQ        = 0xA13D,   
    TOOL_MPH_LTE_APC_FDT_REQ        = 0xA13E,
    TOOL_MPH_LTE_AGC_FDT_REQ        = 0xA13F,

    /// LTE NST
    TOOL_MPH_LTE_NST_INIT           = 0xA140,
    TOOL_MPH_LTE_NST_SYNC           = 0xA141,
    TOOL_MPH_LTE_NST_START_TEST     = 0xA142,
    TOOL_MPH_LTE_NST_GET_BLER       = 0xA143,
    TOOL_MPH_LTE_NST_END            = 0xA144,

    /// IRR
    TOOL_MPH_LTE_IRR_GAIN_IMBALANCE = 0xA145,
    TOOL_MPH_LTE_IRR_PHASE          = 0xA146,
    TOOL_MPH_LTE_PDT                = 0xA147,
    TOOL_MPH_LTE_SWITCH             = 0xA148,
	TOOL_MPH_MGB_EG_SET				= 0XA14A,
    TOOL_MPH_LTE_NST_GET_RSSI       = 0xA14A,
    TOOL_MPH_LTE_FT_IRR             = 0xA14B,
    TOOL_MPH_LTE_DC_CAL             = 0xA14C,
    TOOL_MPH_LTE_TRX                = 0xA14D,
    TOOL_MPH_LTE_TRX_GET_RSSI       = 0xA14F,
	TOOL_MPH_LTE_MGB_LO_INIT		= 0xA14F,
	TOOL_MPH_LTE_MGB_LO				= 0xA150,
    TOOL_MPH_LTE_APC_V2             = 0xA150,
	TOOL_MPH_LTE_MGB_LO_END			= 0xA151,
    TOOL_MPH_LTE_AGC_V2				= 0xA151,
    TOOL_MPH_LTE_READ_CAPA,
    TOOL_MPH_LTE_RX_DC,
    TOOL_MPH_LTE_READ_SPI,
    TOOL_MPH_LTE_AFC_DL,
    TOOL_MPH_LTE_PDT_V2,
    TOOL_MPH_LTE_IQ_DATA_LEN,
    TOOL_MPH_LTE_IQ_DATA,
    TOOL_MPH_LTE_GET_TRANSCEIVERTEMP   = 0xA159,
    //TOOL_MPH_LTE_NST_START_TEST_V2     = 0xA160,  // 此ID还未用 [6/26/2017 jian.zhong]
    // B66开始信道超过uint16，需要改成uint32 [5/11/2017 jian.zhong]
    TOOL_MPH_LTE_NST_SYNC_V2       	= 0xA15A,
    TOOL_MPH_LTE_FT_IRR_V2			= 0xA15B,  // 目前已经是32位 [5/11/2017 jian.zhong]
    TOOL_MPH_LTE_TRX_V2				= 0xA15C,  //Pandora使用，PhoneCommand待合入
    TOOL_MPH_LTE_APC_V3             = 0xA15D,
    TOOL_MPH_LTE_AGC_V3             = 0xA15E,
    TOOL_MPH_LTE_AFC_DL_V2         	= 0xA15F,   // 9820 Lte only项目，暂时不合入 [5/11/2017 jian.zhong]
    TOOL_MPH_LTE_PDT_V3             = 0xA160,
    TOOL_MPH_LTE_NST_START_TEST_V2  = 0xA161,
    TOOL_MPH_LTE_UL_AFC             = 0xA162,   //LTE Only UL

	CALI_FDIQ_CALI					= 0xA167,
	CALI_PA_DROOP_CALI				= 0xA168,
    //Internal
    TOOL_MPH_LTE_INTERNAL_APC_START = 0xB000,
    TOOL_MPH_LTE_INTERNAL_APC_RUN,
    TOOL_MPH_LTE_INTERNAL_APC_STOP,
    TOOL_MPH_LTE_INTERNAL_APC_RSP,
} TOOL_MPH_LTE_MSG_ID_E;

typedef struct
{
    SIGNAL_VARS
} L1_LTE_EMPTY_REQ_T;

typedef struct
{
    SIGNAL_VARS
} L1_LTE_EMPTY_RLT_T;

#define  L1_LTE_SUB_FRAME_NUM 10

typedef struct 
{
    uint16 TxWord;
    uint8 PaMode;
    uint8 Apt;
    uint8 RbNumber;
    uint8 RbPosition; 
    uint8 MODULATIONS; // 1:BPSK 2:QPSK 4:QAM16 6:QAM64
    uint8 subframe[L1_LTE_SUB_FRAME_NUM]; //reserved
}L1_LTE_TX_Always_ON;

typedef struct 
{
    uint16 AGC_index;
    uint8 subframe[L1_LTE_SUB_FRAME_NUM]; //reserved
}L1_LTE_RX_Always_ON;
typedef struct 
{
    SIGNAL_VARS

    uint16    Arfcn;
    uint8     BandIndicator;
    uint8     cc;
    uint8     ant;  //reserved;
    uint8     Mode;	  // 0:off 1:Tx 2:Rx 3:RTX
    uint8     Bw;
    L1_LTE_TX_Always_ON    Tx_Config;
    L1_LTE_RX_Always_ON    Rx_Config;
}L1_LTE_TRX_Always_ON;

typedef struct
{
    SIGNAL_VARS

    uint32 nChannel;  
    uint32  eBW;  
    uint16 nMainOrDiv;   
    uint16 nFactorStep;  
    uint16 nAfcVal;      
    uint16 nPaMode;  
    uint16 nApt_value;   
    uint16 nCdac;    
    uint16 Modulation;  
    uint16 RbPos;   
    uint16 RbNum;
    uint16 nTxNum; 
    uint32 reserved[4];
}L1_LTE_CALI_UL_AFC_T;


typedef struct
{	  
    uint16 read_num; //当 mode为read all时，该参数表示读取寄存器的个数。
    uint16 SPI_ADDR; //表示读取起始的寄存器
} L1_LTE_SPI_read_req;


typedef struct
{
    uint16 SPI_ADDR;
    uint16 SPI_DATA;
} L1_LTE_SPI_write;

typedef struct
{
    SIGNAL_VARS

    uint16              Subframe; //0~9表示对应subframe, 0xa表示 all subframe
    uint16              mode; //0:read one SPI; 1:read all SPI; 2:write one SPI
    L1_LTE_SPI_read_req SPI_read;
    L1_LTE_SPI_write    SPI_write;
} L1_LTE_SPI;

typedef struct
{	  
    SIGNAL_VARS

    uint16 status;
    uint16 SPI_DATA[1024];
} L1_LTE_SPI_read_resp;

typedef struct 
{
    SIGNAL_VARS

    uint32    Arfcn;
    uint8     BandIndicator;
    uint8     cc;
    uint8     ant;  //reserved;
    uint8     Mode;	  // 0:off 1:Tx 2:Rx 3:RTX
    uint8     Bw;
    L1_LTE_TX_Always_ON    Tx_Config;
    L1_LTE_RX_Always_ON    Rx_Config;
}L1_LTE_TRX_Always_ON_V2;

typedef struct
{
    SIGNAL_VARS

    uint32 BW;
    uint16 nMainOrDiv;
    uint16 nArfcn;
    uint16 nPa_mode;
    uint16 nApt_value;
    uint16 nAfc_value;
    uint16 nGainFactor;
    int32  reserved[4];
} L1_LTE_AFC_CW_REQ_T;


typedef struct
{
    uint16  nArfcn;                 //channel
    uint16  indicator;              // Band Indicator
    uint16  nOffset;                // freq Offset(Hz)
    uint16  afcType;                // Crystal Type
    uint16  RxWord;                 // RX word
    uint16  CdacMin;                //  Dcxo Min Cdac
    uint16  CadcMax;                //  Dcxo Max Cdac
    uint16  Afc0;                   //  the Cafc word when we Calibrate CDAC;
    uint16  Afc1;                   //   AFC1        
    uint16  Afc2;                   //   AFC2       
    uint16  Reserved[2];             //
} L1_LTE_AFC_DL_T;

typedef struct
{  
    SIGNAL_VARS  
    L1_LTE_AFC_DL_T lteAfcDL;
}L1_LTE_AFC_DL_REQ;

typedef struct 
{
    SIGNAL_VARS
    uint32     Rssi;                   // RSSi 
    uint16     Cdac;                   // Dcxo校准后的值
    uint16     Cafc;                   //  afc校准后的值
    uint32     FreqError;			   // Cafc校准之后的频偏值
    uint16     Slope;                  // 计算之后的斜率
    uint16     reserved;   
}L1_LTE_AFC_DL_CNF;

// TOOL_MPH_LTE_AGC_FDT_REQ
typedef struct
{ 
    uint16 nArfcn;	           
    uint16 nAgcArrNum;       
    uint16 nAgcRxArr[MAX_L1_LTE_AGC_GAIN_NUM];      
} L1_LTE_FDT_RX_ARFCN_T;

typedef struct 
{  
    SIGNAL_VARS  

    uint32                  nChannelNum;     
    uint32                  BW;                
    uint16                  nTrig;   
    uint16                  nAfcVal;
    L1_LTE_FDT_RX_ARFCN_T   AFRCN[MAX_L1_LTE_APC_CHANNEL_NUM];	
    uint16                  afc1;
    uint16                  afc2;
    int32                   reserved[3];
    uint8                   BandIndicator[MAX_L1_LTE_APC_CHANNEL_NUM];

} L1_LTE_FDT_RX_REQ_T;

typedef struct 
{
    SIGNAL_VARS
    
    uint32 nChannelNum;
    int32  RSSI[MAX_L1_LTE_AGC_RSSI_NUM];	
    int32  FreqDelta[2];
    int32  reserved[2];

} L1_LTE_FDT_RX_RLT_T;

// TOOL_MPH_LTE_APC_FDT_REQ
typedef struct
{
    uint16 nStartFactor;
    uint16 nApt_value;  
    uint16 nPa_mode;    
    uint16 nTxNum;

} L1_LTE_FDT_TX_CONFIG_T;

typedef struct
{
    uint16                  nArfcn;
    uint16                  nTxArryNum;
    L1_LTE_FDT_TX_CONFIG_T  TX_config[MAX_L1_LTE_APC_APT_NUM];
} L1_LTE_FDT_TX_ARFCN_T;

typedef struct
{
    SIGNAL_VARS

    uint32                  BW;
    uint16                  nMainOrDiv;
    uint16                  nFactorStep;
    uint16                  nAfcVal;
    uint16                  nChanNum;
    L1_LTE_FDT_TX_ARFCN_T   ArfcnGroup[MAX_L1_LTE_APC_CHANNEL_NUM];
    int32                   reserved[4];
    uint8                   BandIndicator[MAX_L1_LTE_APC_CHANNEL_NUM];
} L1_LTE_FDT_TX_REQ_T;

// Apc V2
typedef struct
{
    uint8  HeaderSize;
    uint8  Version;
    uint8  ChannelSize;
    uint8  ChannelCount;
    uint16 ChannelOffset;
    uint8  Bw;
    uint8  AptSize;
    uint16 AptCount;
    uint16 AptOffset;
    uint16 PointCount;
    uint8  Ca;
    uint8  Ant;
    uint8  Internal;
}L1_LTE_APC_V2_HEADER_T;

typedef struct
{
    uint16 usArfcn;
    uint8 Indicator;
    uint8 AptCount;
}L1_LTE_APC_V2_CHANNEL_T;

typedef struct
{
    uint8 Voltage : 6;
    uint8 PaMode : 2;
    uint8 StepCount;
    uint16 StepLength : 6;
    uint16 BeginWord : 10;
}L1_LTE_APC_V2_APT_T;

typedef struct
{
    uint8 Voltage : 6;
    uint8 PaMode : 2;
    uint8 StepCount;
    uint16 StepLength : 6;
    uint16 BeginWord : 10;
    uint16 RxIndex : 7;
    uint16 Position : 1;
    uint16 Reserved : 8;
}L1_LTE_APC_V2_APT_INTERNAL_T;

// IRR
typedef struct 
{  
    SIGNAL_VARS

    uint8  BW;
    uint8  Ant;
    uint16 RxWord;
    uint16 Arfcn;
    uint16 AfcValue;
} L1_LTE_IRR_GAIN_IMBALANCE_REQ_T;

typedef struct 
{  
    SIGNAL_VARS  

    uint16 IQ;
    uint16 I;
} L1_LTE_IRR_GAIN_IMBALANCE_RLT_T;


typedef struct 
{    
    SIGNAL_VARS

    uint16 BW;
    uint16 StepCount;
    uint16 RxWordIndex[2];
    uint16 CellPower[2];
    uint32 Arfcn;
    uint32 BandFlag;
} L1_LTE_IRR_TUNING_REQ_T;

typedef struct
{
    uint16 iAlphaAmp;    
    int16  iSlopePhase;   
    int16  iShiftPhase;    

} L1_LTE_IRR_RESULT_T;

typedef struct 
{  
    uint8 AfcCount;
    L1_LTE_IRR_RESULT_T Arfcn[2];
} L1_LTE_IRR_TUNING_RLT_DATA_T;

typedef struct 
{  
    SIGNAL_VARS;
    
    L1_LTE_IRR_TUNING_RLT_DATA_T Data;
} L1_LTE_IRR_TUNING_RLT_T;

typedef struct 
{  
    SIGNAL_VARS

    uint8  BW;
    uint8  Ant;
    uint16 Arfcn;
    uint16 RxWord;
    uint16 RegNum;
    uint8  SignalPosition;
    uint16 AfcValue;
    uint16 Reg1[MAX_L1_LTE_IRR_REG_NUMBER];
    uint16 Reg2[MAX_L1_LTE_IRR_REG_NUMBER];
} L1_LTE_IRR_PHASE_REQ_T;

typedef struct 
{  
    SIGNAL_VARS

    uint32 nCount;
    uint16 RSSI[MAX_L1_LTE_IRR_REG_NUMBER];
} L1_LTE_IRR_PHASE_RLT_T;

//Fdiq
typedef struct
{
	uint16 nPriRxGainWord;
	uint16 nDivRxGainWord;
}L1_LTE_FDIQ_RXWord;

typedef struct
{
	uint16 BW;
	uint16 nFreqGap;
	uint32 nBandFlag;
	L1_LTE_FDIQ_RXWord RxGainWord[3];
	uint32 Arfcn;

}L1_LTE_FDIQ_RXWord_Band;

typedef struct 
{    
	SIGNAL_VARS

	uint32 nBandNum;
	L1_LTE_FDIQ_RXWord_Band RxGainWordBand[10];
	uint32 nOffsetTime;
} L1_LTE_FDIQ_REQ;

typedef struct
{
	int16 P[3];
	int16 G[3];
	int16 K[3];
	int16 C[3][9];
}L1_LTE_FDIQ_RSP_PGK;

typedef struct
{
	L1_LTE_FDIQ_RSP_PGK stBandRsp[2];
}L1_LTE_FDIQ_RSP_MBAND_PGK;

typedef struct 
{  
	SIGNAL_VARS  
		L1_LTE_FDIQ_RSP_MBAND_PGK stRsp[10];
} L1_LTE_FDIQ_RSP;

// Power Detect
typedef struct
{
    uint16 Arfcn;           
    uint8  PointNumber;    
    uint8  RbNumber;       
    uint8  RbPosition;     
    uint8  Apt;             
    uint8  PaMode;           
    uint16 ControlWord[MAX_L1_LTE_PDT_POINT_NUM];
} L1_LTE_PDT_CHANNEL_T;

typedef struct
{
    SIGNAL_VARS

    uint8   ChannelNumber;            
    uint8   BW;                      
    uint16  VctcxoAfc;                
    uint8   Range;                   
    L1_LTE_PDT_CHANNEL_T ChannelConfig[MAX_L1_LTE_PDT_CHANNEL_NUM];  
    uint8   BandIndicator[MAX_L1_LTE_PDT_CHANNEL_NUM];
} L1_LTE_PDT_REQ_T;

typedef struct
{
    SIGNAL_VARS

    uint16 RetNumber; 
    uint16 Value[MAX_L1_LTE_PDT_VALUE_NUM];
    uint16 reserved;
} L1_LTE_PDT_RLT;

typedef struct 
{  
    SIGNAL_VARS  

    uint16 type; 
    uint16 size;
} L1_LTE_SWITCH_REQ_T;

// DC
typedef struct
{  
    uint16 ARFCN;
    uint8  BandIndicator;
    int16  StepBegin;
    int16  StepLength;  
    uint16 StepNumber; 
    uint8  RbNumber;
    uint8  RbPosition;     
    uint8  LoopItem;      
    int16  FixedItemValue;
    uint8  PaMode;
    uint8  Apt;
    uint16 ControlWord;
} L1_LTE_TX_DC_ARFCN_T;

typedef struct
{
    SIGNAL_VARS  

    uint8 ChannelNumber;
    uint8 BW;
    L1_LTE_TX_DC_ARFCN_T ChannelConfig[MAX_L1_LTE_TX_DC_ARFCN_NUM];
} L1_LTE_TX_DC_REQ_T;

enum LTE_NV_CMD
{
    LTE_NV_READ = 0,
    LTE_NV_WRITE,
    LTE_NV_SAVETOFLASH,
    LTE_NV_READ_V2,
    LTE_NV_WRITE_V2,
    LTE_NV_READ_V3,
    LTE_NV_WRITE_V3
};

typedef struct
{   
    uint32 eNvType;       
    uint16 nDataCount;     
    uint16 nData[MAX_LTE_L1_NV_LEN];   
} L1_LTE_NV_WRITE_T;	

typedef struct
{
    uint32 eNvType;       
    uint16 nDataCount;  
} L1_LTE_NV_READ_T;

// V2
typedef struct 
{
    uint16     isPartion;	
    uint16     pationNum;	
    uint16 pationLen[10];	
    uint16          band;		
    uint16 BandDataCount;  
    uint16    reserve[2];
} L1_LTE_NV_V2_HEADINFO_T;

typedef struct
{   
    uint32 eNvType;        
    uint16 nBandNum;
    uint16 reserve;
    L1_LTE_NV_V2_HEADINFO_T  headinfo[20];
    uint16 nData[MAX_LTE_L1_NV_LEN]; 
} L1_LTE_NV_V2_WRITE_T;	

typedef struct
{
    uint32 eNvType;       
    uint16 nBandNum;
    uint16 reserve;
    L1_LTE_NV_V2_HEADINFO_T headinfo[20]; 
} L1_LTE_NV_V2_READ_T;

//V3
typedef struct
{
    int32  Type;
    uint16 Position;
    uint8  Band;
    uint8  Indicator;
}L1_LTE_NV_V3_READ_REQ_T;

typedef struct
{
    uint16 Size;
    uint16 Position;
    uint16 nData[MAX_LTE_L1_NV_LEN_V3];
}L1_LTE_NV_V3_READ_RLT_T;

typedef struct
{
    int32  Type;
    uint16 Position;
    uint8  Band;
    uint8  Indicator;
    uint16 nData[MAX_LTE_L1_NV_LEN_V3];
}L1_LTE_NV_V3_WRITE_REQ_T;

// LMT
// TOOL_MPH_LTE_NST_SYNC
typedef struct  
{
    SIGNAL_VARS

    uint16 Cell_id;                
    uint16 Arfcn;                  
    uint8  RB_num;                 
    uint8  RB_pos;                
    uint8  BW;                   
    uint8  MCS;                    
    uint8  TDD_frameconfig;     
    uint8  reserved;
    uint16 CellPower;            
    uint16 RTNI;                   
    uint16 redundancy_version;     
    uint16 precoding_information;  
    uint16 Ant;
} L1_LTE_NST_SYNC_REQ_T;

typedef struct  
{
    SIGNAL_VARS

    uint16 Cell_id;                
    uint32 Arfcn;                  
    uint8  RB_num;                 
    uint8  RB_pos;                
    uint8  BW;                   
    uint8  MCS;                    
    uint8  TDD_frameconfig;     
    uint8  reserved;
    uint16 CellPower;            
    uint16 RTNI;                   
    uint16 redundancy_version;     
    uint16 precoding_information;  
    uint16 Ant;
} L1_LTE_NST_SYNC_V2_REQ_T;

typedef struct  
{
    SIGNAL_VARS

    uint32 status;      // LTE_NST_STATUS_E
} L1_LTE_NST_SYNC_RLT_T;


// TOOL_MPH_LTE_NST_START_TEST
typedef struct
{
    uint8   frame_num; 
    uint16  CellPower;    
} L1_LTE_NST_RX_CONFIG_T;

typedef struct
{
    uint8 frame_num;       
    uint8 RB_num;            
    uint8 RB_pos;            
    uint8 TPC;               
    int8  closeloop_power;   
    uint8 BW;                
    uint8 MCS;              
    uint8 reserved;  //LMT 最大功率校准时，TDD需要将PA设置成常发。只在功率校准时生效;
} L1_LTE_NST_TX_CONFIG_T;

typedef struct
{
    uint16 arfcn;                        
    uint16 frame_num;                     
    uint8  tx_count;                       
    L1_LTE_NST_RX_CONFIG_T rx;                            
    L1_LTE_NST_TX_CONFIG_T tx[MAX_L1_LTE_NST_TX_NUM];         

} L1_LTE_NST_ARFCN_CONFIG_T;

typedef struct
{
    SIGNAL_VARS

    uint16 arfcn_num;                  
    uint16 Ant;
    L1_LTE_NST_ARFCN_CONFIG_T arfcn[MAX_L1_LTE_NST_ARFCN_NUM];
} L1_LTE_NST_CONFIG_REQ_T;


typedef struct
{
    uint32 arfcn;                        
    uint16 frame_num;                     
    uint8  tx_count;                       
    L1_LTE_NST_RX_CONFIG_T rx;                            
    L1_LTE_NST_TX_CONFIG_T tx[MAX_L1_LTE_NST_TX_NUM];         

} L1_LTE_NST_ARFCN_V2_CONFIG_T;

typedef struct
{
    SIGNAL_VARS

    uint16 arfcn_num;                  
    uint16 Ant;
    L1_LTE_NST_ARFCN_V2_CONFIG_T arfcn[MAX_L1_LTE_NST_ARFCN_NUM];
} L1_LTE_NST_CONFIG_V2_REQ_T;

typedef struct
{
    SIGNAL_VARS

    uint16     status;                         
    uint16  arfcn_num;                    
    uint16     BLER10[MAX_L1_LTE_NST_ARFCN_NUM];  
} L1_LTE_NST_GET_BLER_RLT_T;

typedef struct 
{
    SIGNAL_VARS
    uint32   status;
    uint32   RSSI[2];
}L1_LTE_CONTINOUSTRX_GET_RSSI_CNF;

typedef struct
{
    SIGNAL_VARS

    uint16     status;
    uint16  arfcn_num;
    uint16       RSSI[MAX_L1_LTE_NST_ARFCN_NUM];
} L1_LTE_NST_GET_RSSI_RLT_T;

//  TOOL_MPH_LTE_NST_START_TEST_V2
typedef struct 
{
    uint8   HeaderSize;
    uint8   Version;
    uint8   ChannelSize;
    uint8   ChannelCount;
    uint16  ChannelOffset;
    uint8   TxSize;
    uint8   TxCount;
    uint16  TxOffset;
    uint8   Ant; 
}L1_LTE_NST_V2_HEARDER_T;

typedef struct
{
    SIGNAL_VARS

    uint16 adc1;
    uint16 adc0;
    uint16 k;
    uint16 reserved0;
    uint16 c0;
    uint16 c1;
    uint16 reserved1;
    uint16 reserved2;
}L1_LTE_TRANSCEIVER_TEMP_T;

typedef struct
{
	SIGNAL_VARS

	uint16 type;			// 0:uplink ;1:downlink
	uint16 nswitch;			// 0:off; 1: on
	uint32 indoor_freq;
	uint32 outdoor_freq;
	uint32 bandwidth;
	uint32 gain;
	uint32 power;
}L1_MGB_LTE_EG_SET_T;

typedef struct
{
	SIGNAL_VARS
	unsigned short flag;
	unsigned short link_path;
	unsigned short gainIndex_TX;
	unsigned short gainIndex_RX;
	unsigned short reg_273;
	unsigned short reg_274;
	unsigned short LNA;
	unsigned short reserved;
}L1_MGB_LTE_LO_REQ_T;

typedef struct  
{
	SIGNAL_VARS

	uint32 status;      // LTE_NST_STATUS_E
} L1_MGB_LTE_LO_INIT_RLT_T;

typedef struct
{
	uint32 freq_error;
	uint32 temperature; 
}TSX_DATA_T;

typedef struct 
{
    uint32    	cmd;   		
    uint32 	  	status;
    TSX_DATA_T 	value[2];
} DIAG_TSX_DATA;

typedef struct
{
	uint32 freq_error;
	uint32 tsxTemp; 
	uint32 oscTemp;
}TSX_DATA_V2_T;

typedef struct 
{
	uint32    		cmd;   		
	uint32 	  		status;
	TSX_DATA_V2_T 	value[4];
} DIAG_TSX_DATA_V2;

typedef struct 
{
	uint16  cmd;        // DIAG_AP_CMD_E
	uint16  length;     // Length of structure
	uint32  subcmd;   	//0 = Save, 1 =Load	
	uint32 	status;     //0 = success
	uint32  datalen;   //Data length
} DIAG_WCN_DATA_HEAD;

#pragma pack(pop)  // push 4

//////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

    typedef struct
    {
        uint8    byEngineSn[24];
        uint32   dwMapVersion;
        uint8    byActivateCode[16];

    } REF_GPS_NV_T;

    //#//[cmd] type=0x5E subtype=0x01
    typedef struct 
    {
        uint8           imei1[MAX_IMEI_NV_LENGTH];
        uint8           imei2[MAX_IMEI_NV_LENGTH];
        uint8           BTAddr[MAX_BT_ADDR_NV_LENGTH];
        REF_GPS_NV_T    GpsInfo;
        uint8           WIFIAddr[MAX_WIFI_ADDR_NV_LENGTH];
        uint8           reserved1[2]; 
        uint8           imei3[MAX_IMEI_NV_LENGTH];
        uint8           imei4[MAX_IMEI_NV_LENGTH];
        uint8           reserved2[16];

    } REF_NVWriteDirect_T;

#pragma pack(pop)

#pragma pack(push, 4)

// DIAG_FIXNV_DUMP Bug 652794 
enum
{
    FIXNV_CMD_QUERY = 1,
    FIXNV_CMD_DUMP  = 2
};

typedef struct  
{
    uint32 cmd;       
    uint32 reserved;  // 保留扩展用

} DIAG_FILE_DUMP_REQ_T;

typedef struct  
{
    uint32 cmd;                     // FIXNV_CMD_QUERY
    uint32 total_size;              // nvitem.bin总大小
    uint32 max_diag_buff_size;      // DIAG命令缓存大小（目前是7KB = 7*1024），单位字节。 工具通过这个参数申请数据接收缓存大小

} DIAG_DUMP_QUERY_ACK_T;

enum
{
    DumpFinished = 0,
    UnderDumping = 1,
    DumpError
};
typedef struct  
{
    uint32 cmd;      
    uint32 status;   // 0: 传输结束  1: 传输未完成， 其他为传输错误具体代码
    uint32 data_len; // pData的数据大小，单位字节

} DIAG_FILE_DUMP_ACK_T;

// 必须成对出现，否则会引起字节对齐问题
#pragma pack(pop)


#endif /* _DIAGDEF_H_0CA046AA_63EB_4581_8D73_0A07F1EE7A6D_ */

