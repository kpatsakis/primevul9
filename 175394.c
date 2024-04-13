 */
void
xmlXPathRoot(xmlXPathParserContextPtr ctxt) {
    if ((ctxt == NULL) || (ctxt->context == NULL))
	return;
    valuePush(ctxt, xmlXPathCacheNewNodeSet(ctxt->context,