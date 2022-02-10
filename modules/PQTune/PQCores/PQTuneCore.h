#include "ParserCore.h"
#include "PQTuningParmDefine.h"

enum ENUM_IPCORE{
    DPU_LITE_R2P0,
    DPU_R2P0,
    DPU_R3P0,
    DPU_R4P0,
    DPU_MAX_NUM,
};

class PQTuneCore {
public:
	virtual int tune_connect(char *buf, int len, char *rsp, int rsplen);
	virtual	int tune_rgb_pattern(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_read_regs(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_write_regs(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_start_read_cfg(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_midst_read_cfg(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_end_read_cfg(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_start_write_cfg(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_midst_write_cfg(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_end_write_cfg(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_rd_tuning_reg(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_wr_tuning_reg(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_rd_tuning_xml(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_wr_tuning_xml(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_rd_ambient(char *buf, int len, char *rsp, int rsplen);
	virtual int tune_version();
	virtual ~PQTuneCore();
	PQTuneCore(int ver);

protected:
	int version;
	uint08_t *ctx;
	uint32_t offset;
	uint32_t tune_sizes;
	uint32_t gamma_size;
	uint32_t bld_size;
	uint32_t cms_size;
	uint32_t abc_size;
	uint32_t hsv_size;
	GammaParser *gamma;
	BldParser *bld;
	CmsParser *cms;
	AbcParser *abc;
	HsvParser *hsv;
};

class PQTuneCoreLiteR2p0: public PQTuneCore {
public:
	PQTuneCoreLiteR2p0(int ver);	
};

class PQTuneCoreR3p0: public PQTuneCoreLiteR2p0 {
public:
	PQTuneCoreR3p0(int ver);

};

class PQTuneCoreR4p0: public PQTuneCoreLiteR2p0 {
public:
	PQTuneCoreR4p0(int ver);
	virtual ~PQTuneCoreR4p0();
};
