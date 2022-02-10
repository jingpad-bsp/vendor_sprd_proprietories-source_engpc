#include <utils/Log.h>
#include "ParserCore.h"

#define PARSE_EPF_CONFIG(X, I, J) \
	({ \
	 if (xmlHasProp(propNode, BAD_CAST #X)) { \
	 szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
	 abc->sceneTable[I].sceneTableItem[J].epfCfg.X = strtoul((char *)szPropity, NULL, 0); \
	 xmlFree(szPropity); \
	 propNode = propNode->next; \
	 } \
	 })

#define PARSE_SLP_CONFIG(X, I, J) \
	({ \
	 if (xmlHasProp(propNode, BAD_CAST #X)) { \
	 szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
	 abc->sceneTable[I].sceneTableItem[J].slpCfg.X = strtoul((char *)szPropity, NULL, 0); \
	 xmlFree(szPropity); \
	 propNode = propNode->next; \
	 } \
	 })

#define PARSE_SLP_BASECONFIG(X, I) \
		 ({ \
		  if (xmlHasProp(propNode, BAD_CAST #X)) { \
		  szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
		  abc->sceneTable[I].slpbasecfg.X = strtoul((char *)szPropity, NULL, 0); \
		  xmlFree(szPropity); \
		  propNode = propNode->next; \
		  } \
		  })


#define PARSE_EPF_SP_CONFIG(X, I, J) \
	({ \
	 if (xmlHasProp(propNode, BAD_CAST #X)) { \
	 szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
	 abc->sceneTable[I].sceneTableItem[J].epfCfgSunlightProtector.X = strtoul((char *)szPropity, NULL, 0); \
	 ENG_LOG("aabbcc EPF_SP "#X" = %d \n",abc->sceneTable[I].sceneTableItem[J].epfCfgSunlightProtector.X);\
	 xmlFree(szPropity); \
	 propNode = propNode->next; \
	 } \
	 })

#define PARSE_EPF_SR_CONFIG(X, I, J) \
	({ \
	 if (xmlHasProp(propNode, BAD_CAST #X)) { \
	 szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
	 abc->sceneTable[I].sceneTableItem[J].epfCfgSuperResolution.X = strtoul((char *)szPropity, NULL, 0); \
	 xmlFree(szPropity); \
	 ENG_LOG("aabbcc EPF_SR "#X" = %d \n",abc->sceneTable[I].sceneTableItem[J].epfCfgSuperResolution.X);\
	 propNode = propNode->next; \
	 } \
	 })

#define PARSE_LTM_CONFIG(X, I, J) \
	({ \
		if (xmlHasProp(propNode, BAD_CAST #X)) { \
			szPropity = xmlGetProp(propNode, (const xmlChar*) #X); \
			abc->sceneTable[I].sceneTableItem[J].ltmCfg.X = strtoul((char *)szPropity, NULL, 0); \
			xmlFree(szPropity); \
			propNode = propNode->next; \
		} \
	 })

#define UPDATE_EPF_CONFIG(X, I, J)	\
	({	\
	 if (xmlHasProp(propNode, BAD_CAST #X)) { \
	 snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[I].sceneTableItem[J].epfCfg.X); \
	 xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
	 propNode = propNode->next; \
	 } \
	 })

#define UPDATE_EPF_SP_CONFIG(X, I, J)	\
	({	\
	 if (xmlHasProp(propNode, BAD_CAST #X)) { \
	 snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[I].sceneTableItem[J].epfCfgSunlightProtector.X); \
	 xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
	 propNode = propNode->next; \
	 } \
	 })

#define UPDATE_EPF_SR_CONFIG(X, I, J)	\
	({	\
	 if (xmlHasProp(propNode, BAD_CAST #X)) { \
	 snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[I].sceneTableItem[J].epfCfgSuperResolution.X); \
	 xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
	 propNode = propNode->next; \
	 } \
	 })

#define UPDATE_SLP_CONFIG(X, I, J) \
	({	\
	 if (xmlHasProp(propNode, BAD_CAST #X)) { \
	 snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[I].sceneTableItem[J].slpCfg.X); \
	 xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
	 propNode = propNode->next; \
	 } \
	 })

#define UPDATE_SLP_BASECONFIG(X, I) \
		 ({  \
		  if (xmlHasProp(propNode, BAD_CAST #X)) { \
		  snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[I].slpbasecfg.X); \
		  xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
		  propNode = propNode->next; \
		  } \
		  })


#define UPDATE_LTM_CONFIG(X, I) \
	 ({  \
		 if (xmlHasProp(propNode, BAD_CAST #X)) {	 \
			 snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[I].sceneTableItem[J].ltmCfg.X);	 \
			 xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr);  \
			 propNode = propNode->next; \
		 } \
	 })

#define UPDATE_SLP_CONFIG_CHILD_XML(X) \
	({ \
	childNode = xmlNewNode(NULL,BAD_CAST "param"); \
	if(!propNode) \
		xmlAddChild(subNode, childNode); \
	snprintf(numStr, sizeof(numStr), "%d", 1); \
	xmlNewProp(childNode, BAD_CAST #X, BAD_CAST numStr); \
	if (propNode) \
		xmlAddSibling(propNode, childNode); \
	propNode = childNode; \
	 })

#define UPDATE_EPF_CONFIG_CHILD_XML(X) \
	({ \
	childNode = xmlNewNode(NULL,BAD_CAST "param"); \
	if(!propNode) \
		xmlAddChild(subNode, childNode); \
	snprintf(numStr, sizeof(numStr), "%d", 1); \
	xmlNewProp(childNode, BAD_CAST #X, BAD_CAST numStr);	\
	if (propNode) \
		xmlAddSibling(propNode, childNode); \
	propNode = childNode; \
	 })

#define UPDATE_LTM_CONFIG_CHILD_XML(X) \
({ \
		childNode = xmlNewNode(NULL,BAD_CAST "param"); \
		if(!propNode) \
			xmlAddChild(subNode, childNode); \
		snprintf(numStr, sizeof(numStr), "%d", 1); \
		xmlNewProp(childNode, BAD_CAST #X, BAD_CAST numStr); \
		if (propNode) \
			xmlAddSibling(propNode, childNode); \
		propNode = childNode; \
})

static int parse_epf_child_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode , int epf_fun, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;
	int j = 0;

	subNode = curNode->children;//number
	while(NULL != subNode) {
		propNode = subNode->children;//param
		while (NULL != propNode) {
			if(EPF_FUN_SUNLIGHT_PROTECTOR == epf_fun) {
				PARSE_EPF_SP_CONFIG(epsilon0, i, j);
				PARSE_EPF_SP_CONFIG(epsilon1, i, j);
				PARSE_EPF_SP_CONFIG(gain0, i, j);
				PARSE_EPF_SP_CONFIG(gain1, i, j);
				PARSE_EPF_SP_CONFIG(gain2, i, j);
				PARSE_EPF_SP_CONFIG(gain3, i, j);
				PARSE_EPF_SP_CONFIG(gain4, i, j);
				PARSE_EPF_SP_CONFIG(gain5, i, j);
				PARSE_EPF_SP_CONFIG(gain6, i, j);
				PARSE_EPF_SP_CONFIG(gain7, i, j);
				PARSE_EPF_SP_CONFIG(max_diff, i, j);
				PARSE_EPF_SP_CONFIG(min_diff, i, j);
			} else if(EPF_FUN_SUPER_RESOLUTION == epf_fun) {
				PARSE_EPF_SR_CONFIG(epsilon0, i, j);
				PARSE_EPF_SR_CONFIG(epsilon1, i, j);
				PARSE_EPF_SR_CONFIG(gain0, i, j);
				PARSE_EPF_SR_CONFIG(gain1, i, j);
				PARSE_EPF_SR_CONFIG(gain2, i, j);
				PARSE_EPF_SR_CONFIG(gain3, i, j);
				PARSE_EPF_SR_CONFIG(gain4, i, j);
				PARSE_EPF_SR_CONFIG(gain5, i, j);
				PARSE_EPF_SR_CONFIG(gain6, i, j);
				PARSE_EPF_SR_CONFIG(gain7, i, j);
				PARSE_EPF_SR_CONFIG(max_diff, i, j);
				PARSE_EPF_SR_CONFIG(min_diff, i, j);
			}
			if (propNode)
				propNode = propNode->next;
		}
		subNode = subNode->next;
		j++;
	}
	return 0;
}

static int parse_epf_next_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int epf_fun)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	subNode = curNode->children;//config mode
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ENG_LOG("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				parse_epf_child_config(abc, subNode, epf_fun, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				parse_epf_child_config(abc, subNode, epf_fun, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int parse_epf_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	xmlNodePtr tmpNode;
	xmlNodePtr subNode = curNode->children;

	if(tmpNode = FindNode(subNode, "sunlight_protector")) {
		ENG_LOG("curNode name %s \n", tmpNode->name);
		parse_epf_next_config(abc, tmpNode, EPF_FUN_SUNLIGHT_PROTECTOR);
	}
	if(tmpNode = FindNode(subNode, "super_resolution")) {
		ENG_LOG("curNode name %s \n", tmpNode->name);
		parse_epf_next_config(abc, tmpNode, EPF_FUN_SUPER_RESOLUTION);
	}

	return 0;
}

static int parse_slp_cfg_arrays(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;
	int j = 0;

	subNode = curNode;

	while(NULL != subNode) {
		propNode = subNode->children;
		PARSE_SLP_CONFIG(brightness, i, j);
		PARSE_SLP_CONFIG(brightness_step, i, j);
		PARSE_SLP_CONFIG(first_max_bright_th, i, j);
		PARSE_SLP_CONFIG(first_max_bright_th_step0, i, j);
		PARSE_SLP_CONFIG(first_max_bright_th_step1, i, j);
		PARSE_SLP_CONFIG(first_max_bright_th_step2, i, j);
		PARSE_SLP_CONFIG(first_max_bright_th_step3, i, j);
		PARSE_SLP_CONFIG(first_max_bright_th_step4, i, j);
		PARSE_SLP_CONFIG(mask_height, i, j);
		PARSE_SLP_CONFIG(first_percent_th, i, j);
		PARSE_SLP_CONFIG(first_pth_index0, i, j);
		PARSE_SLP_CONFIG(first_pth_index1, i, j);
		PARSE_SLP_CONFIG(first_pth_index2, i, j);
		PARSE_SLP_CONFIG(first_pth_index3, i, j);
		subNode = subNode->next;
		j++;
	}
	
	abc->sceneTable[i].num = j;
	ENG_LOG("l5pro jkjkjkjk %d %s\n", j, __func__);
	return 0;


}

static void parse_slp_baseconfig(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	propNode = curNode->children;//number

	PARSE_SLP_BASECONFIG(hist_exb_no, i);
	PARSE_SLP_BASECONFIG(hist_exb_percent, i);
	PARSE_SLP_BASECONFIG(hist9_index0, i);
	PARSE_SLP_BASECONFIG(hist9_index1, i);
	PARSE_SLP_BASECONFIG(hist9_index2, i);
	PARSE_SLP_BASECONFIG(hist9_index3, i);
	PARSE_SLP_BASECONFIG(hist9_index4, i);
	PARSE_SLP_BASECONFIG(hist9_index5, i);
	PARSE_SLP_BASECONFIG(hist9_index6, i);
	PARSE_SLP_BASECONFIG(hist9_index7, i);
	PARSE_SLP_BASECONFIG(hist9_index8, i);
	PARSE_SLP_BASECONFIG(glb_x1, i);
	PARSE_SLP_BASECONFIG(glb_x2, i);
	PARSE_SLP_BASECONFIG(glb_x3, i);
	PARSE_SLP_BASECONFIG(glb_s1, i);
	PARSE_SLP_BASECONFIG(glb_s2, i);
	PARSE_SLP_BASECONFIG(glb_s3, i);
	PARSE_SLP_BASECONFIG(fast_ambient_th, i);
	PARSE_SLP_BASECONFIG(screen_change_percent_th, i);
	PARSE_SLP_BASECONFIG(local_weight, i);

}

static int parse_slp_child_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	parse_slp_baseconfig(abc, curNode, i);
	subNode = FindNode(curNode->children, "number");
	parse_slp_cfg_arrays(abc, subNode, i);

	return 0;

}

static int parse_slp_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	subNode = curNode->children;
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ENG_LOG("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				parse_slp_child_config(abc, subNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				parse_slp_child_config(abc, subNode, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int parse_ltm_child_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;
	int j = 0;

	subNode = curNode->children;//number
	while(subNode) {
		propNode = subNode->children;//param
		while (NULL != propNode){
			PARSE_LTM_CONFIG(slp_step_clip, i, j);
			PARSE_LTM_CONFIG(slp_high_clip, i, j);
			PARSE_LTM_CONFIG(slp_low_clip, i, j);
			if (propNode)
				propNode = propNode->next;
		}
		subNode = subNode->next;
		j++;
	}

	abc->sceneTable[i].num = j;

	return 0;

}

static int parse_ltm_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	subNode = curNode->children;
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ENG_LOG("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				parse_ltm_child_config(abc, subNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				parse_ltm_child_config(abc, subNode, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int parse_abc_version(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	ENG_LOG("curNode name %s \n",curNode->name);
	subNode = curNode; //enhance version
	while(!xmlStrcmp(subNode->name, BAD_CAST"enhance")) {
		if(xmlHasProp(subNode, BAD_CAST "version")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*) "version");
			abc->version.version = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
		} else if (xmlHasProp(subNode, BAD_CAST "major_version")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*) "major_version");
			abc->nMajorVersion = strtoul((char *)szPropity, NULL, 0);
			xmlFree(szPropity);
		}
		subNode = subNode->next;
	}
	return 0;
}

static int parse_slp_mapping_child_table(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{
	int j = 0;
	int k = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;

	subNode = curNode->children;//number index
	while(NULL != subNode) {
		propNode = subNode->children;//item
		while (NULL != propNode) {
			attrPtr = propNode->properties;
			while (NULL != attrPtr) {
				if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"ambient")) {
					szPropity = xmlGetProp(propNode, (const xmlChar*)"ambient");
					abc->sceneTable[i].sceneTableItem[j].slpMappingTable.item[k].ambient = strtoul((char *)szPropity, NULL, 0);
					xmlFree(szPropity);
				} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"brightness")) {
					szPropity = xmlGetProp(propNode, (const xmlChar*)"brightness");
					abc->sceneTable[i].sceneTableItem[j].slpMappingTable.item[k].slp_brightness_factor = strtoul((char *)szPropity, NULL, 0);
					xmlFree(szPropity);
				}
				attrPtr = attrPtr->next;
			}
			k++;
			propNode = propNode->next;
		}
		abc->sceneTable[i].sceneTableItem[j].slpMappingTable.map_num = k;
		j++;
		k = 0;
		subNode = subNode->next;
	}

	abc->sceneTable[i].num = j;

	return 0;
}

static int parse_slp_mapping_table(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	subNode = curNode->children;// table mode
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ENG_LOG("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				parse_slp_mapping_child_table(abc, subNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				parse_slp_mapping_child_table(abc, subNode, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int update_abc_version(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	int i = 0;
	xmlNodePtr subNode;
	char numStr[12];

	ENG_LOG("curNode name %s \n",curNode->name);
	subNode = curNode; //enhance version
	while(!xmlStrcmp(subNode->name, BAD_CAST"enhance")) {
		if(xmlHasProp(subNode, BAD_CAST "version")) {
			snprintf(numStr, sizeof(numStr), "%d", abc->version.version);
			xmlSetProp(subNode, BAD_CAST "version", (const xmlChar*)numStr);
		}
		subNode = subNode->next;
	}
	return 0;
}

static int update_slp_items_xml(xmlNodePtr curNode, int wrItemnums, int Itemnums)
{

	xmlNodePtr propNode;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ENG_LOG("item wrItemnums =  %d, Itemnums = %d \n", wrItemnums, Itemnums);
	propNode = curNode;
	if (Itemnums < wrItemnums) {
		for(; Itemnums < wrItemnums; Itemnums++) {
			childNode = xmlNewNode(NULL,BAD_CAST "item");
			snprintf(numStr, sizeof(numStr), "%d", 1);
			xmlNewProp(childNode, BAD_CAST "ambient", BAD_CAST numStr);
			xmlNewProp(childNode,BAD_CAST "brightness", BAD_CAST numStr);
			xmlAddSibling(propNode, childNode);
			propNode = childNode;
		}
	} else {
		propNode = propNode->next;
		while (NULL != propNode) {
			NextpropNode = propNode->next;
			if(propNode)
				RemoveNode(curNode, propNode);
			propNode = NextpropNode;
		}
	}

	return 0;
}

static int update_slp_mapping_items_child_xml(xmlNodePtr curNode, int wrItemnums, int Itemnums)
{

	xmlNodePtr subNode;
	xmlNodePtr propNode = NULL;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ENG_LOG("itemsssss wrItemnums =  %d, Itemnums = %d \n", wrItemnums, Itemnums);
	subNode = curNode;
	if (Itemnums < wrItemnums) {
		for(; Itemnums < wrItemnums; Itemnums++) {
			childNode = xmlNewNode(NULL,BAD_CAST "item");
			if(!propNode)
				xmlAddChild(subNode, childNode);
			snprintf(numStr, sizeof(numStr), "%d", 1);
			xmlNewProp(childNode, BAD_CAST "ambient", BAD_CAST numStr);
			xmlNewProp(childNode,BAD_CAST "brightness", BAD_CAST numStr);
			if (propNode)
				xmlAddSibling(propNode, childNode);
			propNode = childNode;
		}
	}


	return 0;
}

static int update_slp_index_xml(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int Indexnums, int i)
{

	xmlNodePtr subNode;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlNodePtr parentNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];
	int wrIndexnums, wrItemnums;

	wrIndexnums = abc->sceneTable[i].num;
	subNode = curNode;
	parentNode = curNode->parent;

	if (Indexnums < wrIndexnums) {
		for(; Indexnums < wrIndexnums; Indexnums++) {
			childNode = xmlNewNode(NULL,BAD_CAST "number");
			xmlAddSibling(subNode, childNode);
			snprintf(numStr, sizeof(numStr), "%d", Indexnums);
			xmlNewProp(childNode, BAD_CAST "index", BAD_CAST numStr);
			wrItemnums = abc->sceneTable[i].sceneTableItem[Indexnums].slpMappingTable.map_num;
			update_slp_mapping_items_child_xml(childNode, wrItemnums, 0);
		}
	} else {
		subNode = subNode->next;
		while (NULL != subNode) {
			NextpropNode = subNode->next;
			if(subNode) {
				RemoveNode(subNode, subNode->children);
				RemoveNode(parentNode, subNode);
			}
			subNode = NextpropNode;
		}
	}

	return 0;
}

static int update_slp_mapping_indexs_arrays(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{

	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	int wrIndexnums, wrItemnums;
	int Indexnums, Itemnums;
	char numStr[10];
	slp_mapping_table_item *item;

	wrIndexnums = abc->sceneTable[i].num;
	subNode = curNode->children;//number index
	Indexnums = 0;
	Itemnums = 0;
	while(NULL != subNode) {
		propNode = subNode->children;//item
		wrItemnums = abc->sceneTable[i].sceneTableItem[Indexnums].slpMappingTable.map_num;
		while(NULL != propNode) {
			Itemnums++;
			if ((propNode->next) && (Itemnums < wrItemnums))
				propNode = propNode->next;
			else {
				update_slp_items_xml(propNode, wrItemnums, Itemnums);
				break;
			}
		}
		Itemnums = 0;
		Indexnums++;
		if((subNode->next) && (Indexnums < wrIndexnums))
			subNode = subNode->next;
		else {
			update_slp_index_xml(abc, subNode, Indexnums, i);
			break;
		}
	}

	return 0;
}

static int update_slp_mapping_child_table(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	int j = 0;
	int k = 0;
	int index_nums, item_nums;
	char numStr[10];

	index_nums = abc->sceneTable[i].num;
	subNode = curNode->children;//number index
	while(NULL != subNode) {
		propNode = subNode->children;//item
		while (NULL != propNode) {
			attrPtr = propNode->properties;
			while (NULL != attrPtr) {
				if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"ambient")) {
					snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[i].sceneTableItem[j].slpMappingTable.item[k].ambient);
					xmlSetProp(propNode, BAD_CAST "ambient", (const xmlChar*)numStr);
				} else if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"brightness")) {
					snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[i].sceneTableItem[j].slpMappingTable.item[k].slp_brightness_factor);
					xmlSetProp(propNode, BAD_CAST "brightness", (const xmlChar*)numStr);
				}
				attrPtr = attrPtr->next;
			}
			k++;
			propNode = propNode->next;
		}
		k = 0;
		j++;
		subNode = subNode->next;
	}

	return 0;
}

static int update_slp_mapping_table(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	subNode = curNode->children;//table normal
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ENG_LOG("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				ENG_LOG("PQ  bl mapping normal \n");
				xmlFree(szPropity);
				update_slp_mapping_indexs_arrays(abc, subNode, 0);
				update_slp_mapping_child_table(abc, subNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				update_slp_mapping_indexs_arrays(abc, subNode, 1);
				update_slp_mapping_child_table(abc, subNode, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int update_epf_config_child_xml(xmlNodePtr curNode)
{

	xmlNodePtr subNode;
	xmlNodePtr propNode = NULL;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	subNode = curNode;
	UPDATE_EPF_CONFIG_CHILD_XML(epsilon0);
	UPDATE_EPF_CONFIG_CHILD_XML(epsilon1);
	UPDATE_EPF_CONFIG_CHILD_XML(gain0);
	UPDATE_EPF_CONFIG_CHILD_XML(gain1);
	UPDATE_EPF_CONFIG_CHILD_XML(gain2);
	UPDATE_EPF_CONFIG_CHILD_XML(gain3);
	UPDATE_EPF_CONFIG_CHILD_XML(gain4);
	UPDATE_EPF_CONFIG_CHILD_XML(gain5);
	UPDATE_EPF_CONFIG_CHILD_XML(gain6);
	UPDATE_EPF_CONFIG_CHILD_XML(gain7);
	UPDATE_EPF_CONFIG_CHILD_XML(max_diff);
	UPDATE_EPF_CONFIG_CHILD_XML(min_diff);

	return 0;
}

static int update_epf_config_index_xml(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int Indexnums, int i)
{

	xmlNodePtr subNode;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlNodePtr parentNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];
	int wrIndexnums, wrItemnums;

	wrIndexnums = abc->sceneTable[i].num;
	subNode = curNode;
	parentNode = curNode->parent;

	if (Indexnums < wrIndexnums) {
		for(; Indexnums < wrIndexnums; Indexnums++) {
			childNode = xmlNewNode(NULL,BAD_CAST "number");
			xmlAddSibling(subNode, childNode);
			snprintf(numStr, sizeof(numStr), "%d", Indexnums);
			xmlNewProp(childNode, BAD_CAST "index", BAD_CAST numStr);
			update_epf_config_child_xml(childNode);
		}
	} else {
		subNode = subNode->next;
		while (NULL != subNode) {
			NextpropNode = subNode->next;
			if(subNode) {
				RemoveNode(subNode, subNode->children);
				RemoveNode(parentNode, subNode);
			}
			subNode = NextpropNode;
		}
	}

	return 0;
}

static int update_epf_config_indexs_arrays(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{

	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	int wrIndexnums, wrItemnums;
	int Indexnums, Itemnums;
	char numStr[10];
	slp_mapping_table_item *item;

	wrIndexnums = abc->sceneTable[i].num;
	subNode = curNode->children;//number index
	Indexnums = 0;
	while(NULL != subNode) {
		Indexnums++;
		if((subNode->next) && (Indexnums < wrIndexnums))
			subNode = subNode->next;
		else {
			update_epf_config_index_xml(abc, subNode, Indexnums, i);
			break;
		}
	}
	return 0;
}

static int update_epf_child_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int epf_fun, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;
	char numStr[10];
	int j = 0;

	subNode = curNode->children;//number index
	while(NULL != subNode) {
		propNode = subNode->children;
		while (NULL != propNode){
			if(EPF_FUN_SUNLIGHT_PROTECTOR == epf_fun) {
				UPDATE_EPF_SP_CONFIG(epsilon0, i, j);
				UPDATE_EPF_SP_CONFIG(epsilon1, i, j);
				UPDATE_EPF_SP_CONFIG(gain0, i, j);
				UPDATE_EPF_SP_CONFIG(gain1, i, j);
				UPDATE_EPF_SP_CONFIG(gain2, i, j);
				UPDATE_EPF_SP_CONFIG(gain3, i, j);
				UPDATE_EPF_SP_CONFIG(gain4, i, j);
				UPDATE_EPF_SP_CONFIG(gain5, i, j);
				UPDATE_EPF_SP_CONFIG(gain6, i, j);
				UPDATE_EPF_SP_CONFIG(gain7, i, j);
				UPDATE_EPF_SP_CONFIG(max_diff, i, j);
				UPDATE_EPF_SP_CONFIG(min_diff, i, j);
			} else if(EPF_FUN_SUPER_RESOLUTION == epf_fun) {
				UPDATE_EPF_SR_CONFIG(epsilon0, i, j);
				UPDATE_EPF_SR_CONFIG(epsilon1, i, j);
				UPDATE_EPF_SR_CONFIG(gain0, i, j);
				UPDATE_EPF_SR_CONFIG(gain1, i, j);
				UPDATE_EPF_SR_CONFIG(gain2, i, j);
				UPDATE_EPF_SR_CONFIG(gain3, i, j);
				UPDATE_EPF_SR_CONFIG(gain4, i, j);
				UPDATE_EPF_SR_CONFIG(gain5, i, j);
				UPDATE_EPF_SR_CONFIG(gain6, i, j);
				UPDATE_EPF_SR_CONFIG(gain7, i, j);
				UPDATE_EPF_SR_CONFIG(max_diff, i, j);
				UPDATE_EPF_SR_CONFIG(min_diff, i, j);
			}
		}
		j++;
		subNode = subNode->next;
	}

	return 0;
}

static int update_epf_next_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int epf_fun)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ENG_LOG("PQ curNode name %s \n",curNode->name);
	subNode = curNode->children;//config mode
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ENG_LOG("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				ENG_LOG("PQ  epf config normal \n");
				xmlFree(szPropity);
				update_epf_config_indexs_arrays(abc, subNode, 0);
				update_epf_child_config(abc, subNode, epf_fun, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				update_epf_config_indexs_arrays(abc, subNode, 1);
				update_epf_child_config(abc, subNode, epf_fun, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int update_epf_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	xmlNodePtr tmpNode;
	xmlNodePtr subNode = curNode->children;

	if(tmpNode = FindNode(subNode, "sunlight_protector")) {
		ENG_LOG("curNode name %s \n", tmpNode->name);
		update_epf_next_config(abc, tmpNode, EPF_FUN_SUNLIGHT_PROTECTOR);
	}
	if(tmpNode = FindNode(subNode, "super_resolution")) {
		ENG_LOG("curNode name %s \n", tmpNode->name);
		update_epf_next_config(abc, tmpNode, EPF_FUN_SUPER_RESOLUTION);
	}

	return 0;
}

static int update_slp_child_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;
	char numStr[10];
	int j = 0;

	subNode = curNode;//number index
	while(NULL != subNode) {
		propNode = subNode->children;
		while (NULL != propNode){
			UPDATE_SLP_CONFIG(brightness, i, j);
			UPDATE_SLP_CONFIG(brightness_step, i, j);
			UPDATE_SLP_CONFIG(first_max_bright_th, i, j);
			UPDATE_SLP_CONFIG(first_max_bright_th_step0, i, j);
			UPDATE_SLP_CONFIG(first_max_bright_th_step1, i, j);
			UPDATE_SLP_CONFIG(first_max_bright_th_step2, i, j);
			UPDATE_SLP_CONFIG(first_max_bright_th_step3, i, j);
			UPDATE_SLP_CONFIG(first_max_bright_th_step4, i, j);
			UPDATE_SLP_CONFIG(mask_height, i, j);
			UPDATE_SLP_CONFIG(first_percent_th, i, j);
			UPDATE_SLP_CONFIG(first_pth_index0, i, j);
			UPDATE_SLP_CONFIG(first_pth_index1, i, j);
			UPDATE_SLP_CONFIG(first_pth_index2, i, j);
			UPDATE_SLP_CONFIG(first_pth_index3, i, j);
		}
		subNode = subNode->next;
		j++;
	}
	return 0;
}

static int update_slp_config_child_xml(xmlNodePtr curNode)
{

	xmlNodePtr subNode;
	xmlNodePtr propNode = NULL;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	subNode = curNode;

	UPDATE_SLP_CONFIG_CHILD_XML(hist_exb_no);
	UPDATE_SLP_CONFIG_CHILD_XML(hist_exb_percent);
	UPDATE_SLP_CONFIG_CHILD_XML(hist9_index0);
	UPDATE_SLP_CONFIG_CHILD_XML(hist9_index1);
	UPDATE_SLP_CONFIG_CHILD_XML(hist9_index2);
	UPDATE_SLP_CONFIG_CHILD_XML(hist9_index3);
	UPDATE_SLP_CONFIG_CHILD_XML(hist9_index4);
	UPDATE_SLP_CONFIG_CHILD_XML(hist9_index5);
	UPDATE_SLP_CONFIG_CHILD_XML(hist9_index6);
	UPDATE_SLP_CONFIG_CHILD_XML(hist9_index7);
	UPDATE_SLP_CONFIG_CHILD_XML(hist9_index8);
	UPDATE_SLP_CONFIG_CHILD_XML(glb_x1);
	UPDATE_SLP_CONFIG_CHILD_XML(glb_x2);
	UPDATE_SLP_CONFIG_CHILD_XML(glb_x3);
	UPDATE_SLP_CONFIG_CHILD_XML(glb_s1);
	UPDATE_SLP_CONFIG_CHILD_XML(glb_s2);
	UPDATE_SLP_CONFIG_CHILD_XML(glb_s3);
	UPDATE_SLP_CONFIG_CHILD_XML(fast_ambient_th);
	UPDATE_SLP_CONFIG_CHILD_XML(screen_change_percent_th);
	UPDATE_SLP_CONFIG_CHILD_XML(local_weight);

	return 0;
}

static int update_slp_config_index_xml(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int Indexnums, int i)
{

	xmlNodePtr subNode;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlNodePtr parentNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];
	int wrIndexnums, wrItemnums;

	wrIndexnums = abc->sceneTable[i].num;
	subNode = curNode;
	parentNode = curNode->parent;

	if (Indexnums < wrIndexnums) {
		for(; Indexnums < wrIndexnums; Indexnums++) {
			childNode = xmlNewNode(NULL,BAD_CAST "number");
			xmlAddSibling(subNode, childNode);
			snprintf(numStr, sizeof(numStr), "%d", Indexnums);
			xmlNewProp(childNode, BAD_CAST "index", BAD_CAST numStr);
			update_slp_config_child_xml(childNode);
		}
	} else {
		subNode = subNode->next;
		while (NULL != subNode) {
			NextpropNode = subNode->next;
			if(subNode) {
				RemoveNode(subNode, subNode->children);
				RemoveNode(parentNode, subNode);
			}
			subNode = NextpropNode;
		}
	}

	return 0;
}

static int update_slp_config_indexs_arrays(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{

	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	int wrIndexnums, wrItemnums;
	int Indexnums, Itemnums;
	char numStr[10];
	slp_mapping_table_item *item;

	wrIndexnums = abc->sceneTable[i].num;
	subNode = curNode;
	Indexnums = 0;
	while(NULL != subNode) {
		Indexnums++;
		if((subNode->next) && (Indexnums < wrIndexnums))
			subNode = subNode->next;
		else {
			update_slp_config_index_xml(abc, subNode, Indexnums, i);
			break;
		}
	}

	return 0;
}

static void update_slp_baseconfig(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	propNode = curNode->children;
	UPDATE_SLP_BASECONFIG(hist_exb_no, i);
	UPDATE_SLP_BASECONFIG(hist_exb_percent, i);
	UPDATE_SLP_BASECONFIG(hist9_index0, i);
	UPDATE_SLP_BASECONFIG(hist9_index1, i);
	UPDATE_SLP_BASECONFIG(hist9_index2, i);
	UPDATE_SLP_BASECONFIG(hist9_index3, i);
	UPDATE_SLP_BASECONFIG(hist9_index4, i);
	UPDATE_SLP_BASECONFIG(hist9_index5, i);
	UPDATE_SLP_BASECONFIG(hist9_index6, i);
	UPDATE_SLP_BASECONFIG(hist9_index7, i);
	UPDATE_SLP_BASECONFIG(hist9_index8, i);
	UPDATE_SLP_BASECONFIG(glb_x1, i);
	UPDATE_SLP_BASECONFIG(glb_x2, i);
	UPDATE_SLP_BASECONFIG(glb_x3, i);
	UPDATE_SLP_BASECONFIG(glb_s1, i);
	UPDATE_SLP_BASECONFIG(glb_s2, i);
	UPDATE_SLP_BASECONFIG(glb_s3, i);
	UPDATE_SLP_BASECONFIG(fast_ambient_th, i);
	UPDATE_SLP_BASECONFIG(screen_change_percent_th, i);
	UPDATE_SLP_BASECONFIG(local_weight, i);

}
static int update_slp_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr tmpNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ENG_LOG("PQ curNode name %s \n",curNode->name);
	subNode = curNode->children;
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ENG_LOG("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				update_slp_baseconfig(abc, subNode, 0);
				tmpNode = FindNode(subNode->children, "number");
				update_slp_config_indexs_arrays(abc, tmpNode, 0);
				update_slp_child_config(abc, tmpNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				update_slp_baseconfig(abc, subNode, 1);
				tmpNode = FindNode(subNode->children, "number");
				update_slp_config_indexs_arrays(abc, tmpNode, 1);
				update_slp_child_config(abc, tmpNode, 1);
			}
		}
		subNode = subNode->next;
	}
	return 0;
}

static int update_ltm_child_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;
	char numStr[10];
	int j = 0;

	subNode = curNode->children;//number index
	while(NULL != subNode) {
		propNode = subNode->children;
		while (NULL != propNode){
			if (xmlHasProp(propNode, BAD_CAST"slp_step_clip")) {
				snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[i].sceneTableItem[j].ltmCfg.slp_step_clip);
				xmlSetProp(propNode, BAD_CAST"slp_step_clip", (const xmlChar*)numStr);
			} else if (xmlHasProp(propNode, BAD_CAST"slp_high_clip")) {
				snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[i].sceneTableItem[j].ltmCfg.slp_high_clip);
				xmlSetProp(propNode, BAD_CAST"slp_high_clip", (const xmlChar*)numStr);
			} else if (xmlHasProp(propNode, BAD_CAST"slp_low_clip")) {
				snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[i].sceneTableItem[j].ltmCfg.slp_low_clip);
				xmlSetProp(propNode, BAD_CAST"slp_low_clip", (const xmlChar*)numStr);
			}
			propNode = propNode->next;
		}
		subNode = subNode->next;
		j++;
	}
	return 0;
}

#if 1

static int update_ltm_config_child_xml(xmlNodePtr curNode)
{

	xmlNodePtr subNode;
	xmlNodePtr propNode = NULL;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	subNode = curNode;

	UPDATE_LTM_CONFIG_CHILD_XML(slp_step_clip);
	UPDATE_LTM_CONFIG_CHILD_XML(slp_high_clip);
	UPDATE_LTM_CONFIG_CHILD_XML(slp_low_clip);

	return 0;
}

static int update_ltm_config_index_xml(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int Indexnums, int i)
{

	xmlNodePtr subNode;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlNodePtr parentNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];
	int wrIndexnums, wrItemnums;

	wrIndexnums = abc->sceneTable[i].num;
	subNode = curNode;
	parentNode = curNode->parent;

	if (Indexnums < wrIndexnums) {
		for(; Indexnums < wrIndexnums; Indexnums++) {
			childNode = xmlNewNode(NULL,BAD_CAST "number");
			xmlAddSibling(subNode, childNode);
			snprintf(numStr, sizeof(numStr), "%d", Indexnums);
			xmlNewProp(childNode, BAD_CAST "index", BAD_CAST numStr);
			update_ltm_config_child_xml(childNode);
		}
	} else {
		subNode = subNode->next;
		while (NULL != subNode) {
			NextpropNode = subNode->next;
			if(subNode) {
				RemoveNode(subNode, subNode->children);
				RemoveNode(parentNode, subNode);
			}
			subNode = NextpropNode;
		}
	}

	return 0;
}

static int update_ltm_config_indexs_arrays(abc_common_sharkl5Pro *abc, xmlNodePtr curNode, int i)
{

	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	int wrIndexnums, wrItemnums;
	int Indexnums, Itemnums;
	char numStr[10];
	slp_mapping_table_item *item;

	wrIndexnums = abc->sceneTable[i].num;
	subNode = curNode->children;//number index
	Indexnums = 0;
	while(NULL != subNode) {
		Indexnums++;
		if((subNode->next) && (Indexnums < wrIndexnums))
			subNode = subNode->next;
		else {
			update_ltm_config_index_xml(abc, subNode, Indexnums, i);
			break;
		}
	}

	return 0;
}
#endif

static int update_ltm_config(abc_common_sharkl5Pro *abc, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ENG_LOG("PQ curNode name %s \n",curNode->name);
	subNode = curNode->children;
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ENG_LOG("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				ENG_LOG("PQ  slp config normal \n");
				xmlFree(szPropity);
				update_ltm_config_indexs_arrays(abc, subNode, 0);
				update_ltm_child_config(abc, subNode, 0);
				ENG_LOG("update_ltm_child_config finish\n");
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				update_ltm_config_indexs_arrays(abc, subNode, 1);
				update_ltm_child_config(abc, subNode, 1);
				ENG_LOG("update_ltm_child_config finish mode 2\n");
			}
		}
		subNode = subNode->next;
	}
	return 0;
}

int AbcParserR4p0::parse_reg(uint08_t *ctx)
{
	int fdslp, fdepf;
	uint32_t cnt;
	uint32_t szslp, szepf;
	uint08_t* data;
	abc_common_sharkl5Pro *abc;

	abc = &((pq_tuning_parm_sharkl5Pro *)ctx)->abc;

	fdslp = open(DpuLtm, O_RDWR);
	fdepf = open(DpuEpf, O_RDWR);

	if(fdslp < 0 || fdepf < 0) {
		if (fdslp >= 0)
			close(fdslp);
		if (fdepf >= 0)
			close(fdepf);
		ENG_LOG("%s: open file failed, err: %s\n", __func__, strerror(errno));
		return errno;
	}
	szslp = sizeof(slp_ltm_params_l5pro);
	memset(&slp_ltm_params_l5pro, 0, szslp);
	szepf = sizeof(abc->sceneTable[0].sceneTableItem[0].epfCfgSunlightProtector);
	cnt = read(fdepf, (uint08_t*)&abc->sceneTable[0].sceneTableItem[0].epfCfgSunlightProtector, szepf);
	if (cnt != szepf)
		ENG_LOG("L5Pro Epf reg read fail regs_size = %d read_cnt = %d \n", szepf, cnt);
	cnt = read(fdslp, (uint08_t*)&slp_ltm_params_l5pro, szslp);
	if (cnt != szslp)
		ENG_LOG("L5Pro slp reg read fail regs_size = %d read_cnt = %d \n", szslp, cnt);
	abc->sceneTable[0].sceneTableItem[0].slpCfg.brightness = slp_ltm_params_l5pro.brightness;
	abc->sceneTable[0].sceneTableItem[0].slpCfg.brightness_step = slp_ltm_params_l5pro.brightness_step;
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th = slp_ltm_params_l5pro.fst_max_bright_th;
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step0 = slp_ltm_params_l5pro.fst_max_bright_th_step[0];
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step1 = slp_ltm_params_l5pro.fst_max_bright_th_step[1];
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step2 = slp_ltm_params_l5pro.fst_max_bright_th_step[2];
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step3 = slp_ltm_params_l5pro.fst_max_bright_th_step[3];
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step4 = slp_ltm_params_l5pro.fst_max_bright_th_step[4];
	abc->sceneTable[0].sceneTableItem[0].slpCfg.mask_height = slp_ltm_params_l5pro.mask_height;
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_percent_th = slp_ltm_params_l5pro.fst_pth;
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_pth_index0 = slp_ltm_params_l5pro.fst_pth_index[0];
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_pth_index1 = slp_ltm_params_l5pro.fst_pth_index[1];
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_pth_index2 = slp_ltm_params_l5pro.fst_pth_index[2];
	abc->sceneTable[0].sceneTableItem[0].slpCfg.first_pth_index3 = slp_ltm_params_l5pro.fst_pth_index[3];	

	abc->sceneTable[0].slpbasecfg.hist_exb_no = slp_ltm_params_l5pro.hist_exb_no;
	abc->sceneTable[0].slpbasecfg.hist_exb_percent = slp_ltm_params_l5pro.hist_exb_percent;

	abc->sceneTable[0].slpbasecfg.hist9_index0 = slp_ltm_params_l5pro.hist9_index[0];
	abc->sceneTable[0].slpbasecfg.hist9_index1 = slp_ltm_params_l5pro.hist9_index[1];
	abc->sceneTable[0].slpbasecfg.hist9_index2 = slp_ltm_params_l5pro.hist9_index[2];
	abc->sceneTable[0].slpbasecfg.hist9_index3 = slp_ltm_params_l5pro.hist9_index[3];
	abc->sceneTable[0].slpbasecfg.hist9_index4 = slp_ltm_params_l5pro.hist9_index[4];
	abc->sceneTable[0].slpbasecfg.hist9_index5 = slp_ltm_params_l5pro.hist9_index[5];
	abc->sceneTable[0].slpbasecfg.hist9_index6 = slp_ltm_params_l5pro.hist9_index[6];
	abc->sceneTable[0].slpbasecfg.hist9_index7 = slp_ltm_params_l5pro.hist9_index[7];
	abc->sceneTable[0].slpbasecfg.hist9_index8 = slp_ltm_params_l5pro.hist9_index[8];

	abc->sceneTable[0].slpbasecfg.glb_x1 = slp_ltm_params_l5pro.glb_x[0];
	abc->sceneTable[0].slpbasecfg.glb_x2 = slp_ltm_params_l5pro.glb_x[1];
	abc->sceneTable[0].slpbasecfg.glb_x3 = slp_ltm_params_l5pro.glb_x[2];

	abc->sceneTable[0].slpbasecfg.glb_s1 = slp_ltm_params_l5pro.glb_s[0];
	abc->sceneTable[0].slpbasecfg.glb_s2 = slp_ltm_params_l5pro.glb_s[1];
	abc->sceneTable[0].slpbasecfg.glb_s3 = slp_ltm_params_l5pro.glb_s[2];


	abc->sceneTable[0].slpbasecfg.fast_ambient_th = slp_ltm_params_l5pro.fast_ambient_th;
	abc->sceneTable[0].slpbasecfg.screen_change_percent_th = slp_ltm_params_l5pro.scene_change_percent_th;
	abc->sceneTable[0].slpbasecfg.local_weight = slp_ltm_params_l5pro.local_weight;

	abc->sceneTable[0].sceneTableItem[0].ltmCfg.slp_low_clip = slp_ltm_params_l5pro.limit_lclip;
	abc->sceneTable[0].sceneTableItem[0].ltmCfg.slp_high_clip = slp_ltm_params_l5pro.limit_hclip;
	abc->sceneTable[0].sceneTableItem[0].ltmCfg.slp_step_clip = slp_ltm_params_l5pro.limit_clip_step;

	close(fdslp);
	close(fdepf);

	return 0;
}

int AbcParserR4p0::update_reg(uint08_t *ctx)
{
	int fdslp;
	int fdepf;
	int cnt;
	char backlight[11] = {0};
	uint32_t szslp, szepf;
	uint08_t* data;
	uint32_t disable;
	abc_common_sharkl5Pro *abc;

	abc = &((pq_tuning_parm_sharkl5Pro *)ctx)->abc;

	if(abc->version.enable) {
		 fdslp = open(DpuSlp, O_RDWR);
		 fdepf = open(DpuEpf, O_RDWR);
		 if(fdslp< 0 || fdepf < 0) {
			if (fdslp >= 0)
				close(fdslp);
			if (fdepf >= 0)
				close(fdepf);
			 ENG_LOG("%s: open file failed, err: %s\n", __func__, strerror(errno));
			 return errno;
		}
		 slp_ltm_params_l5pro.brightness = abc->sceneTable[0].sceneTableItem[0].slpCfg.brightness;
		 slp_ltm_params_l5pro.brightness_step = abc->sceneTable[0].sceneTableItem[0].slpCfg.brightness_step;
		 slp_ltm_params_l5pro.fst_max_bright_th = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th;
		 slp_ltm_params_l5pro.fst_max_bright_th_step[0] = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step0;
		 slp_ltm_params_l5pro.fst_max_bright_th_step[1] = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step1;
		 slp_ltm_params_l5pro.fst_max_bright_th_step[2] = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step2;
		 slp_ltm_params_l5pro.fst_max_bright_th_step[3] = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step3;
		 slp_ltm_params_l5pro.fst_max_bright_th_step[4] = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_max_bright_th_step4;

		 slp_ltm_params_l5pro.mask_height = abc->sceneTable[0].sceneTableItem[0].slpCfg.mask_height;
		 slp_ltm_params_l5pro.fst_pth = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_percent_th;
		 slp_ltm_params_l5pro.fst_pth_index[0] = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_pth_index0;
		 slp_ltm_params_l5pro.fst_pth_index[1] = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_pth_index1;
		 slp_ltm_params_l5pro.fst_pth_index[2] = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_pth_index2;
		 slp_ltm_params_l5pro.fst_pth_index[3] = abc->sceneTable[0].sceneTableItem[0].slpCfg.first_pth_index3;

		 slp_ltm_params_l5pro.hist_exb_no = abc->sceneTable[0].slpbasecfg.hist_exb_no;
		 slp_ltm_params_l5pro.hist_exb_percent = abc->sceneTable[0].slpbasecfg.hist_exb_percent;

		 slp_ltm_params_l5pro.hist9_index[0] = abc->sceneTable[0].slpbasecfg.hist9_index0;
		 slp_ltm_params_l5pro.hist9_index[1] = abc->sceneTable[0].slpbasecfg.hist9_index1;
		 slp_ltm_params_l5pro.hist9_index[2] = abc->sceneTable[0].slpbasecfg.hist9_index2;
		 slp_ltm_params_l5pro.hist9_index[3] = abc->sceneTable[0].slpbasecfg.hist9_index3;
		 slp_ltm_params_l5pro.hist9_index[4] = abc->sceneTable[0].slpbasecfg.hist9_index4;
		 slp_ltm_params_l5pro.hist9_index[5] = abc->sceneTable[0].slpbasecfg.hist9_index5;
		 slp_ltm_params_l5pro.hist9_index[6] = abc->sceneTable[0].slpbasecfg.hist9_index6;
		 slp_ltm_params_l5pro.hist9_index[7] = abc->sceneTable[0].slpbasecfg.hist9_index7;
		 slp_ltm_params_l5pro.hist9_index[8] = abc->sceneTable[0].slpbasecfg.hist9_index8;

		 slp_ltm_params_l5pro.glb_x[0] = abc->sceneTable[0].slpbasecfg.glb_x1;
		 slp_ltm_params_l5pro.glb_x[1] = abc->sceneTable[0].slpbasecfg.glb_x2;
		 slp_ltm_params_l5pro.glb_x[2] = abc->sceneTable[0].slpbasecfg.glb_x3;

		 slp_ltm_params_l5pro.glb_s[0] = abc->sceneTable[0].slpbasecfg.glb_s1;
		 slp_ltm_params_l5pro.glb_s[1] = abc->sceneTable[0].slpbasecfg.glb_s2;
		 slp_ltm_params_l5pro.glb_s[2] = abc->sceneTable[0].slpbasecfg.glb_s3;

		 slp_ltm_params_l5pro.fast_ambient_th = abc->sceneTable[0].slpbasecfg.fast_ambient_th;
		 slp_ltm_params_l5pro.scene_change_percent_th = abc->sceneTable[0].slpbasecfg.screen_change_percent_th;
		 slp_ltm_params_l5pro.local_weight = abc->sceneTable[0].slpbasecfg.local_weight;

		 slp_ltm_params_l5pro.limit_lclip = abc->sceneTable[0].sceneTableItem[0].ltmCfg.slp_low_clip;
		 slp_ltm_params_l5pro.limit_hclip = abc->sceneTable[0].sceneTableItem[0].ltmCfg.slp_high_clip;
		 slp_ltm_params_l5pro.limit_clip_step = abc->sceneTable[0].sceneTableItem[0].ltmCfg.slp_step_clip;

		szslp = sizeof(slp_ltm_params_l5pro) - 2;
		cnt = write(fdslp, (uint08_t *)&slp_ltm_params_l5pro, szslp);
		if (cnt != szslp)
			ENG_LOG("L5Pro write slp fail regs_size%d wr_cnt %d\n", szslp, cnt);
		szepf = sizeof(abc->sceneTable[0].sceneTableItem[0].epfCfgSunlightProtector);
		cnt = write(fdepf, (uint08_t *)&abc->sceneTable[0].sceneTableItem[0].epfCfgSunlightProtector, szepf);
		if (cnt != szepf)
			ENG_LOG("L5Pro write epf fail regs_size%d wr_cnt %d\n", szepf, cnt);
		close(fdslp);
		close(fdepf);
	}
	else {
		fdslp = open(PQDisable, O_WRONLY);
		if(fdslp < 0) {
			ENG_LOG("PQ %s: open fd 1file failed, err: %s\n", __func__, strerror(errno));
			return errno;
		}
		disable = SLP_EN | LTM_EN;
		write(fdslp, &disable, sizeof(disable));
		close(fdslp);
	}

	return 0;
}

int AbcParserR4p0::parse_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	abc_common_sharkl5Pro *abc;

	abc = &((pq_tuning_parm_sharkl5Pro *)ctx)->abc;

	doc = xmlReadFile(abc_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
		ENG_LOG("read %s Document not parsed successfully.\n",__func__);
		return -1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"root")) {
		ENG_LOG("root node != root\n");
		xmlFreeDoc(doc);
		return -1;
	}
	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "epf_config"))
		parse_epf_config(abc, tmpNode);
	if(tmpNode = FindNode(curNode, "slp_config"))
		parse_slp_config(abc, tmpNode);
	if(tmpNode = FindNode(curNode, "ltm_config"))
		parse_ltm_config(abc, tmpNode);
	if(tmpNode = FindNode(curNode, "slp_mapping_table"))
		parse_slp_mapping_table(abc, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		parse_abc_version(abc, tmpNode);

	xmlSaveFormatFileEnc(abc_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);

	return 0;
}

int AbcParserR4p0::update_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	int ret;
	abc_common_sharkl5Pro *abc;

	abc = &((pq_tuning_parm_sharkl5Pro *)ctx)->abc;

	doc = xmlReadFile(abc_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
		ENG_LOG("write %s Document not parsed successfully.\n",__func__);
		return 1;
	}
	curNode = xmlDocGetRootElement(doc);
	if (xmlStrcmp(curNode->name, (const xmlChar*)"root")) {
		ENG_LOG("root node != root\n");
		xmlFreeDoc(doc);
		return 1;
	}
	curNode = curNode->children;

	if(tmpNode = FindNode(curNode, "epf_config"))
		update_epf_config(abc, tmpNode);
	if(tmpNode = FindNode(curNode, "slp_config"))
		update_slp_config(abc, tmpNode);
	if(tmpNode = FindNode(curNode, "ltm_config"))
		update_ltm_config(abc, tmpNode);
	if(tmpNode = FindNode(curNode, "slp_mapping_table"))
		update_slp_mapping_table(abc, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		update_abc_version(abc, tmpNode);

	ret = xmlSaveFormatFileEnc(abc_xml, doc, "UTF-8", 1);
	if (ret == -1)
		ret = xmlSaveFormatFileEnc(abc_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);

	if (ret == -1)
		return 1;

	return 0;
}
