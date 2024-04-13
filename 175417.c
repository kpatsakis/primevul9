 */
void
xmlXPathValueFlipSign(xmlXPathParserContextPtr ctxt) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return;
    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);