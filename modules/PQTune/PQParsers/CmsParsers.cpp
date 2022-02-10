#include <utils/Log.h>
#include "ParserCore.h"

#define PARSE_CMS_HSVCM(i, X) \
({	\
	if (xmlHasProp(propNode, BAD_CAST #X)) {	\
		szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
		cms->hsvcm[i].cm.X = strtoul((char *)szPropity, NULL, 0); \
		ALOGD(""#X" %d \n", cms->hsvcm[i].cm.X); \
		xmlFree(szPropity); \
		propNode = propNode->next; \
	}	\
})

#define PARSE_CMS_CM(i, X) \
({	\
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
		cms->cm[i].X = strtoul((char *)szPropity, NULL, 0);	\
		ALOGD(""#X" %d \n", cms->cm[i].X); \
		xmlFree(szPropity); \
		propNode = propNode->next; \
	} \
})

#define UPDATE_CMS_HSVCM(i, X) \
({	\
	if (xmlHasProp(propNode, BAD_CAST #X)) {	\
		snprintf(numStr, sizeof(numStr), "%d", cms->hsvcm[i].cm.X);	\
		xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr);	\
		ALOGD(""#X" %d \n", cms->hsvcm[i].cm.X);	\
		propNode = propNode->next; \
	}	\
})

#define UPDATE_CMS_CM(i, X) \
({	\
	if (xmlHasProp(propNode, BAD_CAST #X)) {	\
		snprintf(numStr, sizeof(numStr), "%d", cms->cm[i].X);	\
		xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr);	\
		ALOGD(""#X" %d \n", cms->cm[i].X);	\
		propNode = propNode->next; \
	} \
})


#define PARSE_CM_HSVCM_ARRAYS(i) \
({ \
	PARSE_CMS_HSVCM(i, coef00); \
	PARSE_CMS_HSVCM(i, coef01); \
	PARSE_CMS_HSVCM(i, coef02); \
	PARSE_CMS_HSVCM(i, coef03); \
	PARSE_CMS_HSVCM(i, coef10); \
	PARSE_CMS_HSVCM(i, coef11); \
	PARSE_CMS_HSVCM(i, coef12); \
	PARSE_CMS_HSVCM(i, coef13); \
	PARSE_CMS_HSVCM(i, coef20); \
	PARSE_CMS_HSVCM(i, coef21); \
	PARSE_CMS_HSVCM(i, coef22); \
	PARSE_CMS_HSVCM(i, coef23); \
})

#define PARSE_CM_CONFIG_ARRAYS(i) \
({ \
	PARSE_CMS_CM(i, coef00); \
	PARSE_CMS_CM(i, coef01); \
	PARSE_CMS_CM(i, coef02); \
	PARSE_CMS_CM(i, coef03); \
	PARSE_CMS_CM(i ,coef10); \
	PARSE_CMS_CM(i, coef11); \
	PARSE_CMS_CM(i, coef12); \
	PARSE_CMS_CM(i, coef13); \
	PARSE_CMS_CM(i, coef20); \
	PARSE_CMS_CM(i, coef21); \
	PARSE_CMS_CM(i, coef22); \
	PARSE_CMS_CM(i, coef23); \
})


#define UPDATE_CM_CONFIG_ARRAYS(i) \
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

#define UPDATE_CM_HSVCM_ARRAYS(i) \
({ \
	UPDATE_CMS_HSVCM(i, coef00); \
	UPDATE_CMS_HSVCM(i, coef01); \
	UPDATE_CMS_HSVCM(i, coef02); \
	UPDATE_CMS_HSVCM(i, coef03); \
	UPDATE_CMS_HSVCM(i, coef10); \
	UPDATE_CMS_HSVCM(i, coef11); \
	UPDATE_CMS_HSVCM(i, coef12); \
	UPDATE_CMS_HSVCM(i, coef13); \
	UPDATE_CMS_HSVCM(i, coef20); \
	UPDATE_CMS_HSVCM(i, coef21); \
	UPDATE_CMS_HSVCM(i, coef22); \
	UPDATE_CMS_HSVCM(i, coef23); \
})


static int parse_hsvcm(cms_common *cms, xmlNodePtr subNode, int i)
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
				ALOGD("parse_hue j %d hue %d \n", j, cms->hsvcm[i].hsv.table[j].hue);
				xmlFree(szPropity);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"sat")) {
				szPropity = xmlGetProp(propNode, (const xmlChar*)"sat");
				cms->hsvcm[i].hsv.table[j].sat = strtoul((char *)szPropity, NULL, 0);
				ALOGD("sat %d \n", cms->hsvcm[i].hsv.table[j].sat);
				xmlFree(szPropity);
			}
			attrPtr = attrPtr->next;
		}
		j++;
		propNode = propNode->next;
	}

	propNode = subNode->next->children;
	PARSE_CM_HSVCM_ARRAYS(i);

	return 0;
}

static int parse_cms_version(cms_common *cms, xmlNodePtr curNode)
{
	xmlNodePtr subNode;
	xmlChar* szPropity;
    const char *endptr = NULL;

    ALOGD("curNode name %s \n",curNode->name);
    subNode = curNode; //subNode table
	while(!xmlStrcmp(subNode->name, BAD_CAST"enhance")) {
        if(xmlHasProp(subNode, BAD_CAST "version")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*) "version");
            cms->version.version = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
        }
		subNode = subNode->next;
	}
        return 0;
}

static int parse_cms_hsvcm(cms_common *cms, xmlNodePtr curNode)
{
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode->children; //subNode table
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "auto")) {
				xmlFree(szPropity);
				ALOGD("PQ CMS coef auto\n");
				parse_hsvcm(cms, subNode->children, 0);
			}
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "enhance")) {
				xmlFree(szPropity);
				ALOGD("PQ CMS enhance coef\n");
				parse_hsvcm(cms, subNode->children, 1);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "standard")) {
				xmlFree(szPropity);
				ALOGD("PQ CMS coef standard\n");
				parse_hsvcm(cms, subNode->children, 2);
			}
		}
		subNode = subNode->next;
	}
	return 0;
}

static int parse_cms_rgbmap_table(cms_common *cms, xmlNodePtr curNode)
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


static int parse_cms_config_table(cms_common *cms, xmlNodePtr curNode)
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
				PARSE_CM_CONFIG_ARRAYS(i);
			}
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "warm")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "0")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
				ALOGD("PQ auto_auto read 0 \n");
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "1")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
				ALOGD("PQ auto_auto 1 read \n");
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "2")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "3")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "4")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "5")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "6")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "7")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "8")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "9")) {
				xmlFree(szPropity);
				PARSE_CM_CONFIG_ARRAYS(i);
			}
		}
		i++;
		subNode = subNode->next;
		if(subNode)
			propNode = subNode->children;
	}

	ALOGD("cm_config finish0 %s \n",curNode->name);
	return 0;
}

static int update_hsvcm(cms_common *cms, xmlNodePtr subNode, int i)
{
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	int j = 0;
	char numStr[10];

	propNode = subNode->children;
	while (NULL != propNode) {
		attrPtr = propNode->properties;
		while (NULL != attrPtr) {
			if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"hue")) {
				snprintf(numStr, sizeof(numStr), "%d", cms->hsvcm[i].hsv.table[j].hue);
				//snprintf(numStr, sizeof(numStr), "%d", 100);
				xmlSetProp(propNode, BAD_CAST "hue", (const xmlChar*)numStr);
				ALOGD("hue %d \n", cms->hsvcm[i].hsv.table[j].hue);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"sat")) {
				snprintf(numStr, sizeof(numStr), "%d", cms->hsvcm[i].hsv.table[j].sat);
				//snprintf(numStr, sizeof(numStr), "%d", 100);
				xmlSetProp(propNode, BAD_CAST "sat", (const xmlChar*)numStr);
				ALOGD("sat %d \n", cms->hsvcm[i].hsv.table[j].sat);
			}
			attrPtr = attrPtr->next;
 		}
		j++;
		propNode = propNode->next;
	}
	propNode = subNode->next->children;
	UPDATE_CM_HSVCM_ARRAYS(i);

	return 0;
}

static int update_cms_version(cms_common *cms, xmlNodePtr curNode)
{
        int i = 0;
        xmlNodePtr subNode;
        char numStr[12];

        ALOGD("curNode name %s \n",curNode->name);
        subNode = curNode; //subNode table
		while(!xmlStrcmp(subNode->name, BAD_CAST"enhance")) {
			if(xmlHasProp(subNode, BAD_CAST "version")) {
				snprintf(numStr, sizeof(numStr), "%d", cms->version.version);
				xmlSetProp(subNode, BAD_CAST "version", (const xmlChar*)numStr);
			}
			subNode = subNode->next;
		}

        return 0;
}

static int update_cms_hsvcm(cms_common *cms, xmlNodePtr curNode)
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
				ALOGD("PQ wr CMS coef auto\n");
				update_hsvcm(cms, subNode->children, 0);
			}
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "enhance")) {
				ALOGD("PQ wr CMS coef enhance\n");
				update_hsvcm(cms, subNode->children, 1);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "standard")) {
				ALOGD("PQ wr CMS coef standard\n");
				update_hsvcm(cms, subNode->children, 2);
			}
		}
		subNode = subNode->next;
	}
	return 0;
}

static int update_cms_rgbmap_table(cms_common *cms, xmlNodePtr curNode)
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


static int update_cms_config_table(cms_common *cms, xmlNodePtr curNode)
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
				UPDATE_CM_CONFIG_ARRAYS(i);
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "warm"))
				UPDATE_CM_CONFIG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "0")) {
				UPDATE_CM_CONFIG_ARRAYS(i);
				ALOGD("PQ auto_auto 0\n");
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "1")) {
				UPDATE_CM_CONFIG_ARRAYS(i);
				ALOGD("PQ auto_auto 1\n");
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "2"))
				UPDATE_CM_CONFIG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "3"))
				UPDATE_CM_CONFIG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "4"))
				UPDATE_CM_CONFIG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "5"))
				UPDATE_CM_CONFIG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "6"))
				UPDATE_CM_CONFIG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "7"))
				UPDATE_CM_CONFIG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "8"))
				UPDATE_CM_CONFIG_ARRAYS(i);
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "9"))
				UPDATE_CM_CONFIG_ARRAYS(i);
		}
		i++;
		subNode = subNode->next;
		if(subNode)
			propNode = subNode->children;
	}

	ALOGD("cm_config finish0 %s \n",curNode->name);
	return 0;
}

int CmsParser::parse_reg(uint08_t *ctx)
{
	int fd0, fd1;
	uint32_t sizes0,sizes1;
	uint08_t* data;
	int cnt;
	cms_common *cms;

	cms = &((pq_tuning_parm *)ctx)->cms;

	fd0 = open(DpuHsv, O_RDWR);
	fd1 = open(DpuCm, O_RDWR);
	if(fd0 < 0 || fd1 < 0) {
		ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
		if (fd0 >= 0)
			close(fd0);
		if (fd1 >= 0)
			close(fd1);
		return errno;
	}
	sizes0 = sizeof(cms->hsvcm[0].hsv);
	sizes1 = sizeof(cms->hsvcm[0].cm);
	ALOGD("parse_cms_reg sizes0 %d: size1 %d\n", sizes0, sizes1);
	cnt = read(fd0, &(cms->hsvcm[0].hsv), sizes0);
	ALOGD("parse_cms_reg cnt0 %d \n", cnt);
	cnt = read(fd1, &(cms->hsvcm[0].cm), sizes1);
	ALOGD("parse_cms_reg cnt %d\n", cnt);
	close(fd0);
	close(fd1);

	return 0;
}

int CmsParser::update_reg(uint08_t *ctx)
{
	int fd0, fd1, fd2;
	uint32_t sizes0,sizes1;
	uint08_t* data;
	uint32_t disable;
	cms_common *cms;

	cms = &((pq_tuning_parm *)ctx)->cms;

	if(cms->version.enable) {
		fd0 = open(DpuHsv, O_RDWR);
		fd1 = open(DpuCm, O_RDWR);
		if(fd0 < 0 || fd1 < 0) {
			if (fd0 >= 0)
				close(fd0);
			if (fd1 >= 0)
				close(fd1);
			ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
			return errno;
		}
		sizes0 = sizeof(cms->hsvcm[0].hsv);
		sizes1 = sizeof(cms->hsvcm[0].cm);
		write(fd0, (uint08_t *)&cms->hsvcm[0].hsv, sizes0);
		write(fd1, (uint08_t *)&cms->hsvcm[0].cm, sizes1);
		close(fd0);
		close(fd1);
	} else {
		fd2 = open(PQDisable, O_WRONLY);
		if(fd2 < 0) {
			ALOGD("%s: fd2 open file failed, err: %s\n", __func__, strerror(errno));
			return errno;
		}
		disable = CMS_EN | HSV_EN;
		write(fd2, &disable, sizeof(disable));
		close(fd2);
	}

	return 0;
}

int CmsParser::update_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	cms_common *cms;

	cms = &((pq_tuning_parm *)ctx)->cms;

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

	if(tmpNode = FindNode(curNode, "hsv_cm"))
		update_cms_hsvcm(cms, tmpNode);
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


int CmsParser::parse_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	cms_common *cms;

	cms = &((pq_tuning_parm *)ctx)->cms;

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

	if(tmpNode = FindNode(curNode, "hsv_cm"))
		parse_cms_hsvcm(cms, tmpNode);
	if(tmpNode = FindNode(curNode, "rgb_mapping_table"))
		parse_cms_rgbmap_table(cms, tmpNode);
	if(tmpNode = FindNode(curNode, "cm_cfg"))
		parse_cms_config_table(cms, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		parse_cms_version(cms, tmpNode);

	xmlSaveFormatFileEnc(cms_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	ALOGD("this is pq\n");
	return 0;
}
