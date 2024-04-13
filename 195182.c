xmlXIncludeSetFlags(xmlXIncludeCtxtPtr ctxt, int flags) {
    if (ctxt == NULL)
        return(-1);
    ctxt->parseFlags = flags;
    return(0);
}