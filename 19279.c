 */
static void
xmlXPathCompVariableReference(xmlXPathParserContextPtr ctxt) {
    xmlChar *name;
    xmlChar *prefix;

    SKIP_BLANKS;
    if (CUR != '$') {
	XP_ERROR(XPATH_VARIABLE_REF_ERROR);
    }
    NEXT;
    name = xmlXPathParseQName(ctxt, &prefix);
    if (name == NULL) {
	XP_ERROR(XPATH_VARIABLE_REF_ERROR);
    }
    ctxt->comp->last = -1;
    PUSH_LONG_EXPR(XPATH_OP_VARIABLE, 0, 0, 0,
	           name, prefix);
    SKIP_BLANKS;
    if ((ctxt->context != NULL) && (ctxt->context->flags & XML_XPATH_NOVAR)) {
	XP_ERROR(XPATH_FORBID_VARIABLE_ERROR);