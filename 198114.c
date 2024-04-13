xmlExpNewRange(xmlExpCtxtPtr ctxt, xmlExpNodePtr subset, int min, int max) {
    if (ctxt == NULL)
        return(NULL);
    if ((subset == NULL) || (min < 0) || (max < -1) ||
        ((max >= 0) && (min > max))) {
	xmlExpFree(ctxt, subset);
        return(NULL);
    }
    return(xmlExpHashGetEntry(ctxt, XML_EXP_COUNT, subset,
                              NULL, NULL, min, max));
}