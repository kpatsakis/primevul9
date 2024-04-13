xmlExpNewOr(xmlExpCtxtPtr ctxt, xmlExpNodePtr left, xmlExpNodePtr right) {
    if (ctxt == NULL)
        return(NULL);
    if ((left == NULL) || (right == NULL)) {
        xmlExpFree(ctxt, left);
        xmlExpFree(ctxt, right);
        return(NULL);
    }
    return(xmlExpHashGetEntry(ctxt, XML_EXP_OR, left, right, NULL, 0, 0));
}