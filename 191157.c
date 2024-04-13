htmlDocDumpMemory(xmlDocPtr cur, xmlChar**mem, int *size) {
	htmlDocDumpMemoryFormat(cur, mem, size, 1);
}