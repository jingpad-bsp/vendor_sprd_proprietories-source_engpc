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


#define UPDATE_SLP_CONFIG(X, I, J) \
	({	\
	 if (xmlHasProp(propNode, BAD_CAST #X)) { \
	 snprintf(numStr, sizeof(numStr), "%d", abc->sceneTable[I].sceneTableItem[J].slpCfg.X); \
	 xmlSetProp(propNode, BAD_CAST #X, (const xmlChar*)numStr); \
	 propNode = propNode->next; \
	 } \
	 })

#define UPDATE_LTM_CONFIG(X, I, J) \
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

static int parse_epf_child_config(abc_common_sharkl5 *abc, xmlNodePtr curNode , int i)
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
			PARSE_EPF_CONFIG(epsilon0, i, j);
			PARSE_EPF_CONFIG(epsilon1, i, j);
			PARSE_EPF_CONFIG(gain0, i, j);
			PARSE_EPF_CONFIG(gain1, i, j);
			PARSE_EPF_CONFIG(gain2, i, j);
			PARSE_EPF_CONFIG(gain3, i, j);
			PARSE_EPF_CONFIG(gain4, i, j);
			PARSE_EPF_CONFIG(gain5, i, j);
			PARSE_EPF_CONFIG(gain6, i, j);
			PARSE_EPF_CONFIG(gain7, i, j);
			PARSE_EPF_CONFIG(max_diff, i, j);
			PARSE_EPF_CONFIG(min_diff, i, j);
			if (propNode)
				propNode = propNode->next;
		}
		subNode = subNode->next;
		j++;
	}
	return 0;
}

static int parse_epf_config(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	subNode = curNode->children;//config mode
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				parse_epf_child_config(abc, subNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				parse_epf_child_config(abc, subNode, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int parse_slp_child_config(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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
			PARSE_SLP_CONFIG(brightness, i, j);
			PARSE_SLP_CONFIG(conversion_matrix, i, j);
			PARSE_SLP_CONFIG(brightness_step, i, j);
			PARSE_SLP_CONFIG(second_bright_factor, i, j);
			PARSE_SLP_CONFIG(first_percent_th, i, j);
			PARSE_SLP_CONFIG(first_max_bright_th, i, j);
			if (propNode)
				propNode = propNode->next;
		}
		subNode = subNode->next;
		j++;
	}
	return 0;

}

static int parse_slp_config(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	subNode = curNode->children;
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
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

static int parse_ltm_child_config(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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
	return 0;

}

static int parse_ltm_config(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	subNode = curNode->children;
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
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

static int parse_abc_version(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	ALOGD("curNode name %s \n",curNode->name);
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

static int parse_slp_mapping_child_table(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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

static int parse_slp_mapping_table(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	xmlNodePtr propNode;
	xmlNodePtr subNode;
	xmlChar* szPropity;
	const char *endptr = NULL;

	subNode = curNode->children;// table mode
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
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

static int parse_bl_mapping_table(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;

	ALOGD("curNode name %s \n",curNode->name);
	subNode = curNode->children; //table mode
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
			propNode = subNode->children;
			while (NULL != propNode) {
				attrPtr = propNode->properties;
				while (NULL != attrPtr) {
					if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"ambient")) {
						szPropity = xmlGetProp(propNode, (const xmlChar*)"ambient");
						abc->blMappingTable[i].item[j].ambient = strtoul((char *)szPropity, NULL, 0);
						xmlFree(szPropity);
					} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"brightness")) {
						szPropity = xmlGetProp(propNode, (const xmlChar*)"brightness");
						abc->blMappingTable[i].item[j].backlight = strtoul((char *)szPropity, NULL, 0);
						xmlFree(szPropity);
					}
					attrPtr = attrPtr->next;
				}
				j++;
				propNode = propNode->next;
			}
			abc->blMappingTable[i].map_num = j;
		}
		i++;
		j = 0;
		subNode = subNode->next;
	}

	return 0;
}

static int update_abc_version(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	int i = 0;
	xmlNodePtr subNode;
	char numStr[12];

	ALOGD("curNode name %s \n",curNode->name);
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

	ALOGD("item wrItemnums =  %d, Itemnums = %d \n", wrItemnums, Itemnums);
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

	ALOGD("itemsssss wrItemnums =  %d, Itemnums = %d \n", wrItemnums, Itemnums);
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

static int update_slp_index_xml(abc_common_sharkl5 *abc, xmlNodePtr curNode, int Indexnums, int i)
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

static int update_slp_mapping_indexs_arrays(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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

static int update_slp_mapping_child_table(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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

static int update_slp_mapping_table(abc_common_sharkl5 *abc, xmlNodePtr curNode)
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
			ALOGD("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				ALOGD("PQ  bl mapping normal \n");
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

static int update_bl_child_table(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
{
	xmlNodePtr propNode;
	xmlNodePtr NextpropNode;
	xmlNodePtr childNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	int j = 0;
	int nums;
	char numStr[10];

	propNode = curNode->children;
	while (NULL != propNode) {
		nums = abc->blMappingTable[i].map_num;
		attrPtr = propNode->properties;
		while (NULL != attrPtr) {
			if (!xmlStrcmp(attrPtr->name, (const xmlChar*)"ambient")) {
				snprintf(numStr, sizeof(numStr), "%d", abc->blMappingTable[i].item[j].ambient);
				xmlSetProp(propNode, BAD_CAST "ambient", (const xmlChar*)numStr);
			} else if(!xmlStrcmp(attrPtr->name, (const xmlChar*)"brightness")) {
				snprintf(numStr, sizeof(numStr), "%d", abc->blMappingTable[i].item[j].backlight);
				xmlSetProp(propNode, BAD_CAST "brightness", (const xmlChar*)numStr);
			}
			attrPtr = attrPtr->next;
		}
		j++;
		if ((propNode->next) && ( j < nums)) {
			ALOGD("PQ nums =  %d j = %d \n", nums, j);
			propNode = propNode->next;
		}
		else
			break;
	}
	if(j < nums) {
		for(; j < nums; j++) {
			childNode = xmlNewNode(NULL, BAD_CAST "item");
			snprintf(numStr, sizeof(numStr), "%d", abc->blMappingTable[i].item[j].ambient);
			xmlNewProp(childNode, BAD_CAST "ambient", BAD_CAST numStr);
			snprintf(numStr, sizeof(numStr), "%d", abc->blMappingTable[i].item[j].backlight);
			xmlNewProp(childNode, BAD_CAST "brightness", BAD_CAST numStr);
			xmlAddSibling(propNode, childNode);
			propNode = childNode;
		}
	} else {
		propNode = propNode->next;
		while (NULL != propNode) {
			NextpropNode = propNode->next;
			if(propNode)
				RemoveNode(curNode, propNode);
			j++;
			propNode = NextpropNode;
			ALOGD("PQ REMOVE  j = %d\n", j);
		}
	}

	return 0;
}

static int update_bl_mapping_table(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ALOGD("PQ curNode name %s abc->mode %d \n", curNode->name, abc->mode);
	subNode = curNode->children; //subNode table
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				update_bl_child_table(abc, subNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				update_bl_child_table(abc, subNode, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int update_epf_child_config(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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
			UPDATE_EPF_CONFIG(epsilon0, i, j);
			UPDATE_EPF_CONFIG(epsilon1, i, j);
			UPDATE_EPF_CONFIG(gain0, i, j);
			UPDATE_EPF_CONFIG(gain1, i, j);
			UPDATE_EPF_CONFIG(gain2, i, j);
			UPDATE_EPF_CONFIG(gain3, i, j);
			UPDATE_EPF_CONFIG(gain4, i, j);
			UPDATE_EPF_CONFIG(gain5, i, j);
			UPDATE_EPF_CONFIG(gain6, i, j);
			UPDATE_EPF_CONFIG(gain7, i, j);
			UPDATE_EPF_CONFIG(max_diff, i, j);
			UPDATE_EPF_CONFIG(min_diff, i, j);
		}
		j++;
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

static int update_epf_config_index_xml(abc_common_sharkl5 *abc, xmlNodePtr curNode, int Indexnums, int i)
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

static int update_epf_config_indexs_arrays(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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

static int update_epf_config(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ALOGD("PQ curNode name %s \n",curNode->name);
	subNode = curNode->children;
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				ALOGD("PQ  epf config normal \n");
				xmlFree(szPropity);
				update_epf_config_indexs_arrays(abc, subNode, 0);
				update_epf_child_config(abc, subNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				update_epf_config_indexs_arrays(abc, subNode, 1);
				update_epf_child_config(abc, subNode, 1);
			}
		}
		subNode = subNode->next;
	}

	return 0;
}

static int update_slp_child_config(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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
			UPDATE_SLP_CONFIG(brightness, i , j);
			UPDATE_SLP_CONFIG(conversion_matrix, i, j);
			UPDATE_SLP_CONFIG(brightness_step, i, j);
			UPDATE_SLP_CONFIG(second_bright_factor, i, j);
			UPDATE_SLP_CONFIG(first_percent_th, i, j);
			UPDATE_SLP_CONFIG(first_max_bright_th, i, j);
		}
		subNode = subNode->next;
		j++;
	}
	return 0;
}

#if 1

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

	UPDATE_SLP_CONFIG_CHILD_XML(brightness);
	UPDATE_SLP_CONFIG_CHILD_XML(conversion_matrix);
	UPDATE_SLP_CONFIG_CHILD_XML(conversion_matrix);
	UPDATE_SLP_CONFIG_CHILD_XML(brightness_step);
	UPDATE_SLP_CONFIG_CHILD_XML(second_bright_factor);
	UPDATE_SLP_CONFIG_CHILD_XML(first_percent_th);
	UPDATE_SLP_CONFIG_CHILD_XML(first_max_bright_th);

	return 0;
}

static int update_slp_config_index_xml(abc_common_sharkl5 *abc, xmlNodePtr curNode, int Indexnums, int i)
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

static int update_slp_config_indexs_arrays(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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
			update_slp_config_index_xml(abc, subNode, Indexnums, i);
			break;
		}
	}

	return 0;
}
#endif

static int update_slp_config(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ALOGD("PQ curNode name %s \n",curNode->name);
	subNode = curNode->children;
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				ALOGD("PQ  slp config normal \n");
				update_slp_config_indexs_arrays(abc, subNode, 0);
				update_slp_child_config(abc, subNode, 0);
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				update_slp_config_indexs_arrays(abc, subNode, 1);
				update_slp_child_config(abc, subNode, 1);
			}
		}
		subNode = subNode->next;
	}
	return 0;
}

static int update_ltm_child_config(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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

static int update_ltm_config_index_xml(abc_common_sharkl5 *abc, xmlNodePtr curNode, int Indexnums, int i)
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

static int update_ltm_config_indexs_arrays(abc_common_sharkl5 *abc, xmlNodePtr curNode, int i)
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

static int update_ltm_config(abc_common_sharkl5 *abc, xmlNodePtr curNode)
{
	int i = 0;
	int j = 0;
	const char *endptr = NULL;
	xmlNodePtr subNode;
	xmlNodePtr propNode;
	xmlAttrPtr attrPtr;
	xmlChar* szPropity;
	char numStr[10];

	ALOGD("PQ curNode name %s \n",curNode->name);
	subNode = curNode->children;
	while(NULL != subNode) {
		if(xmlHasProp(subNode, BAD_CAST "mode")) {
			ALOGD("curNode name %s \n",subNode->name);
			szPropity = xmlGetProp(subNode, (const xmlChar*)"mode");
			if(!xmlStrcmp(szPropity, (const xmlChar *) "normal")) {
				xmlFree(szPropity);
				ALOGD("PQ  slp config normal \n");
				update_ltm_config_indexs_arrays(abc, subNode, 0);
				update_ltm_child_config(abc, subNode, 0);
				ALOGD("update_ltm_child_config finish\n");
			} else if(!xmlStrcmp(szPropity, (const xmlChar *) "low power")) {
				xmlFree(szPropity);
				update_ltm_config_indexs_arrays(abc, subNode, 1);
				update_ltm_child_config(abc, subNode, 1);
				ALOGD("update_ltm_child_config finish mode 2\n");
			}
		}
		subNode = subNode->next;
	}
	return 0;
}

int AbcParserLiteR2p0::parse_reg(uint08_t *ctx)
{
	int fd, fd1;
	unsigned int cnt;
	unsigned int sizes, sizes1;
	unsigned char* data;
	unsigned short tmp,tmp1;
	abc_common_sharkl5 *abc;

	abc = &((pq_tuning_parm_sharkl5 *)ctx)->abc;
	fd = open(DpuSlp, O_RDWR);
	fd1 = open(DpuEpf, O_RDWR);

	if(fd < 0 || fd1 < 0) {
		if (fd >= 0)
			close(fd);
		if (fd1 >= 0)
			close(fd1);
		ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
		return errno;
	}
	sizes = sizeof(slp_ltm_params);
	memset(&slp_ltm_params, 0, sizes);
	sizes1 = sizeof(abc->sceneTable[0].sceneTableItem[0].epfCfg);
	cnt = read(fd1, (unsigned char*)&abc->sceneTable[0].sceneTableItem[0].epfCfg, sizes1);
	cnt = read(fd, (unsigned char*)&slp_ltm_params, sizes);
	tmp = sizeof(abc->sceneTable[0].sceneTableItem[0].slpCfg);
	memcpy((unsigned char*)&abc->sceneTable[0].sceneTableItem[0].slpCfg, (unsigned char*)&slp_ltm_params, tmp);
	tmp1 = sizeof(abc->sceneTable[0].sceneTableItem[0].ltmCfg);
	memcpy((unsigned char*)&abc->sceneTable[0].sceneTableItem[0].ltmCfg, (unsigned char*)&slp_ltm_params + tmp, tmp1);
	close(fd);
	close(fd1);

	return 0;
}

int AbcParserLiteR2p0::update_reg(uint08_t *ctx)
{
	int fd;
	int fd1;
	char backlight[11] = {0};
	unsigned int sizes, sizes1;
	unsigned char* data;
	unsigned int disable;
	unsigned short tmp,tmp1;
	abc_common_sharkl5 *abc;

	abc = &((pq_tuning_parm_sharkl5 *)ctx)->abc;

	if(abc->version.enable) {
		if (abc->slpblMode == 2) {
			fd = open(Brightness, O_RDWR);
			if (fd < 0)
				return errno;
			snprintf(backlight, sizeof(backlight), "%d", 255);
			write(fd, backlight, sizeof(backlight));
			ALOGD("PQ backlights  %s", backlight);
			close(fd);
		}
		 fd = open(DpuSlp, O_RDWR);
		 fd1 = open(DpuEpf, O_RDWR);
		 if(fd < 0 || fd1 < 0) {
			if (fd >= 0)
				close(fd);
			if (fd1 >= 0)
				close(fd1);
			 ALOGD("%s: open file failed, err: %s\n", __func__, strerror(errno));
			 return errno;
		}
		tmp = sizeof(abc->sceneTable[0].sceneTableItem[0].slpCfg);
		memcpy((unsigned char*)&slp_ltm_params, (unsigned char*)&abc->sceneTable[0].sceneTableItem[0].slpCfg, tmp);
		tmp1 = sizeof(abc->sceneTable[0].sceneTableItem[0].ltmCfg);
		memcpy((unsigned char*)&slp_ltm_params + tmp, (unsigned char*)&abc->sceneTable[0].sceneTableItem[0].ltmCfg, tmp1);
		sizes = sizeof(slp_ltm_params) - 2;
		write(fd, (unsigned char *)&slp_ltm_params, sizes);
		sizes1 = sizeof(abc->sceneTable[0].sceneTableItem[0].epfCfg);
		write(fd1, (unsigned char *)&abc->sceneTable[0].sceneTableItem[0].epfCfg, sizes1);
		close(fd);
		close(fd1);
	}
	else {
		fd1 = open(PQDisable, O_WRONLY);
		if(fd1 < 0) {
			ALOGD("PQ %s: open fd 1file failed, err: %s\n", __func__, strerror(errno));
			return errno;
		}
		disable = SLP_EN | LTM_EN | EPF_EN;
		write(fd1, &disable, sizeof(disable));
		close(fd1);
	}

	return 0;
}

int AbcParserLiteR2p0::parse_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	abc_common_sharkl5 *abc;

	ALOGD("%s in\n", __func__);
	abc = &((pq_tuning_parm_sharkl5 *)ctx)->abc;

	doc = xmlReadFile(abc_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
		ALOGD("read %s Document not parsed successfully.\n",__func__);
		return -1;
	}

	curNode = xmlDocGetRootElement(doc);

	if (xmlStrcmp(curNode->name, (const xmlChar*)"root")) {
		ALOGD("root node != root\n");
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
	if(tmpNode = FindNode(curNode, "bl_mapping_table"))
		parse_bl_mapping_table(abc, tmpNode);
	if(tmpNode = FindNode(curNode, "enhance"))
		parse_abc_version(abc, tmpNode);

	xmlSaveFormatFileEnc(abc_xml, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	ALOGD("%s exit\n", __func__);
	return 0;
}

int AbcParserLiteR2p0::update_xml(uint08_t *ctx)
{
	xmlDocPtr doc;
	xmlNodePtr curNode;
	xmlNodePtr tmpNode;
	int ret;
	abc_common_sharkl5 *abc;

	ALOGD("%s in\n", __func__);
	abc = &((pq_tuning_parm_sharkl5 *)ctx)->abc;

	doc = xmlReadFile(abc_xml, "utf-8", XML_PARSE_NOBLANKS);
	if (NULL == doc)
	{
		ALOGD("write %s Document not parsed successfully.\n",__func__);
		return 1;
	}
	curNode = xmlDocGetRootElement(doc);
	if (xmlStrcmp(curNode->name, (const xmlChar*)"root")) {
		ALOGD("root node != root\n");
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
	if(tmpNode = FindNode(curNode, "slp_mapping_table")) {
		if(abc->slpblMode == 1)
			update_slp_mapping_table(abc, tmpNode);
	}
	if(tmpNode = FindNode(curNode, "bl_mapping_table")) {
		if(abc->slpblMode == 2)
			update_bl_mapping_table(abc, tmpNode);
	}
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
