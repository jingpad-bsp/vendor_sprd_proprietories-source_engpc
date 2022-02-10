#include <utils/Log.h>
#include "ParserCore.h"

#define PARSE_BLD_HSVCM(i, X) \
({	\
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
		bld->hsvcm[i].cm.X = strtoul((char *)szPropity, NULL, 0); \
		ALOGD(""#X" %d \n", bld->hsvcm[i].cm.X); \
		xmlFree(szPropity); \
		propNode = propNode->next; \
	}	\
})

#define UPDATE_BLD_HSVCM(i, X) \
({	\
	if (xmlHasProp(propNode, BAD_CAST #X)) {	\
		snprintf(numStr, sizeof(numStr), "%d", bld->hsvcm[i].cm.X);	\
		xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr);	\
		ALOGD(""#X" %d \n", bld->hsvcm[i].cm.X); \
		propNode = propNode->next; \
	}	\
})


#define PARSE_CM_HSVCM_ARRAYS(i) \
({ \
	PARSE_BLD_HSVCM(i, coef00); \
	PARSE_BLD_HSVCM(i, coef01); \
	PARSE_BLD_HSVCM(i, coef02); \
	PARSE_BLD_HSVCM(i, coef03); \
	PARSE_BLD_HSVCM(i, coef10); \
	PARSE_BLD_HSVCM(i, coef11); \
	PARSE_BLD_HSVCM(i, coef12); \
	PARSE_BLD_HSVCM(i, coef13); \
	PARSE_BLD_HSVCM(i, coef20); \
	PARSE_BLD_HSVCM(i, coef21); \
	PARSE_BLD_HSVCM(i, coef22); \
	PARSE_BLD_HSVCM(i, coef23); \
})

#define UPDATE_CM_HSVCM_ARRAYS(i) \
({ \
	UPDATE_BLD_HSVCM(i, coef00); \
	UPDATE_BLD_HSVCM(i, coef01); \
	UPDATE_BLD_HSVCM(i, coef02); \
	UPDATE_BLD_HSVCM(i, coef03); \
	UPDATE_BLD_HSVCM(i, coef10); \
	UPDATE_BLD_HSVCM(i, coef11); \
	UPDATE_BLD_HSVCM(i, coef12); \
	UPDATE_BLD_HSVCM(i, coef13); \
	UPDATE_BLD_HSVCM(i, coef20); \
	UPDATE_BLD_HSVCM(i, coef21); \
	UPDATE_BLD_HSVCM(i, coef22); \
	UPDATE_BLD_HSVCM(i, coef23); \
})


static int parse_hsvcm(bld_common *bld, xmlNodePtr subNode, int i)
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
				bld->hsvcm[i].hsv.table[j].hue = strtoul((char *)szPropity, NULL, 0);
				ALOGD("hue %d \n", bld->hsvcm[i].hsv.table[j].hue);
				xmlFree(szPropity);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"sat")) {
				szPropity = xmlGetProp(propNode, (const xmlChar*)"sat");
				bld->hsvcm[i].hsv.table[j].sat = strtoul((char *)szPropity, NULL, 0);
				ALOGD("sat %d \n", bld->hsvcm[i].hsv.table[j].sat);
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

static int parse_bld_version(bld_common *bld, xmlNodePtr curNode)
{
    xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

    ALOGD("curNode name %s \n",curNode->name);
    subNode = curNode; //subNode table
	while(!xmlStrcmp(subNode->name, (const xmlChar*)"enhance")) {
        if(xmlHasProp(subNode, BAD_CAST (const xmlChar*)"version")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*) "version");
			bld->version.version = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
		}
		subNode = subNode->next;
	}
    return 0;
}

static int parse_bld_hsvcm(bld_common *bld, xmlNodePtr curNode)
{
	int i = 0;
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
			if(!xmlStrcmp(szPropity, (const xmlChar *) "default")) {
				xmlFree(szPropity);
				parse_hsvcm(bld, subNode->children, i);
			}
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "middle")) {
				xmlFree(szPropity);
				parse_hsvcm(bld, subNode->children, i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "high")) {
				xmlFree(szPropity);
				parse_hsvcm(bld, subNode->children, i);
			}
		}
		i++;
		subNode = subNode->next;
	}
	return 0;
}

static int update_hsvcm(bld_common *bld, xmlNodePtr subNode, int i)
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
				ALOGD("hue %d \n", bld->hsvcm[i].hsv.table[j].hue);
				snprintf(numStr, sizeof(numStr), "%d", bld->hsvcm[i].hsv.table[j].hue);
				xmlSetProp(propNode, BAD_CAST "hue", (const xmlChar*)numStr);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"sat")) {
				ALOGD("sat %d \n", bld->hsvcm[i].hsv.table[j].sat);
				snprintf(numStr, sizeof(numStr), "%d", bld->hsvcm[i].hsv.table[j].sat);
				xmlSetProp(propNode, BAD_CAST "sat", (const xmlChar*)numStr);
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

static int update_bld_version(bld_common *bld, xmlNodePtr curNode)
{
	int i = 0;
	xmlNodePtr subNode;
	char numStr[12];

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode; //subNode table
	while(!xmlStrcmp(subNode->name, (const xmlChar*)"enhance")) {
		if(xmlHasProp(subNode, BAD_CAST "version")) {
			snprintf(numStr, sizeof(numStr), "%d", bld->version.version);
			xmlSetProp(subNode, BAD_CAST "version", (const xmlChar*)numStr);
		}
		subNode = subNode->next;
	}
	return 0;
}

static int update_bld_hsvcm(bld_common *bld, xmlNodePtr curNode)
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
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "default")) {
				xmlFree(szPropity);
				update_hsvcm(bld, subNode->children, i);
			}
			else if (!xmlStrcmp(szPropity, (const xmlChar *) "middle")) {
				xmlFree(szPropity);
				update_hsvcm(bld, subNode->children, i);
			}
			else if(!xmlStrcmp(szPropity, (const xmlChar *) "high")) {
				xmlFree(szPropity);
				update_hsvcm(bld, subNode->children, i);
			}
		}
		i++;
		subNode = subNode->next;
	}
	return 0;
}

int BldParser::parse_reg(uint08_t *ctx)
{
	int fd0, fd1;
	uint32_t cnt0, cnt1;
	uint32_t sizes0,sizes1;
	uint08_t* data;
	bld_common *bld;

	bld = &((pq_tuning_parm *)ctx)->bld;

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
	sizes0 = sizeof(bld->hsvcm[0].hsv);
	sizes1 = sizeof(bld->hsvcm[0].cm);
	cnt0 = read(fd0, (uint08_t *)&bld->hsvcm[0].hsv, sizes0);
	cnt1 = read(fd1, (uint08_t *)&bld->hsvcm[0].cm, sizes1);
	close(fd0);
	close(fd1);

	if ((cnt0 == sizes0)&&(cnt1 == sizes1))
		return 0;
	else
		return -1;
}

int BldParser::update_reg(uint08_t *ctx)
{
	int fd0, fd1, fd2;
	uint32_t sizes0,sizes1;
	uint08_t* data;
	uint32_t disable;
	bld_common *bld;

	bld = &((pq_tuning_parm *)ctx)->bld;
	ALOGD("PQ BLD  %s enalbe %d\n", __func__, bld->version.enable);
	if(bld->version.enable) {

		ALOGD("PQ BLD  %s\n", __func__);
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
		sizes0 = sizeof(bld->hsvcm[0].hsv);
		sizes1 = sizeof(bld->hsvcm[0].cm);
		write(fd0, (uint08_t *)&bld->hsvcm[0].hsv, sizes0);
		write(fd1, (uint08_t *)&bld->hsvcm[0].cm, sizes1);
		close(fd0);
		close(fd1);
	} else {
		fd2 = open(PQDisable, O_WRONLY);
		if(fd2 < 0) {
			ALOGD("%s: fd2 open file failed, err: %s\n", __func__, strerror(errno));
			return errno;
		}
		disable = HSV_EN | CMS_EN;
		write(fd2, &disable, sizeof(disable));
		close(fd2);
	}
	return 0;
}

int BldParser::parse_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	bld_common *bld;

	bld = &((pq_tuning_parm *)ctx)->bld;

	doc = xmlReadFile(bld_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
        ALOGD("Document not parsed successfully.\n");
        return -1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"root"))
	{
		ALOGD("root node != root\n");
		xmlFreeDoc(doc);
        return -1;
	} else
		ALOGD("root node is root\n");

	ALOGD("curNode name %s \n",curNode->name);
	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "hsv_cm"))
		parse_bld_hsvcm(bld, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		parse_bld_version(bld, tmpNode);

	xmlSaveFormatFileEnc(bld_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	return 0;

}

int BldParser::update_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	bld_common *bld;

	bld = &((pq_tuning_parm *)ctx)->bld;

	doc = xmlReadFile(bld_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
        ALOGD("Document not parsed successfully.\n");
        return -1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"root"))
	{
        ALOGD("root node != root\n");
		xmlFreeDoc(doc);
        return -1;
	} else
		ALOGD("root node is root\n");

	ALOGD("curNode name %s \n",curNode->name);
	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "enhance"))
		update_bld_version(bld, tmpNode);
	if(tmpNode = FindNode(curNode, "hsv_cm"))
		update_bld_hsvcm(bld, tmpNode);

	xmlSaveFormatFileEnc(bld_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	return 0;

}
