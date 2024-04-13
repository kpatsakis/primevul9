xsltCopyTree(xsltTransformContextPtr ctxt, xmlNodePtr node,
	     xmlNodePtr insert, int literal)
{
    return(xsltCopyTreeInternal(ctxt, node, node, insert, literal, 0));

}