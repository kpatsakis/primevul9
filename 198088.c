xmlExpStringDerive(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp,
                   const xmlChar *str, int len) {
    const xmlChar *input;

    if ((exp == NULL) || (ctxt == NULL) || (str == NULL)) {
        return(NULL);
    }
    /*
     * check the string is in the dictionary, if yes use an interned
     * copy, otherwise we know it's not an acceptable input
     */
    input = xmlDictExists(ctxt->dict, str, len);
    if (input == NULL) {
        return(forbiddenExp);
    }
    return(xmlExpStringDeriveInt(ctxt, exp, input));
}