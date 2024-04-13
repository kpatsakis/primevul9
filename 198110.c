xmlExpGetStart(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp,
               const xmlChar**tokList, int len) {
    if ((ctxt == NULL) || (exp == NULL) || (tokList == NULL) || (len <= 0))
        return(-1);
    return(xmlExpGetStartInt(ctxt, exp, tokList, len, 0));
}