#include <utils/Log.h>
#include "ParserCore.h"

#define PARSE_BLD_CM(i, X) \
({  \
	     if (xmlHasProp(propNode, BAD_CAST #X)) { \
	         szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
	         bld->cmrange.cm.X = strtoul((char *)szPropity, NULL, 0); \
		 xmlFree(szPropity); \
	         propNode = propNode->next; \
	     }   \
})

#define UPDATE_BLD_CM(i, X) \
({  \
	 if (xmlHasProp(propNode, BAD_CAST #X)) {    \
	 snprintf(numStr, sizeof(numStr), "%d", bld->cmrange.cm.X); \
	 xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr);  \
	 propNode = propNode->next; \
	 }   \
})

#define PARSE_BLD_CM_ARRAYS(i) \
	({ \
	PARSE_BLD_CM(i, coef00); \
	PARSE_BLD_CM(i, coef01); \
	PARSE_BLD_CM(i, coef02); \
	PARSE_BLD_CM(i, coef03); \
	PARSE_BLD_CM(i, coef10); \
	PARSE_BLD_CM(i, coef11); \
	PARSE_BLD_CM(i, coef12); \
	PARSE_BLD_CM(i, coef13); \
	PARSE_BLD_CM(i, coef20); \
	PARSE_BLD_CM(i, coef21); \
	PARSE_BLD_CM(i, coef22); \
	PARSE_BLD_CM(i, coef23); \
})

#define UPDATE_BLD_CM_ARRAYS(i) \
({ \
	UPDATE_BLD_CM(i, coef00); \
	UPDATE_BLD_CM(i, coef01); \
	UPDATE_BLD_CM(i, coef02); \
	UPDATE_BLD_CM(i, coef03); \
	UPDATE_BLD_CM(i, coef10); \
	UPDATE_BLD_CM(i, coef11); \
	UPDATE_BLD_CM(i, coef12); \
	UPDATE_BLD_CM(i, coef13); \
	UPDATE_BLD_CM(i, coef20); \
	UPDATE_BLD_CM(i, coef21); \
	UPDATE_BLD_CM(i, coef22); \
	UPDATE_BLD_CM(i, coef23); \
})


#define PARSE_BLD_RANGE(i, X) \
({  \
	if (xmlHasProp(propNode, BAD_CAST #X)) { \
		szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
	    	bld->cmrange.range.X = strtoul((char *)szPropity, NULL, 0); \
		xmlFree(szPropity); \
	    	propNode = propNode->next; \
	}   \
})

#define UPDATE_BLD_RANGE(i, X) \
({  \
	if (xmlHasProp(propNode, BAD_CAST #X)) {    \
		snprintf(numStr, sizeof(numStr), "%d", bld->cmrange.range.X); \
		xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr);  \
		ALOGD("BLD UPDATE range "#X" %d \n", bld->cmrange.range.X);  \
		propNode = propNode->next; \
	}   \
})

#define PARSE_BLD_RANGE_ARRAYS(i) \
({ \
	PARSE_BLD_RANGE(i, rmin); \
	PARSE_BLD_RANGE(i, rmax); \
	PARSE_BLD_RANGE(i, gmin); \
	PARSE_BLD_RANGE(i, gmax); \
	PARSE_BLD_RANGE(i, bmin); \
	PARSE_BLD_RANGE(i, bmax); \
})

#define UPDATE_BLD_RANGE_ARRAYS(i) \
({ \
	UPDATE_BLD_RANGE(i, rmin); \
	UPDATE_BLD_RANGE(i, rmax); \
	UPDATE_BLD_RANGE(i, gmin); \
	UPDATE_BLD_RANGE(i, gmax); \
	UPDATE_BLD_RANGE(i, bmin); \
	UPDATE_BLD_RANGE(i, bmax); \
})

static int parse_bld_version(bld_common_sharkl5 *bld, xmlNodePtr curNode)
{
    xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

    ALOGD("curNode name %s \n",curNode->name);
    subNode = curNode; //subNode table
	while(!xmlStrcmp(subNode->name, BAD_CAST"enhance")) {
		if(xmlHasProp(subNode, BAD_CAST "version")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*) "version");
			bld->version.version = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
		} else if (xmlHasProp(subNode, BAD_CAST "major_version")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*) "major_version");
			bld->nMajorVersion = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
		}
		subNode = subNode->next;
	}
    return 0;
}

static int parse_cmrange(bld_common_sharkl5 *bld, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	char numStr[10];

	while(NULL != curNode) {
		if (!xmlStrcmp(curNode->name, (const xmlChar*)"cm")) {
			propNode = curNode->children;
			PARSE_BLD_CM_ARRAYS(0);
		} else if (!xmlStrcmp(curNode->name, (const xmlChar*)"range")) {
			propNode = curNode->children;
			PARSE_BLD_RANGE_ARRAYS(0);
		}
		curNode = curNode->next;
	}

	return 0;
}

static int parse_bld_cmrange_config(bld_common_sharkl5 *bld, xmlNodePtr curNode)
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
				parse_cmrange(bld, subNode->children);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int update_bld_version(bld_common_sharkl5 *bld, xmlNodePtr curNode)
{
	int i = 0;
	xmlNodePtr subNode;
	char numStr[12];

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode; //subNode table
	while(!xmlStrcmp(subNode->name, BAD_CAST"enhance")) {
		if(xmlHasProp(subNode, BAD_CAST "version")) {
			snprintf(numStr, sizeof(numStr), "%d", bld->version.version);
			xmlSetProp(subNode, BAD_CAST "version", (const xmlChar*)numStr);
			ALOGD("pqpqpqcurNode name %s %d\n", numStr,bld->version.version);
		}
		subNode = subNode->next;
	}
	return 0;
}

static int update_cmrange(bld_common_sharkl5 *bld, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	char numStr[10];

	while(NULL != curNode) {
		if (!xmlStrcmp(curNode->name, (const xmlChar*)"cm")) {
			propNode = curNode->children;
			UPDATE_BLD_CM_ARRAYS(0);
		} else if (!xmlStrcmp(curNode->name, (const xmlChar*)"range")) {
			propNode = curNode->children;
			UPDATE_BLD_RANGE_ARRAYS(0);
		}
		curNode = curNode->next;
	}

	return 0;
}

static int update_bld_cmrange_config(bld_common_sharkl5 *bld, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char *endptr = NULL;
	char numStr[10];
	int i = 0;

	subNode = curNode->children; //subNode table
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "default")) {
				xmlFree(szPropity);
				update_cmrange(bld, subNode->children);
			}
		}
		i++;
		subNode = subNode->next;
	}

	return 0;
}

static int update_bld_cm_config(bld_common_sharkl5 *bld, xmlNodePtr curNode)
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
				update_cmrange(bld, subNode->children);
			}
		}
		i++;
		subNode = subNode->next;
	}
	return 0;
}

int BldParserLiteR2p0::parse_reg(uint08_t *ctx)
{
	int fd0, fd1;
	unsigned int cnt0,cnt1;
	unsigned int sizes1;
	unsigned char* data;
	bld_common_sharkl5 *bld;

	bld = &((pq_tuning_parm_sharkl5 *)ctx)->bld;

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
	sizes1 = sizeof(bld->cmrange.cm);
	cnt1 = read(fd1, (unsigned char *)&bld->cmrange.cm, sizes1);
	close(fd0);
	close(fd1);

	if (cnt1 == sizes1)
		return 0;
	else
		return -1;
}

int BldParserLiteR2p0::update_reg(uint08_t *ctx)
{
	int fd0, fd1, fd2;
	unsigned int sizes1;
	unsigned char* data;
	unsigned int disable;
	bld_common_sharkl5 *bld;

	bld = &((pq_tuning_parm_sharkl5 *)ctx)->bld;
	ALOGD("PQ BLD  %s enalbe %d\n", __func__, bld->version.enable);
	if(bld->version.enable) {

		ALOGD("PQ BLD  %s\n", __func__);
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
		sizes1 = sizeof(bld->cmrange.cm);
		write(fd1, (unsigned char *)&bld->cmrange.cm, sizes1);
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

int BldParserLiteR2p0::parse_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	bld_common_sharkl5 *bld;

	bld = &((pq_tuning_parm_sharkl5 *)ctx)->bld;

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

	if(tmpNode = FindNode(curNode, "cm_config"))
		parse_bld_cmrange_config(bld, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		parse_bld_version(bld, tmpNode);

	xmlSaveFormatFileEnc(bld_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	return 0;

}

int BldParserLiteR2p0::update_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	int ret;
	bld_common_sharkl5 *bld;

	bld = &((pq_tuning_parm_sharkl5 *)ctx)->bld;

	doc = xmlReadFile(bld_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
        	ALOGD("Document not parsed successfully.\n");
		return 1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"root"))
	{
        	ALOGD("root node != root\n");
		xmlFreeDoc(doc);
		return 1;
	} else
		ALOGD("root node is root\n");

	ALOGD("curNode name %s \n",curNode->name);
	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "enhance"))
		update_bld_version(bld, tmpNode);
	if(tmpNode = FindNode(curNode, "cm_config"))
		update_bld_cmrange_config(bld, tmpNode);

	ret = xmlSaveFormatFileEnc(bld_xml, doc, "UTF-8", 1);
	if (ret == -1)
		xmlSaveFormatFileEnc(bld_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);

	if (ret == -1)
		return 1;
	return 0;
}
