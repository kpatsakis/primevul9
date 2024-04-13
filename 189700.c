xsltCompilePattern(const xmlChar *pattern, xmlDocPtr doc,
	           xmlNodePtr node, xsltStylesheetPtr style,
		   xsltTransformContextPtr runtime) {
    return (xsltCompilePatternInternal(pattern, doc, node, style, runtime, 0));
}