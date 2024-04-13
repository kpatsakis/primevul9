static void registerNode(xmlNodePtr node)
{
    node->_private = malloc(sizeof(long));
    if (node->_private == NULL) {
        fprintf(stderr, "Out of memory in xmllint:registerNode()\n");
	exit(XMLLINT_ERR_MEM);
    }
    *(long*)node->_private = (long) 0x81726354;
    nbregister++;
}