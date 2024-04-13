xsltDecimalFormatGetByName(xsltStylesheetPtr style, xmlChar *name)
{
    xsltDecimalFormatPtr result = NULL;

    if (name == NULL)
	return style->decimalFormat;

    while (style != NULL) {
	for (result = style->decimalFormat->next;
	     result != NULL;
	     result = result->next) {
	    if (xmlStrEqual(name, result->name))
		return result;
	}
	style = xsltNextImport(style);
    }
    return result;
}