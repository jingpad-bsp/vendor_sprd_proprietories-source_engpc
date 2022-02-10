#include <utils/Log.h>
#include "ParserCore.h"

#define PARSE_CMS_EPF(I, X) \
({ \
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
		cms->hsvcm[I].epf.X = strtoul((char *)szPropity, NULL, 0); \
		ALOGD("pqpqpq epf rrrr %d %s i = %d",cms->hsvcm[I].epf.X, #X, I); \
		xmlFree(szPropity); \
		propNode = propNode->next; \
	} \
})

#define PARSE_CMS_CM(I, X) \
({ \
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
		cms->hsvcm[I].cm.X = strtoul((char *)szPropity, NULL, 0); \
		ALOGD("pqpqpq cm rrrr %d %s i = %d",cms->hsvcm[I].cm.X, #X, I); \
		xmlFree(szPropity); \
		propNode = propNode->next; \
	} \
})

#define PARSE_CMS_CM_CFG(I, X) \
({ \
        if (xmlHasProp(propNode, BAD_CAST #X)) { \
                szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
                cms->cm[I].X = strtoul((char *)szPropity, NULL, 0); \
				xmlFree(szPropity); \
                propNode = propNode->next; \
        } \
})

#define PARSE_CMS_SLP(I, X) \
({ \
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
		cms->hsvcm[I].slp.X = strtoul((char *)szPropity, NULL, 0); \
		ALOGD("pqpqpq slp rrrr %d %s %d",cms->hsvcm[I].slp.X, #X, I); \
		xmlFree(szPropity); \
		propNode = propNode->next; \
	} \
})

#define PARSE_CMS_LTM(I, X) \
({ \
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
		cms->hsvcm[I].ltm.X = strtoul((char *)szPropity, NULL, 0); \
		ALOGD("pqpqpq ltm rrrr %d %s i = %d",cms->hsvcm[I].ltm.X, #X, I); \
		xmlFree(szPropity); \
		propNode = propNode->next; \
	} \
})


#define UPDATE_CMS_EPF(I, X)   \
({  \
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		snprintf(numStr, sizeof(numStr), "%d", cms->hsvcm[I].epf.X); \
		xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
		ALOGD("pqpqpq epf wwwwww %d %s i = %d",cms->hsvcm[I].epf.X, #X, I); \
		propNode = propNode->next; \
	} \
})

#define UPDATE_CMS_CM(I, X) \
({	\
	if (xmlHasProp(propNode, BAD_CAST #X)) {	\
		snprintf(numStr, sizeof(numStr), "%d", cms->hsvcm[I].cm.X); \
		xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
		ALOGD("pqpqpq cm wwww %d %s i = %d",cms->hsvcm[I].cm.X, #X, I); \
		propNode = propNode->next; \
	}	\
})

#define UPDATE_CMS_CM_CFG(I, X) \
({      \
        if (xmlHasProp(propNode, BAD_CAST #X)) {        \
                snprintf(numStr, sizeof(numStr), "%d", cms->cm[I].X); \
                xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
		ALOGD("pqpqpq cm_cfg wwww %d %s i= %d",cms->cm[I].X, #X, I); \
                propNode = propNode->next; \
        }       \
})

#define UPDATE_CMS_SLP(I, X) \
({  \
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		snprintf(numStr, sizeof(numStr), "%d", cms->hsvcm[I].slp.X); \
		xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
		ALOGD("pqpqpq slp wwww %d %s i= %d",cms->hsvcm[I].slp.X, #X, I); \
		propNode = propNode->next; \
	} \
})

#define UPDATE_CMS_LTM(I, X) \
({	\
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		snprintf(numStr, sizeof(numStr), "%d", cms->hsvcm[I].ltm.X); \
		xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
		ALOGD("pqpqpq ltm wwww %d %s i = %d",cms->hsvcm[I].ltm.X, #X, i); \
		propNode = propNode->next; \
	} \
})


#define PARSE_CMS_CM_ARRAYS(i) \
({ \
	PARSE_CMS_CM(i, coef00); \
	PARSE_CMS_CM(i, coef01); \
	PARSE_CMS_CM(i, coef02); \
	PARSE_CMS_CM(i, coef03); \
	PARSE_CMS_CM(i, coef10); \
	PARSE_CMS_CM(i, coef11); \
	PARSE_CMS_CM(i, coef12); \
	PARSE_CMS_CM(i, coef13); \
	PARSE_CMS_CM(i, coef20); \
	PARSE_CMS_CM(i, coef21); \
	PARSE_CMS_CM(i, coef22); \
	PARSE_CMS_CM(i, coef23); \
})

#define PARSE_CMS_CM_CFG_ARRAYS(i) \
({ \
        PARSE_CMS_CM_CFG(i, coef00); \
        PARSE_CMS_CM_CFG(i, coef01); \
        PARSE_CMS_CM_CFG(i, coef02); \
        PARSE_CMS_CM_CFG(i, coef03); \
        PARSE_CMS_CM_CFG(i, coef10); \
        PARSE_CMS_CM_CFG(i, coef11); \
        PARSE_CMS_CM_CFG(i, coef12); \
        PARSE_CMS_CM_CFG(i, coef13); \
        PARSE_CMS_CM_CFG(i, coef20); \
        PARSE_CMS_CM_CFG(i, coef21); \
        PARSE_CMS_CM_CFG(i, coef22); \
        PARSE_CMS_CM_CFG(i, coef23); \
})

#define PARSE_CMS_EPF_ARRAYS(i) \
({ \
	PARSE_CMS_EPF(i, epsilon0); \
	PARSE_CMS_EPF(i, epsilon1); \
	PARSE_CMS_EPF(i, gain0); \
	PARSE_CMS_EPF(i, gain1); \
	PARSE_CMS_EPF(i, gain2); \
	PARSE_CMS_EPF(i, gain3); \
	PARSE_CMS_EPF(i, gain4); \
	PARSE_CMS_EPF(i ,gain5); \
	PARSE_CMS_EPF(i, gain6); \
	PARSE_CMS_EPF(i, gain7); \
	PARSE_CMS_EPF(i, max_diff); \
	PARSE_CMS_EPF(i, min_diff); \
})

#define PARSE_CMS_SLP_ARRAYS(i) \
({ \
	PARSE_CMS_SLP(i, brightness); \
	PARSE_CMS_SLP(i, conversion_matrix); \
	PARSE_CMS_SLP(i, brightness_step); \
	PARSE_CMS_SLP(i, second_bright_factor); \
	PARSE_CMS_SLP(i, first_percent_th); \
	PARSE_CMS_SLP(i ,first_max_bright_th); \
})

#define PARSE_CMS_LTM_ARRAYS(i) \
({ \
	PARSE_CMS_LTM(i, slp_step_clip); \
	PARSE_CMS_LTM(i, slp_high_clip); \
	PARSE_CMS_LTM(i, slp_low_clip); \
})

#define UPDATE_CMS_CM_ARRAYS(i) \
({ \
	UPDATE_CMS_CM(i, coef00); \
	UPDATE_CMS_CM(i, coef01); \
	UPDATE_CMS_CM(i, coef02); \
	UPDATE_CMS_CM(i, coef03); \
	UPDATE_CMS_CM(i ,coef10); \
	UPDATE_CMS_CM(i, coef11); \
	UPDATE_CMS_CM(i, coef12); \
	UPDATE_CMS_CM(i, coef13); \
	UPDATE_CMS_CM(i, coef20); \
	UPDATE_CMS_CM(i, coef21); \
	UPDATE_CMS_CM(i, coef22); \
	UPDATE_CMS_CM(i, coef23); \
})

#define UPDATE_CMS_CM_CFG_ARRAYS(i) \
({ \
        UPDATE_CMS_CM_CFG(i, coef00); \
        UPDATE_CMS_CM_CFG(i, coef01); \
        UPDATE_CMS_CM_CFG(i, coef02); \
        UPDATE_CMS_CM_CFG(i, coef03); \
        UPDATE_CMS_CM_CFG(i ,coef10); \
        UPDATE_CMS_CM_CFG(i, coef11); \
        UPDATE_CMS_CM_CFG(i, coef12); \
        UPDATE_CMS_CM_CFG(i, coef13); \
        UPDATE_CMS_CM_CFG(i, coef20); \
        UPDATE_CMS_CM_CFG(i, coef21); \
        UPDATE_CMS_CM_CFG(i, coef22); \
        UPDATE_CMS_CM_CFG(i, coef23); \
})

#define UPDATE_CMS_EPF_ARRAYS(i) \
({ \
	UPDATE_CMS_EPF(i, epsilon0); \
	UPDATE_CMS_EPF(i, epsilon1); \
	UPDATE_CMS_EPF(i, gain0); \
	UPDATE_CMS_EPF(i, gain1); \
	UPDATE_CMS_EPF(i, gain2); \
	UPDATE_CMS_EPF(i, gain3); \
	UPDATE_CMS_EPF(i, gain4); \
	UPDATE_CMS_EPF(i ,gain5); \
	UPDATE_CMS_EPF(i, gain6); \
	UPDATE_CMS_EPF(i, gain7); \
	UPDATE_CMS_EPF(i, max_diff); \
	UPDATE_CMS_EPF(i, min_diff); \
})

#define UPDATE_CMS_SLP_ARRAYS(i) \
({ \
	UPDATE_CMS_SLP(i, brightness); \
	UPDATE_CMS_SLP(i, conversion_matrix); \
	UPDATE_CMS_SLP(i, brightness_step); \
	UPDATE_CMS_SLP(i, second_bright_factor); \
	UPDATE_CMS_SLP(i, first_percent_th); \
	UPDATE_CMS_SLP(i, first_max_bright_th);\
})

#define UPDATE_CMS_LTM_ARRAYS(i) \
({ \
	UPDATE_CMS_LTM(i, slp_step_clip); \
	UPDATE_CMS_LTM(i, slp_high_clip); \
	UPDATE_CMS_LTM(i, slp_low_clip); \
})



static int parse_hsv(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	int j = 0;

	propNode = subNode->children;
	while (NULL != propNode) {
		attrPtr = propNode->properties;
		while (NULL != attrPtr) {
			if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"hue")) {
				szPropity = xmlGetProp(propNode, (const xmlChar*)"hue");
				cms->hsvcm[i].hsv.table[j].hue = strtoul((char *)szPropity, NULL, 0);
				xmlFree(szPropity);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"sat")) {
				szPropity = xmlGetProp(propNode, (const xmlChar*)"sat");
				cms->hsvcm[i].hsv.table[j].sat = strtoul((char *)szPropity, NULL, 0);
				xmlFree(szPropity);
			}
			attrPtr = attrPtr->next;
		}
		j++;
		propNode = propNode->next;
	}

	return 0;
}

static int parse_epf(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;

	propNode = subNode->children;

	PARSE_CMS_EPF_ARRAYS(i);

	return 0;
}

static int parse_cm(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;

	propNode = subNode->children;

	PARSE_CMS_CM_ARRAYS(i);

	return 0;
}

static int parse_slp(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;

	propNode = subNode->children;

	PARSE_CMS_SLP_ARRAYS(i);

	return 0;
}

static int parse_ltm(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	int j = 0;

	propNode = subNode->children;

	PARSE_CMS_LTM_ARRAYS(i);

	return 0;
}

static int parse_hsv_cm_epf_slp_ltm(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr curNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;

	curNode = subNode;
	
	while(NULL != curNode) {
		if (!xmlStrcmp(curNode->name, (const xmlChar*)"hsv")) {
			parse_hsv(cms, curNode, i);
		} else if (!xmlStrcmp(curNode->name, (const xmlChar*)"cm")) {
			parse_cm(cms, curNode, i);
		} else if (!xmlStrcmp(curNode->name, (const xmlChar*)"epf")) {
			parse_epf(cms, curNode, i);
		} else if (!xmlStrcmp(curNode->name, (const xmlChar*)"slp")) {
			parse_slp(cms, curNode, i);
		} else if (!xmlStrcmp(curNode->name, (const xmlChar*)"ltm")) {
			parse_ltm(cms, curNode, i);
		}
		curNode = curNode->next;
	}

	return 0;
}

static int parse_cms_version(cms_common_sharkl5 *cms, xmlNodePtr curNode)
{
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

    	subNode = curNode; //subNode table
	while(!xmlStrcmp(subNode->name, BAD_CAST"enhance")) {
		if(xmlHasProp(subNode, BAD_CAST "version")) {
            szPropity = xmlGetProp(subNode, (const xmlChar*) "version");
            cms->version.version = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
        } else if (xmlHasProp(subNode, BAD_CAST "major_version")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*) "major_version");
			cms->nMajorVersion = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
		}
		subNode = subNode->next;
	}

	return 0;
}

static int parse_cms_hsvcm_epf_slp_ltm(cms_common_sharkl5 *cms, xmlNodePtr curNode)
{
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	int i = 0;
	int j = 0;

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode->children; //subNode table
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "auto")) {
				xmlFree(szPropity);
				ALOGD("PQ CMS coef auto\n");
				parse_hsv_cm_epf_slp_ltm(cms, subNode->children, 0);
			}
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "enhance")) {
				xmlFree(szPropity);
				ALOGD("PQ CMS enhance coef\n");
				parse_hsv_cm_epf_slp_ltm(cms, subNode->children, 1);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "standard")) {
				xmlFree(szPropity);
				ALOGD("PQ CMS coef standard\n");
				parse_hsv_cm_epf_slp_ltm(cms, subNode->children, 2);
			}
		}
		subNode = subNode->next;
		i++;
	}
	return 0;
}

static int parse_cms_rgbmap_table(cms_common_sharkl5 *cms, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;

	subNode = curNode;
	ALOGD("PQ read curNode name %s \n",curNode->name);
	propNode = subNode->children;
	while (NULL != propNode) {
		attrPtr = propNode->properties;
		while (NULL != attrPtr) {
			if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"rgb")) {
				szPropity = xmlGetProp(propNode, (const xmlChar*)"rgb");
				cms->rgbcm[i].rgb = strtoul((char *)szPropity, NULL, 0);
				ALOGD("rgb %d \n", cms->rgbcm[i].rgb);
				xmlFree(szPropity);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"index")) {
				szPropity = xmlGetProp(propNode, (const xmlChar*)"index");
				cms->rgbcm[i].cmindex = strtoul((char *)szPropity, NULL, 0);
				ALOGD("index %d \n", cms->rgbcm[i].cmindex);
				xmlFree(szPropity);
			}
			attrPtr = attrPtr->next;
        }
		i++;
		propNode = propNode->next;
	}
	return 0;
}


static int parse_cms_config_table(cms_common_sharkl5 *cms, xmlNodePtr curNode)
{
	int i = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode->children; //subNode table

	if (NULL != subNode)
		propNode = subNode->children;
	else
		propNode = NULL;

	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "cold")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "warm")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "0")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
				ALOGD("PQ auto_auto read 0 \n");
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "1")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
				ALOGD("PQ auto_auto 1 read \n");
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "2")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "3")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "4")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "5")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "6")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "7")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "8")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "9")) {
				xmlFree(szPropity);
				PARSE_CMS_CM_CFG_ARRAYS(i);
			}
		}
		i++;
		subNode = subNode->next;
		if(subNode)
			propNode = subNode->children;
	}

	return 0;
}

static int update_cms_version(cms_common_sharkl5 *cms, xmlNodePtr curNode)
{
    int i = 0;
    xmlNodePtr subNode;
    char numStr[12];

    ALOGD("curNode name %s \n",curNode->name);
    subNode = curNode; //subNode table
	while(!xmlStrcmp(subNode->name, BAD_CAST "enhance")) {
		if(xmlHasProp(subNode, BAD_CAST "version")) {
            snprintf(numStr, sizeof(numStr), "%d", cms->version.version);
            xmlSetProp(subNode, BAD_CAST "version", (const xmlChar*)numStr);
		}
		subNode = subNode->next;
	}
    return 0;
}

static int update_hsv(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	int j = 0;
	char numStr[10];

	propNode = subNode;
	while (NULL != propNode) {
		attrPtr = propNode->properties;
		while (NULL != attrPtr) {
			if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"hue")) {
				snprintf(numStr, sizeof(numStr), "%d", cms->hsvcm[i].hsv.table[j].hue);
				xmlSetProp(propNode, BAD_CAST "hue", (const xmlChar*)numStr);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"sat")) {
				snprintf(numStr, sizeof(numStr), "%d", cms->hsvcm[i].hsv.table[j].sat);
				xmlSetProp(propNode, BAD_CAST "sat", (const xmlChar*)numStr);
			}
			attrPtr = attrPtr->next;
 		}
		j++;
		propNode = propNode->next;
	}

	return 0;
}

static int update_cm(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	int j = 0;
	char numStr[10];

	propNode = subNode;
	UPDATE_CMS_CM_ARRAYS(i);

	return 0;
}

static int update_epf(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	int j = 0;
	char numStr[10];

	propNode = subNode;
	UPDATE_CMS_EPF_ARRAYS(i);

	return 0;
}

static int update_slp(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	int j = 0;
	char numStr[10];

	propNode = subNode;
	UPDATE_CMS_SLP_ARRAYS(i);

	return 0;
}

static int update_ltm(cms_common_sharkl5 *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	int j = 0;
	char numStr[10];

	propNode = subNode;
	UPDATE_CMS_LTM_ARRAYS(i);

	return 0;
}

static int update_hsv_cm_epf_slp_ltm(cms_common_sharkl5 *cms, xmlNodePtr curNode, int i)
{

	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;

	subNode = curNode;

	while(NULL != subNode) {
		if (!xmlStrcmp(subNode->name, (const xmlChar*)"hsv"))
			update_hsv(cms, subNode->children, i);
		else if (!xmlStrcmp(subNode->name, (const xmlChar*)"cm"))
			update_cm(cms, subNode->children, i);
		else if (!xmlStrcmp(subNode->name, (const xmlChar*)"slp"))
			update_slp(cms, subNode->children, i);
		else if (!xmlStrcmp(subNode->name, (const xmlChar*)"epf"))
			update_epf(cms, subNode->children, i);
		else if (!xmlStrcmp(subNode->name, (const xmlChar*)"ltm"))
			update_ltm(cms, subNode->children, i);
		subNode = subNode->next;
	}

	return 0;
}

static int update_cms_hsv_cm_epf_slp_ltm(cms_common_sharkl5 *cms, xmlNodePtr curNode)
{
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode->children; //subNode table
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "auto")) {
				xmlFree(szPropity);
				ALOGD("PQ wr CMS coef auto\n");
				update_hsv_cm_epf_slp_ltm(cms, subNode->children, 0);
			}
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "enhance")) {
				xmlFree(szPropity);
				ALOGD("PQ wr CMS coef enhance\n");
				update_hsv_cm_epf_slp_ltm(cms, subNode->children, 1);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "standard")) {
				xmlFree(szPropity);
				ALOGD("PQ wr CMS coef standard\n");
				update_hsv_cm_epf_slp_ltm(cms, subNode->children, 2);
			}
		}
		subNode = subNode->next;
	}
	return 0;
}

static int update_cms_rgbmap_table(cms_common_sharkl5 *cms, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	subNode = curNode;
	ALOGD("PQ enter updatea rgpmap curNode name %s \n",curNode->name);
	propNode = subNode->children;
	while (NULL != propNode) {
		attrPtr = propNode->properties;
		while (NULL != attrPtr) {
			if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"rgb")) {
				snprintf(numStr, sizeof(numStr), "%d", cms->rgbcm[i].rgb);
				xmlSetProp(propNode, BAD_CAST "rgb", (const xmlChar*)numStr);
				ALOGD("rgb %d \n", cms->rgbcm[i].rgb);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"index")) {
				snprintf(numStr, sizeof(numStr), "%d", cms->rgbcm[i].cmindex);
				xmlSetProp(propNode, BAD_CAST "index", (const xmlChar*)numStr);
				ALOGD("index %d \n", cms->rgbcm[i].cmindex);
			}
			attrPtr = attrPtr->next;
        }
		i++;
		propNode = propNode->next;
	}
	return 0;
}


static int update_cms_config_table(cms_common_sharkl5 *cms, xmlNodePtr curNode)
{
	int i = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode->children; //subNode table

	if (NULL != subNode)
		propNode = subNode->children;
	else
		propNode = NULL;

	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "cold"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "warm"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "0")) {
				UPDATE_CMS_CM_CFG_ARRAYS(i);
				ALOGD("PQ auto_auto 0\n");
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "1")) {
				UPDATE_CMS_CM_CFG_ARRAYS(i);
				ALOGD("PQ auto_auto 1\n");
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "2"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "3"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "4"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "5"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "6"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "7"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "8"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "9"))
				UPDATE_CMS_CM_CFG_ARRAYS(i);
			if (szPropity)
				xmlFree(szPropity);
		}
		i++;
		subNode = subNode->next;
		if(subNode)
			propNode = subNode->children;
	}

	ALOGD("cm_config finish0 %s \n",curNode->name);
	return 0;
}

int CmsParserLiteR2p0::parse_reg(uint08_t *ctx)
{
	int fd0, fd1, fd2, fd3;
	unsigned int sizes0,sizes1, sizes2, sizes3;
	unsigned char* data;
	int cnt;
	unsigned short tmp,tmp1;
	cms_common_sharkl5 *cms;

	cms = &((pq_tuning_parm_sharkl5 *)ctx)->cms;


	fd0 = open(DpuHsv, O_RDWR);
	fd1 = open(DpuCm, O_RDWR);
	fd2 = open(DpuSlp, O_RDWR);
	fd3 = open(DpuEpf, O_RDWR);

	if(fd0 < 0 || fd1 < 0 || fd2 < 0 || fd3 < 0) {
		if (fd0 >= 0)
			close(fd0);
		if (fd1 >= 0)
			close(fd1);
		if (fd2 >= 0)
			close(fd2);
		if (fd3 >= 0)
			close(fd3);
		ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
		return errno;
	}
	sizes0 = sizeof(cms->hsvcm[0].hsv);
	sizes1 = sizeof(cms->hsvcm[0].cm);
	sizes2 = sizeof(slp_ltm_params);
	sizes3 = sizeof(cms->hsvcm[0].epf);
	memset(&slp_ltm_params, 0, sizes2);
	ALOGD("parse_cms_reg sizes0 %d: size1 %d\n", sizes0, sizes1);
	cnt = read(fd0, &(cms->hsvcm[0].hsv), sizes0);
	ALOGD("parse_cms_reg cnt0 %d \n", cnt);
	cnt = read(fd1, &(cms->hsvcm[0].cm), sizes1);
	ALOGD("parse_cms_reg cnt %d\n", cnt);
	cnt = read(fd3, &(cms->hsvcm[0].epf), sizes3);
	cnt = read(fd2, (unsigned char*)&slp_ltm_params, sizes2);
	tmp = sizeof(cms->hsvcm[0].slp);
	memcpy((unsigned char*)&cms->hsvcm[0].slp, (unsigned char*)&slp_ltm_params, tmp);
	tmp1 = sizeof(cms->hsvcm[0].ltm);
	memcpy((unsigned char*)&cms->hsvcm[0].ltm, (unsigned char*)&slp_ltm_params + tmp, tmp1);
	close(fd0);
	close(fd1);
	close(fd2);
	close(fd3);

	return 0;
}

int CmsParserLiteR2p0::update_reg(uint08_t *ctx)
{
	int fd0, fd1, fd2, fd3;
	unsigned int sizes0,sizes1,sizes2,sizes3;
	unsigned char* data;
	unsigned int disable;
	unsigned short tmp,tmp1;
	cms_common_sharkl5 *cms;

	cms = &((pq_tuning_parm_sharkl5 *)ctx)->cms;


	if(cms->version.enable) {
		fd0 = open(DpuHsv, O_RDWR);
		fd1 = open(DpuCm, O_RDWR);
		fd2 = open(DpuSlp, O_RDWR);
		fd3 = open(DpuEpf, O_RDWR);
		if(fd0 < 0 || fd1 < 0 || fd2 < 0 || fd3 < 0) {
			if (fd0 >= 0)
				close(fd0);
			if (fd1 >= 0)
				close(fd1);
			if (fd2 >= 0)
				close(fd2);
			if (fd3 >= 0)
				close(fd3);
			ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
			return errno;
		}
		sizes0 = sizeof(cms->hsvcm[0].hsv);
		sizes1 = sizeof(cms->hsvcm[0].cm);
		sizes2 = sizeof(slp_ltm_params);
		sizes3 = sizeof(cms->hsvcm[0].epf);
		write(fd0, (unsigned char *)&cms->hsvcm[0].hsv, sizes0);
		write(fd1, (unsigned char *)&cms->hsvcm[0].cm, sizes1);
		write(fd3, (unsigned char *)&cms->hsvcm[0].epf, sizes3);
		tmp = sizeof(cms->hsvcm[0].slp);
		memcpy((unsigned char*)&slp_ltm_params, (unsigned char*)&cms->hsvcm[0].slp, tmp);
		tmp1 = sizeof(cms->hsvcm[0].ltm);
		memcpy((unsigned char*)&slp_ltm_params + tmp, (unsigned char*)&cms->hsvcm[0].ltm, tmp1);
		write(fd2, (unsigned char*)&slp_ltm_params, sizes2 - 2);
		close(fd0);
		close(fd1);
		close(fd2);
		close(fd3);
	} else {
		fd2 = open(PQDisable, O_WRONLY);
		if(fd2 < 0) {
			ALOGD("%s: fd2 open file failed, err: %s\n", __func__, strerror(errno));
			return errno;
		}
		disable = CMS_EN | HSV_EN | EPF_EN | LTM_EN;
		write(fd2, &disable, sizeof(disable));
		close(fd2);
	}

	return 0;
}

int CmsParserLiteR2p0::update_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	cms_common_sharkl5 *cms;

	cms = &((pq_tuning_parm_sharkl5 *)ctx)->cms;

	doc = xmlReadFile(cms_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
        ALOGD("Document not parsed successfully.\n");
        return -1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"root")) {
        ALOGD("root node != root\n");
		xmlFreeDoc(doc);
        return -1;
	}

	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "hsv_cm_epf_slp_ltm"))
		update_cms_hsv_cm_epf_slp_ltm(cms, tmpNode);
	if(tmpNode = FindNode(curNode, "rgb_mapping_table"))
		update_cms_rgbmap_table(cms, tmpNode);
	if(tmpNode = FindNode(curNode, "cm_cfg"))
		update_cms_config_table(cms, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		update_cms_version(cms, tmpNode);

	xmlSaveFormatFileEnc(cms_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	ALOGD("this is pq\n");
	return 0;
}


int CmsParserLiteR2p0::parse_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	int ret;
	cms_common_sharkl5 *cms;

	cms = &((pq_tuning_parm_sharkl5 *)ctx)->cms;
	doc = xmlReadFile(cms_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
		ALOGD("Document not parsed successfully.\n");
		return 1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"root")) {
		ALOGD("root node != root\n");
		xmlFreeDoc(doc);
		return 1;
	}

	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "hsv_cm_epf_slp_ltm"))
		parse_cms_hsvcm_epf_slp_ltm(cms, tmpNode);
	if(tmpNode = FindNode(curNode, "rgb_mapping_table"))
		parse_cms_rgbmap_table(cms, tmpNode);
	if(tmpNode = FindNode(curNode, "cm_cfg"))
		parse_cms_config_table(cms, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		parse_cms_version(cms, tmpNode);

	ret = xmlSaveFormatFileEnc(cms_xml, doc, "UTF-8", 1);
	if (ret == -1)
		ret = xmlSaveFormatFileEnc(cms_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);

	if (ret == -1)
		return 1;

	return 0;
}
