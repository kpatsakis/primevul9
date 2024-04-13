 */
void
xmlXPathModValues(xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr arg;
    double arg1, arg2;

    arg = valuePop(ctxt);
    if (arg == NULL)
	XP_ERROR(XPATH_INVALID_OPERAND);
    arg2 = xmlXPathCastToNumber(arg);
    xmlXPathReleaseObject(ctxt->context, arg);
    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);
    arg1 = ctxt->value->floatval;
    if (arg2 == 0)
	ctxt->value->floatval = NAN;
    else {
	ctxt->value->floatval = fmod(arg1, arg2);