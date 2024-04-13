xsltRegisterAllElement(xsltTransformContextPtr ctxt)
{
    xsltRegisterExtElement(ctxt, (const xmlChar *) "apply-templates",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltApplyTemplates);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "apply-imports",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltApplyImports);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "call-template",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltCallTemplate);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "element",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltElement);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "attribute",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltAttribute);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "text",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltText);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "processing-instruction",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltProcessingInstruction);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "comment",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltComment);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "copy",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltCopy);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "value-of",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltValueOf);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "number",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltNumber);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "for-each",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltForEach);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "if",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltIf);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "choose",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltChoose);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "sort",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltSort);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "copy-of",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltCopyOf);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "message",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltMessage);

    /*
     * Those don't have callable entry points but are registered anyway
     */
    xsltRegisterExtElement(ctxt, (const xmlChar *) "variable",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltDebug);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "param",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltDebug);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "with-param",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltDebug);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "decimal-format",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltDebug);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "when",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltDebug);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "otherwise",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltDebug);
    xsltRegisterExtElement(ctxt, (const xmlChar *) "fallback",
                           XSLT_NAMESPACE,
			   (xsltTransformFunction) xsltDebug);

}