xmlExpGetLanguage(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp,
                  const xmlChar**langList, int len) {
    if ((ctxt == NULL) || (exp == NULL) || (langList == NULL) || (len <= 0))
        return(-1);
    return(xmlExpGetLanguageInt(ctxt, exp, langList, len, 0));
}