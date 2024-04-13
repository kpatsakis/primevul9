xsltFreeCompMatch(xsltCompMatchPtr comp) {
    xsltStepOpPtr op;
    int i;

    if (comp == NULL)
	return;
    if (comp->pattern != NULL)
	xmlFree((xmlChar *)comp->pattern);
    if (comp->nsList != NULL)
	xmlFree(comp->nsList);
    for (i = 0;i < comp->nbStep;i++) {
	op = &comp->steps[i];
	if (op->value != NULL)
	    xmlFree(op->value);
	if (op->value2 != NULL)
	    xmlFree(op->value2);
	if (op->value3 != NULL)
	    xmlFree(op->value3);
	if (op->comp != NULL)
	    xmlXPathFreeCompExpr(op->comp);
    }
    xmlFree(comp->steps);
    memset(comp, -1, sizeof(xsltCompMatch));
    xmlFree(comp);
}