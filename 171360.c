xsltParseStylesheetDecimalFormat(xsltStylesheetPtr style, xmlNodePtr cur)
{
    xmlChar *prop;
    xsltDecimalFormatPtr format;
    xsltDecimalFormatPtr iter;
    
    if ((cur == NULL) || (style == NULL))
	return;

    format = style->decimalFormat;
    
    prop = xmlGetNsProp(cur, BAD_CAST("name"), NULL);
    if (prop != NULL) {
	format = xsltDecimalFormatGetByName(style, prop);
	if (format != NULL) {
	    xsltTransformError(NULL, style, cur,
	 "xsltParseStylestyleDecimalFormat: %s already exists\n", prop);
	    if (style != NULL) style->warnings++;
	    return;
	}
	format = xsltNewDecimalFormat(prop);
	if (format == NULL) {
	    xsltTransformError(NULL, style, cur,
     "xsltParseStylestyleDecimalFormat: failed creating new decimal-format\n");
	    if (style != NULL) style->errors++;
	    return;
	}
	/* Append new decimal-format structure */
	for (iter = style->decimalFormat; iter->next; iter = iter->next)
	    ;
	if (iter)
	    iter->next = format;
    }

    prop = xmlGetNsProp(cur, (const xmlChar *)"decimal-separator", NULL);
    if (prop != NULL) {
	if (format->decimalPoint != NULL) xmlFree(format->decimalPoint);
	format->decimalPoint  = prop;
    }
    
    prop = xmlGetNsProp(cur, (const xmlChar *)"grouping-separator", NULL);
    if (prop != NULL) {
	if (format->grouping != NULL) xmlFree(format->grouping);
	format->grouping  = prop;
    }

    prop = xmlGetNsProp(cur, (const xmlChar *)"infinity", NULL);
    if (prop != NULL) {
	if (format->infinity != NULL) xmlFree(format->infinity);
	format->infinity  = prop;
    }
    
    prop = xmlGetNsProp(cur, (const xmlChar *)"minus-sign", NULL);
    if (prop != NULL) {
	if (format->minusSign != NULL) xmlFree(format->minusSign);
	format->minusSign  = prop;
    }
    
    prop = xmlGetNsProp(cur, (const xmlChar *)"NaN", NULL);
    if (prop != NULL) {
	if (format->noNumber != NULL) xmlFree(format->noNumber);
	format->noNumber  = prop;
    }
    
    prop = xmlGetNsProp(cur, (const xmlChar *)"percent", NULL);
    if (prop != NULL) {
	if (format->percent != NULL) xmlFree(format->percent);
	format->percent  = prop;
    }
    
    prop = xmlGetNsProp(cur, (const xmlChar *)"per-mille", NULL);
    if (prop != NULL) {
	if (format->permille != NULL) xmlFree(format->permille);
	format->permille  = prop;
    }
    
    prop = xmlGetNsProp(cur, (const xmlChar *)"zero-digit", NULL);
    if (prop != NULL) {
	if (format->zeroDigit != NULL) xmlFree(format->zeroDigit);
	format->zeroDigit  = prop;
    }
    
    prop = xmlGetNsProp(cur, (const xmlChar *)"digit", NULL);
    if (prop != NULL) {
	if (format->digit != NULL) xmlFree(format->digit);
	format->digit  = prop;
    }
    
    prop = xmlGetNsProp(cur, (const xmlChar *)"pattern-separator", NULL);
    if (prop != NULL) {
	if (format->patternSeparator != NULL) xmlFree(format->patternSeparator);
	format->patternSeparator  = prop;
    }
    if (cur->children != NULL) {
	xsltParseContentError(style, cur->children);
    }
}