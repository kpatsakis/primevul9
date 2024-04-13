 */
void
xmlXPathDivValues(xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr arg;
    double val;

    arg = valuePop(ctxt);
    if (arg == NULL)
	XP_ERROR(XPATH_INVALID_OPERAND);
    val = xmlXPathCastToNumber(arg);
    xmlXPathReleaseObject(ctxt->context, arg);
    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);