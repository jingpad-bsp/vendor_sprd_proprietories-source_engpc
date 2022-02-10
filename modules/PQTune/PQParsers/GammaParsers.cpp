#include <utils/Log.h>
#include "ParserCore.h"

static int parse_gamma_version(gamma_common *gamma, xmlNodePtr curNode)
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
			ALOGD("aaaaaa gamma verison %d \n", gamma->version.version);
			xmlFree(szPropity);
		}
		subNode = subNode->next;
	}
	return 0;
}

static int parse_gamma_regs_table(gamma_common *gamma, xmlNodePtr curNode)
{
	int i = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode; //subNode table
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
			propNode = subNode->children;
			while (NULL != propNode) {
				attrPtr = propNode->properties;
				while (NULL != attrPtr) {
                	if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"r")) {
                    	szPropity = xmlGetProp(propNode, (const xmlChar*)"r");
						gamma->gamma.r[i] = strtoul((char *)szPropity, NULL, 0);
						ALOGD("r %d \n", gamma->gamma.r[i]);
						xmlFree(szPropity);
					} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"g")) {
						szPropity = xmlGetProp(propNode, (const xmlChar*)"g");
						gamma->gamma.g[i] = strtoul((char *)szPropity, NULL, 0);
						ALOGD("g %d \n", gamma->gamma.g[i]);
						xmlFree(szPropity);
					} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"b")) {
						szPropity = xmlGetProp(propNode, (const xmlChar*)"b");
						gamma->gamma.b[i] = strtoul((char *)szPropity, NULL, 0);
						ALOGD("b %d \n", gamma->gamma.b[i]);
						xmlFree(szPropity);
					}
					attrPtr = attrPtr->next;
            	}
				i++;
				propNode = propNode->next;
			}
		}
		i = 0;
		subNode = subNode->next;
	}

	return 0;
}

static int update_gamma_version(gamma_common *gamma, xmlNodePtr curNode)
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

static int update_gamma_regs_table(gamma_common *gamma, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode; //subNode table
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
			propNode = subNode->children;
			while (NULL != propNode) {
				attrPtr = propNode->properties;
				while (NULL != attrPtr) {
                	if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"r")) {
						snprintf(numStr, sizeof(numStr), "%d", gamma->gamma.r[i]);
						xmlSetProp(propNode, BAD_CAST "r", (const xmlChar*)numStr);
						ALOGD("r %d \n", gamma->gamma.r[i]);
					} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"g")) {
						snprintf(numStr, sizeof(numStr), "%d", gamma->gamma.g[i]);
						xmlSetProp(propNode, BAD_CAST "g", (const xmlChar*)numStr);
					} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"b")) {
						snprintf(numStr, sizeof(numStr), "%d", gamma->gamma.b[i]);
						xmlSetProp(propNode, BAD_CAST "b", (const xmlChar*)numStr);
					}
					attrPtr = attrPtr->next;
            	}
				i++;
				propNode = propNode->next;
			}
		}
		i = 0;
		subNode = subNode->next;
	}

	return 0;
}

int GammaParser::parse_reg(uint08_t *ctx)
{
	int fd;
	uint32_t sizes;
	uint08_t* data;
	int cnt;
	gamma_common *gamma;

	gamma = &((pq_tuning_parm *)ctx)->gamma;

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

int GammaParser::update_reg(uint08_t *ctx)
{
	int fd;
	int fd1;
	uint32_t sizes;
	uint08_t* data;
	int cnt;
	uint32_t disable;
	gamma_common *gamma;

	gamma = &((pq_tuning_parm *)ctx)->gamma;

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


int GammaParser::update_xml(uint08_t *ctx)
{

	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	gamma_common *gamma;

	gamma = &((pq_tuning_parm *)ctx)->gamma;

	doc = xmlReadFile(gamma_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
		ALOGD("Document not parsed successfully.\n");
		return -1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"gamma_config"))
	{
		ALOGD("gamma_config node != root\n");
		xmlFreeDoc(doc);
		return -1;
	}

	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "reg_table"))
		update_gamma_regs_table(gamma, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		update_gamma_version(gamma, tmpNode);

	xmlSaveFormatFileEnc(gamma_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	return 0;
}

int GammaParser::parse_xml(uint08_t *ctx)
{

	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	gamma_common *gamma;

	gamma = &((pq_tuning_parm *)ctx)->gamma;

	doc = xmlReadFile(gamma_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
		ALOGD("Document not parsed successfully.\n");
		return -1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"gamma_config")) {
		ALOGD("gamma_config node != root\n");
		xmlFreeDoc(doc);
		return -1;
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
