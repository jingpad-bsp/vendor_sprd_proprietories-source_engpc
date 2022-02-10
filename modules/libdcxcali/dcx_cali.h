#ifndef TSX_RAWDATA_H_
#define TSX_RAWDATA_H_

typedef unsigned short uint16;
typedef unsigned int uint32;


//cmd = DIAG_AP_CMD_SET_PMIC_AFC_MODE;
//Req£ºTOOLS_DIAG_AP_REQ_T + DIAG_AP_PMIC_AFC_MODE
//Conf£ºTOOLS_DIAG_AP_CNF_T
typedef struct
{
    uint16 mode;    //0: low mode; 1:High mode
    uint16 reserved;
}DIAG_AP_PMIC_AFC_MODE;

//cmd = DIAG_AP_CMD_SET_PMIC_AFC_CDAC;
//Req£ºTOOLS_DIAG_AP_REQ_T + DIAG_AP_SET_PMIC_CDAC
//Conf£ºTOOLS_DIAG_AP_CNF_T
typedef struct
{
    uint16 CDAC;
    uint16 reserved;
}DIAG_AP_SET_PMIC_CDAC;

//cmd = DIAG_AP_CMD_PMIC_AFC_AMP_CALI;
//Req£ºTOOLS_DIAG_AP_REQ_T
//Conf£ºTOOLS_DIAG_AP_CNF_T + DIAG_AP_PMIC_AFC_AMP_DATA_T
typedef struct
{
    uint16 Amp_26M;  // HP or LP mode value specified by PCr
    uint16 reserved;
}DIAG_AP_PMIC_AFC_AMP_DATA_T;

//cmd = DIAG_AP_CMD_SAVE_PMIC_AFC_DATA;
//Req£ºTOOLS_DIAG_AP_REQ_T + DIAG_AP_PMIC_AFC_CALI_DATA_T
//Conf£ºTOOLS_DIAG_AP_CNF_T

//cmd = DIAG_AP_CMD_LOAD_PMIC_AFC_DATA;
//Req£ºTOOLS_DIAG_AP_REQ_T 
//Conf£ºTOOLS_DIAG_AP_CNF_T + DIAG_AP_PMIC_AFC_CALI_DATA_T
typedef struct
{
    uint16 CDAC;
    uint16 HP_Mode_Amp_26M;
    uint16 LP_Mode_Amp_26M;
    uint16 LP_Mode_Freq_Drift_32K;
    uint16 reserved[4];
}DIAG_AP_PMIC_AFC_CALI_DATA_T;


#endif