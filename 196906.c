static void deregisterNode(xmlNodePtr node)
{
    assert(node->_private != NULL);
    assert(*(long*)node->_private == (long) 0x81726354);
    free(node->_private);
    nbregister--;
}