xmlExpNewAtom(xmlExpCtxtPtr ctxt, const xmlChar *name, int len) {
    if ((ctxt == NULL) || (name == NULL))
        return(NULL);
    name = xmlDictLookup(ctxt->dict, name, len);
    if (name == NULL)
        return(NULL);
    return(xmlExpHashGetEntry(ctxt, XML_EXP_ATOM, NULL, NULL, name, 0, 0));
}