#include <utils/Log.h>
#include "ParserCore.h"

static int parse_gamma_version(gamma_common_sharkl5 *gamma, xmlNodePtr curNode)
{
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode; //subNode table
	while(!xmlStrcmp(subNode->name, BAD_CAST"enhance")) {
		if(xmlHasProp(subNode, BAD_CAST "version")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*) "version");
			gamma->version.version = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
		} else if (xmlHasProp(subNode, BAD_CAST "major_version")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*) "major_version");
			gamma->nMajorVersion = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
		}
		subNode = subNode->next;
	}
	return 0;
}

static int parse_gamma_child_config(gamma_common_sharkl5 *gamma, xmlNodePtr curNode , int i)
{
	xmlNodePtr propNode;
	xmlChar* szPropity;
	xmlAttrPtr attrPtr;
	const char *endptr = NULL;
	int j = 0;

	propNode = curNode->children;
	while (NULL != propNode) {
		attrPtr = propNode->properties;
		while (NULL != attrPtr) {
			if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"r")) {
				szPropity = xmlGetProp(propNode, (const xmlChar*)"r");
				gamma->gamma[i].r[j] = strtoul((char *)szPropity, NULL, 0);
				xmlFree(szPropity);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"g")) {
				szPropity = xmlGetProp(propNode, (const xmlChar*)"g");
				gamma->gamma[i].g[j] = strtoul((char *)szPropity, NULL, 0);
				xmlFree(szPropity);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"b")) {
				szPropity = xmlGetProp(propNode, (const xmlChar*)"b");
				gamma->gamma[i].b[j] = strtoul((char *)szPropity, NULL, 0);
				xmlFree(szPropity);
			}
			attrPtr = attrPtr->next;
		}
		j++;
		propNode = propNode->next;
	}

	return 0;
}

static int parse_gamma_regs_table(gamma_common_sharkl5 *gamma, xmlNodePtr curNode)
{
	xmlNodePtr subNode;
	xmlChar* szPropity;

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode; //subNode table
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "default")) {
				xmlFree(szPropity);
				parse_gamma_child_config(gamma, subNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "standard")) {
				xmlFree(szPropity);
				parse_gamma_child_config(gamma, subNode, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int update_gamma_version(gamma_common_sharkl5 *gamma, xmlNodePtr curNode)
{
	int i = 0;
	xmlNodePtr subNode;
	char numStr[12];

	ALOGD("ggaammaa version = %d \n", gamma->version.version);
	subNode = curNode; //subNode table
	while(!xmlStrcmp(subNode->name, BAD_CAST"enhance")) {
		if(xmlHasProp(subNode, BAD_CAST "version")) {
			snprintf(numStr, sizeof(numStr), "%d", gamma->version.version);
			xmlSetProp(subNode, BAD_CAST "version", (const xmlChar*)numStr);
	}
		subNode = subNode->next;
	}

	return 0;
}

static int update_gamma_child_config(gamma_common_sharkl5 *gamma, xmlNodePtr curNode , int i)
{

	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	propNode = curNode->children;
	while (NULL != propNode) {
		attrPtr = propNode->properties;
		while (NULL != attrPtr) {
			if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"r")) {
				snprintf(numStr, sizeof(numStr), "%d", gamma->gamma[i].r[j]);
				xmlSetProp(propNode, BAD_CAST "r", (const xmlChar*)numStr);
				ALOGD("r %d \n", gamma->gamma[i].r[j]);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"g")) {
				snprintf(numStr, sizeof(numStr), "%d", gamma->gamma[i].g[j]);
				xmlSetProp(propNode, BAD_CAST "g", (const xmlChar*)numStr);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"b")) {
				snprintf(numStr, sizeof(numStr), "%d", gamma->gamma[i].b[j]);
				xmlSetProp(propNode, BAD_CAST "b", (const xmlChar*)numStr);
			}
			attrPtr = attrPtr->next;
		}
		j++;
		propNode = propNode->next;
	}

	return 0;
}

static int update_gamma_regs_table(gamma_common_sharkl5 *gamma, xmlNodePtr curNode)
{
	int i = 0;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	char numStr[10];

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode; //subNode table
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
			if(xmlHasProp(subNode, BAD_CAST "mode")) {
				ALOGD("curNode name %s \n",subNode->name);
				szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
				if(!xmlStrcmp(szPropity, (const xmlChar *) "default")) {
					xmlFree(szPropity);
					update_gamma_child_config(gamma, subNode, 0);
				} else if(!xmlStrcmp(szPropity, (const xmlChar *) "standard")) {
					xmlFree(szPropity);
					update_gamma_child_config(gamma, subNode, 1);
				}
			}
		}
		i = 0;
		subNode = subNode->next;
	}

	return 0;
}

int GammaParserLiteR2p0::parse_reg(uint08_t *ctx)
{
	int fd;
	unsigned int sizes;
	unsigned char* data;
	int cnt;
	gamma_common_sharkl5 *gamma;

	gamma = &((pq_tuning_parm_sharkl5 *)ctx)->gamma;

	fd = open(DpuGamma, O_RDWR);

	if(fd < 0) {
		ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
		return errno;
	}
	sizes = sizeof(gamma->gamma);
	cnt = read(fd, &gamma->gamma, sizes);
	ALOGD("parse_gamma_reg cnt0 %d sizes %d \n", cnt, sizes);
	close(fd);

	return 0;
}

int GammaParserLiteR2p0::update_reg(uint08_t *ctx)
{
	int fd;
	int fd1;
	unsigned int sizes;
	unsigned char* data;
	int cnt;
	unsigned int disable;
	gamma_common_sharkl5 *gamma;

	gamma = &((pq_tuning_parm_sharkl5 *)ctx)->gamma;

	if(gamma->version.enable) {
		fd = open(DpuGamma, O_RDWR);
		if(fd < 0) {
			ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
			return errno;
		}
		sizes = sizeof(gamma->gamma);
		cnt = write(fd, &gamma->gamma, sizes);
		close(fd);
	}
	else {
		fd1 = open(PQDisable, O_WRONLY);
		if(fd1 < 0) {
			ALOGD("%s: open file fd1 failed, err: %s\n", __func__, strerror(errno));
			return errno;
		}
		disable = GAMMA_EN;
		write(fd1, &disable, sizeof(disable));
		close(fd1);
	}

	return 0;
}

int GammaParserLiteR2p0::update_xml(uint08_t *ctx)
{

	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	int ret;
	gamma_common_sharkl5 *gamma;

	gamma = &((pq_tuning_parm_sharkl5 *)ctx)->gamma;
	doc = xmlReadFile(gamma_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
		ALOGD("Document not parsed successfully.\n");
		return 1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"gamma_config"))
	{
		ALOGD("gamma_config node != root\n");
		xmlFreeDoc(doc);
		return 1;
	}

	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "reg_table"))
		update_gamma_regs_table(gamma, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		update_gamma_version(gamma, tmpNode);

	ret = xmlSaveFormatFileEnc(gamma_xml, doc, "UTF-8", 1);
	if (ret == -1)
		ret = xmlSaveFormatFileEnc(gamma_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);

	if (ret == -1)
		return 1;
	return 0;
}

int GammaParserLiteR2p0::parse_xml(uint08_t *ctx)
{

	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	gamma_common_sharkl5 *gamma;

	gamma = &((pq_tuning_parm_sharkl5 *)ctx)->gamma;

	doc = xmlReadFile(gamma_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
		ALOGD("Document not parsed successfully.\n");
		return 1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"gamma_config")) {
		ALOGD("gamma_config node != root\n");
		xmlFreeDoc(doc);
		return 1;
	}

	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "reg_table"))
		parse_gamma_regs_table(gamma, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		parse_gamma_version(gamma, tmpNode);

	xmlSaveFormatFileEnc(gamma_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	ALOGD("this is pq\n");
	return 0;
}
