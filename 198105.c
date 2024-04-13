xmlExpNewNode(xmlExpCtxtPtr ctxt, xmlExpNodeType type) {
    xmlExpNodePtr ret;

    if (ctxt->nb_nodes >= MAX_NODES)
        return(NULL);
    ret = (xmlExpNodePtr) xmlMalloc(sizeof(xmlExpNode));
    if (ret == NULL)
        return(NULL);
    memset(ret, 0, sizeof(xmlExpNode));
    ret->type = type;
    ret->next = NULL;
    ctxt->nb_nodes++;
    ctxt->nb_cons++;
    return(ret);
}