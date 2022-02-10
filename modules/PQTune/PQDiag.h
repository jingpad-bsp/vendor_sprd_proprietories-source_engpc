#define MAX_NAME_LEN 64
#define MAX_OPEN_TIMES 100


typedef struct _RESOLUTION_T
{
  unsigned int HPixel;
  unsigned int VPixel;
}RESOLUTION_T;

typedef struct _DUT_INFO_T
{
  unsigned int nSize;             // size of this struct itself
  unsigned int nVersion;          // the version of struct itself
  char szModelName[MAX_NAME_LEN]; // Model name,such as SC7731.
  char szChipName[MAX_NAME_LEN];  // Chip model ,such as 7731
  RESOLUTION_T stResolution;      // Resolution
  float  fLcdInch;                // LcdInch
}DUT_INFO_T;

typedef enum {
  CMD_PQ_DUMMY,
  CMD_PQ_CONNECT,
  CMD_PQ_RGB_PATTERN,
  CMD_PQ_RD_REGS,
  CMD_PQ_WR_REGS,
  CMD_PQ_START_READ_CFG,
  CMD_PQ_MIDST_READ_CFG,
  CMD_PQ_END_READ_CFG,
  CMD_PQ_START_WRITE_CFG,
  CMD_PQ_MIDST_WRITE_CFG,
  CMD_PQ_END_WRITE_CFG,
  CMD_PQ_RD_TUNING_REG,
  CMD_PQ_WR_TUNING_REG,
  CMD_PQ_RD_TUNING_XML,
  CMD_PQ_WR_TUNING_XML,
  CMD_PQ_RD_AMBIENT,
  CMD_PQ_END
} DIAG_PQ_CMD_TYPE;

typedef enum {
  PQ_VERSION,
  PQ_GAMMA,
  PQ_BLD,
  PQ_CMS,
  PQ_ABC,
  PQ_HSV,
  E_UNDEFINE,
} PQ_MODULE_TYPE;

