/*************************************************************************
	> File Name: ParserCore.cpp
	> Author: junxiao.feng
	> Mail: junxiao.feng@unisoc.com 
	> Created Time: Mon 10 Jun 2019 01:53:17 PM CST
 ************************************************************************/

#include "ParserCore.h"



xmlNodePtr FindNode(xmlNodePtr curNode, const char *name)
{
	if (!curNode) {
		ALOGD("FindNode fail");
		return NULL;
	}   
	while(NULL != curNode) {
		if (!xmlStrcmp(curNode->name, (const xmlChar*)name)) {
			return curNode;
		}
		curNode = curNode->next;
	}   

	return NULL;
}

void RemoveNode(xmlNode *ParentNode, xmlNode *ChildNode)
{
	if (ChildNode == NULL) {
		return;
	}   

	xmlNodePtr siblingNode = ChildNode->next;//next index

	while (siblingNode != NULL) {
		if (siblingNode->type == XML_ELEMENT_NODE) {
			break;
		}
		siblingNode = siblingNode->next;
	}   


	xmlNode *childrenNode = ChildNode->children;//item
	if (childrenNode == NULL) {
	}   
	xmlNodePtr nextChildNode = NULL;

	while (childrenNode != NULL) {
		nextChildNode = childrenNode->next;//next item
		xmlUnlinkNode(childrenNode);
		childrenNode = nextChildNode;
	}   
	xmlUnlinkNode(ChildNode);
	xmlFreeNode(ChildNode);
}

