#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <utils/Log.h>
#include <libxml/parser.h>                                                                                                                                                                          
#include <libxml/tree.h>
#include <../libxml.h>
#include "PQSysfs.h"
#include "PQTuningParmDefine.h"

xmlNodePtr FindNode(xmlNodePtr curNode, const char *name);
void RemoveNode(xmlNode *ParentNode, xmlNode *ChildNode);

typedef unsigned char    u8;
typedef          char    s8;
typedef unsigned short   u16;
typedef unsigned int     u32;

enum epf_fun
{
	EPF_FUN_SUNLIGHT_PROTECTOR,
	EPF_FUN_SUPER_RESOLUTION,
	EPF_FUN_MAX,
};

typedef struct {
	uint32_t value[729];
} lut3d_cfg;

typedef struct
{
	uint08_t brightness;
	uint08_t conversion_matrix;
	uint08_t brightness_step;
	uint08_t second_bright_factor;
	uint08_t first_percent_th;
	uint08_t first_max_bright_th;
	uint16_t slp_low_clip;
	uint16_t slp_high_clip;
	uint16_t slp_step_clip;
	uint16_t reserved;
} slp_ltm_cfg;

typedef struct
{
	uint08_t brightness;
	uint16_t brightness_step;
	uint08_t fst_max_bright_th;
	uint08_t fst_max_bright_th_step[5];
	uint08_t hist_exb_no;
	uint08_t hist_exb_percent;
	uint16_t mask_height;
	uint08_t fst_pth_index[4];
	uint08_t hist9_index[9];
	uint08_t glb_x[3];
	uint16_t glb_s[3];
	uint16_t limit_hclip;
	uint16_t limit_lclip;
	uint16_t limit_clip_step;
	uint08_t fast_ambient_th;
	uint08_t scene_change_percent_th;
	uint08_t local_weight;
	uint08_t fst_pth;
	uint08_t cabc_endv;
	uint08_t cabc_startv;
} slp_ltm_cfg_sharkl5pro;

static slp_ltm_cfg  slp_ltm_params;
static slp_ltm_cfg_sharkl5pro  slp_ltm_params_l5pro;
static lut3d_cfg lut3d_l5pro;

class XmlParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
	virtual ~XmlParser(){}
};

class AbcParser: public XmlParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};

class CmsParser: public XmlParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};

class GammaParser: public XmlParser{
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};

class BldParser: public XmlParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};

class AbcParserLiteR2p0: public AbcParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};

class CmsParserLiteR2p0: public CmsParser {
public:
	virtual int update_xml(uint08_t *cms);
	virtual int parse_xml(uint08_t *cms);
	virtual int update_reg(uint08_t *cms);
	virtual int parse_reg(uint08_t *cms);
};

class GammaParserLiteR2p0: public GammaParser{
public:
	int update_xml(uint08_t *ctx);
	int parse_xml(uint08_t *ctx);
	int update_reg(uint08_t *ctx);
	int parse_reg(uint08_t *ctx);
};

class BldParserLiteR2p0: public BldParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};

class AbcParserR3p0: public AbcParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};

class AbcParserR4p0: public AbcParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};


class CmsParserR4p0: public CmsParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};

class HsvParser: public XmlParser {
public:
	virtual int update_xml(uint08_t *ctx);
	virtual int parse_xml(uint08_t *ctx);
	virtual int update_reg(uint08_t *ctx);
	virtual int parse_reg(uint08_t *ctx);
};

