#pragma once
typedef unsigned char    uint08_t;
typedef          char    int08_t;
typedef unsigned short   uint16_t;
typedef unsigned int     uint32_t;

typedef struct  
{
    uint32_t version; // reserved
    uint32_t enable;  // 0/1
} subversion;

typedef struct  
{
    uint32_t version;     // 
    uint32_t modify_time; // YYMMDDHHMM 
} mainversion;

typedef struct  
{
    uint16_t r[256];
    uint16_t g[256];
    uint16_t b[256];
} gamma_lut;

typedef struct 
{
    uint16_t hue;
    uint16_t sat;
} hsv_lut_table;

typedef struct  
{
    short coef00;
    short coef01;
    short coef02;
    short coef03;
    short coef10;
    short coef11;
    short coef12;
    short coef13;
    short coef20;
    short coef21;
    short coef22;
    short coef23;
} cm_cfg;

typedef struct  
{
    short rmin; 
    short rmax;
    short gmin;
    short gmax;
    short bmin;
    short bmax;
} range_cfg;

typedef struct  
{
    hsv_lut_table table[360];
} hsv_lut;

typedef struct  
{
    hsv_lut hsv;
    cm_cfg cm;
} hsv_cm;

typedef struct  
{
    cm_cfg cm;
    range_cfg range;
} cm_range;

typedef struct 
{
    uint08_t rgb;
    uint08_t cmindex;
} rgb_cm_mapping;

typedef struct 
{
    uint16_t ambient;
    uint16_t slp_brightness_factor;
} slp_mapping_table_item;

typedef struct  
{
    uint16_t ambient;
    uint16_t backlight;
} bl_mapping_table_item;

typedef struct  
{
    uint08_t brightness;
    uint08_t conversion_matrix;
    uint08_t brightness_step;
    uint08_t second_bright_factor;
    uint08_t first_percent_th;
    uint08_t first_max_bright_th;
} slp_cfg;

typedef struct 
{
    uint16_t epsilon0;
    uint16_t epsilon1;
    int08_t gain0;
    uint08_t gain1;
    uint08_t gain2;
    uint08_t gain3;
    int08_t gain4;
    uint08_t gain5;
    uint08_t gain6;
    uint08_t gain7;
    uint08_t max_diff;
    uint08_t min_diff;
} epf_cfg;

typedef struct  
{
    uint32_t map_num;
    slp_mapping_table_item item[128];
} slp_mapping_table;

typedef struct  
{
    uint32_t map_num;             // The count of actual use
    bl_mapping_table_item item[128];
} bl_mapping_table;

typedef struct  
{
    epf_cfg             epfCfgSunlightProtector;
    epf_cfg             epfCfgSuperResolution;
    slp_cfg             slpCfg;
    slp_mapping_table   slpMappingTable;
} scene_table_item;


typedef struct  
{
    uint16_t num;
    uint16_t index;
    scene_table_item sceneTableItem[16];
} scene_table;


#pragma region SHARKL3

typedef struct  
{
    subversion          version;
    gamma_lut           gamma;
} gamma_common;

typedef struct  
{
    subversion          version;
    uint32_t            mode;  // 1:default; 2:middle; 3:high
    hsv_cm              hsvcm[3];
} bld_common;

typedef struct  
{
    subversion          version;
    uint32_t            mode;   // 1:auto; 2:enhance; 3:standard 
    hsv_cm              hsvcm[3];
    rgb_cm_mapping      rgbcm[10];
    uint16_t            cm_mode;   // 1:cold; 2:warm; 3-12:rgb auto
    cm_cfg              cm[12];
} cms_common;

typedef struct  
{
    subversion          version;
    uint08_t            mode;                // 1:Normal; 2:low
	uint08_t			nMajorVersion;
    uint16_t            slpblMode;           // 1:slp; 2:bl
    scene_table         sceneTable[2];  
    bl_mapping_table    blMappingTable[2];
} abc_common;

typedef struct 
{
    mainversion         version;
    gamma_common        gamma;
    bld_common          bld;
    cms_common          cms;
    abc_common          abc;

} pq_tuning_parm;

#pragma endregion SHARKL3

#pragma region SHARKL5_ROC1

typedef struct  
{
    uint16_t slp_low_clip;
    uint16_t slp_high_clip;
    uint16_t slp_step_clip;
    uint16_t reserved;
} ltm_cfg;

typedef struct  
{
    epf_cfg             epfCfg;
    slp_cfg             slpCfg;
    ltm_cfg             ltmCfg;
    slp_mapping_table   slpMappingTable;
} scene_table_item_sharkl5;

typedef struct  
{
    uint16_t num;
    uint16_t index;
    scene_table_item_sharkl5 sceneTableItem[16];
} scene_table_sharkl5;


typedef struct  
{
    hsv_lut hsv;
    cm_cfg  cm;
    epf_cfg epf;
    slp_cfg slp;
    ltm_cfg ltm;
} hsv_cm_sharkl5;

typedef struct  
{
    subversion          version;
    uint16_t            nMode;  // 1: default; 2: Standard
    uint16_t            nMajorVersion;
    gamma_lut           gamma[2];
} gamma_common_sharkl5;


typedef struct  
{
    subversion          version;
    uint32_t            nMajorVersion;
    cm_range            cmrange;
} bld_common_sharkl5;

typedef struct  
{
    subversion          version;
    uint16_t            mode;   // 1:auto; 2:enhance; 3:standard 
    uint16_t            nMajorVersion;
    hsv_cm_sharkl5      hsvcm[3];
    rgb_cm_mapping      rgbcm[10];
    uint16_t            cm_mode;   // 1:cold; 2:warm; 3-12:rgb auto
    cm_cfg              cm[12];
} cms_common_sharkl5;

typedef struct  
{
    subversion                  version;
    uint08_t                    mode;                // 1:Normal; 2:low
    uint08_t                    nMajorVersion;
    uint16_t                    slpblMode;           // 1:slp; 2:bl
    scene_table_sharkl5         sceneTable[2];  
    bl_mapping_table            blMappingTable[2];
} abc_common_sharkl5;

typedef struct 
{
    mainversion             version;
    gamma_common_sharkl5    gamma;
    bld_common_sharkl5      bld;
    cms_common_sharkl5      cms;
    abc_common_sharkl5      abc;

} pq_tuning_parm_sharkl5;

typedef struct  
{
	epf_cfg             epfCfgSunlightProtector;
	epf_cfg             epfCfgSuperResolution;
    slp_cfg             slpCfg;
    ltm_cfg             ltmCfg;
    slp_mapping_table   slpMappingTable;
} scene_table_item_roc1;

typedef struct  
{
    uint16_t num;
    uint16_t index;
    scene_table_item_roc1 sceneTableItem[16];
} scene_table_roc1;

typedef struct  
{
    subversion          version;
    uint08_t            mode;                // 1:Normal; 2:low
    uint08_t            nMajorVersion;      
    uint16_t            slpblMode;           // 1:slp; 2:bl
    scene_table_roc1    sceneTable[2];  
    bl_mapping_table    blMappingTable[2];
} abc_common_roc1;

typedef struct 
{
    mainversion             version;
    gamma_common_sharkl5    gamma;
    bld_common_sharkl5      bld;
    cms_common_sharkl5      cms;
    abc_common_roc1         abc;

} pq_tuning_parm_roc1;

#pragma endregion SHARKL5_ROC1

#pragma region SHARKL5PRO

typedef struct  
{
    uint16_t r[729];
    uint16_t g[729];
    uint16_t b[729];
} lut_3d;

typedef struct
{
    subversion version;
    uint32_t   nMajorVersion;  
    uint32_t   flag_bit_no; 
    lut_3d     lut3d[3];
} hsv_common_sharkl5Pro;

typedef struct  
{
    uint16_t brightness;
} slp_cfg_sharkl5Pro_cms;

typedef struct  
{
    hsv_lut hsv;
    cm_cfg  cm;
    epf_cfg epf;
    slp_cfg_sharkl5Pro_cms slp;
    ltm_cfg ltm;
} hsv_cm_sharkl5Pro;

typedef struct  
{
    subversion          version;
    uint16_t            mode;   // 1:auto; 2:enhance; 3:standard 
    uint16_t            nMajorVersion;
    hsv_cm_sharkl5Pro   hsvcm[3];
    rgb_cm_mapping      rgbcm[10];
    uint16_t            cm_mode;   // 1:cold; 2:warm; 3-12:rgb auto
    cm_cfg              cm[12];
} cms_common_sharkl5Pro;

typedef struct
{
	uint08_t brightness;
	uint08_t reserved0;
	uint16_t brightness_step;
	uint08_t first_max_bright_th;
	uint08_t first_max_bright_th_step0;
	uint08_t first_max_bright_th_step1;
	uint08_t first_max_bright_th_step2;
	uint08_t first_max_bright_th_step3;
	uint08_t first_max_bright_th_step4;
	uint16_t mask_height;
	uint08_t first_percent_th;
	uint08_t first_pth_index0;
	uint08_t first_pth_index1;
	uint08_t first_pth_index2;
	uint08_t first_pth_index3;
	uint08_t reserved[3];
} slp_cfg_sharkl5Pro_abc;

typedef struct
{
	uint08_t hist_exb_no;
	uint08_t hist_exb_percent;
	uint08_t hist9_index0;
	uint08_t hist9_index1;
	uint08_t hist9_index2;
	uint08_t hist9_index3;
	uint08_t hist9_index4;
	uint08_t hist9_index5;
	uint08_t hist9_index6;
	uint08_t hist9_index7;
	uint08_t hist9_index8;
	uint08_t glb_x1;
	uint08_t glb_x2;
	uint08_t glb_x3;
	uint16_t glb_s1;
	uint16_t glb_s2;
	uint16_t glb_s3;
	uint08_t fast_ambient_th;
	uint08_t screen_change_percent_th;
	uint08_t local_weight;
	uint08_t reserved;
} slp_basecfg_sharkl5Pro_abc;

typedef struct  
{
    epf_cfg                epfCfgSunlightProtector;
    epf_cfg                epfCfgSuperResolution;
    slp_cfg_sharkl5Pro_abc slpCfg;
    ltm_cfg                ltmCfg;
    slp_mapping_table      slpMappingTable;
} scene_table_item_sharkl5Pro;

typedef struct  
{
    uint16_t num;
    uint16_t index;
    slp_basecfg_sharkl5Pro_abc slpbasecfg;
    scene_table_item_sharkl5Pro sceneTableItem[16];
} scene_table_sharkl5Pro;

typedef struct  
{
    subversion          version;
    uint16_t            mode;                // 1:Normal; 2:low
    uint16_t            nMajorVersion;      
    scene_table_sharkl5Pro sceneTable[2];  
} abc_common_sharkl5Pro;

typedef struct 
{
    mainversion             version;
    gamma_common_sharkl5    gamma;
    bld_common_sharkl5      bld;
    cms_common_sharkl5Pro   cms;
    abc_common_sharkl5Pro   abc;
    hsv_common_sharkl5Pro   hsv;

} pq_tuning_parm_sharkl5Pro;

#pragma endregion SHARKL5PRO

typedef struct 
{
    uint32_t light;
} ambient_light;


