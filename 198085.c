xmlExpParse(xmlExpCtxtPtr ctxt, const char *expr) {
    xmlExpNodePtr ret;

    ctxt->expr = expr;
    ctxt->cur = expr;

    ret = xmlExpParseExpr(ctxt);
    SKIP_BLANKS
    if (*ctxt->cur != 0) {
        xmlExpFree(ctxt, ret);
        return(NULL);
    }
    return(ret);
}