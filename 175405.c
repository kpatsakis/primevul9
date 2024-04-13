 */
static int
xmlXPathCompOpEvalLast(xmlXPathParserContextPtr ctxt, xmlXPathStepOpPtr op,
                       xmlNodePtr * last)
{
    int total = 0, cur;
    xmlXPathCompExprPtr comp;
    xmlXPathObjectPtr arg1, arg2;

    CHECK_ERROR0;
    comp = ctxt->comp;
    switch (op->op) {
        case XPATH_OP_END:
            return (0);
        case XPATH_OP_UNION:
            total =
                xmlXPathCompOpEvalLast(ctxt, &comp->steps[op->ch1], last);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL)
                && (ctxt->value->type == XPATH_NODESET)
                && (ctxt->value->nodesetval != NULL)
                && (ctxt->value->nodesetval->nodeNr >= 1)) {
                /*
                 * limit tree traversing to first node in the result
                 */
		if (ctxt->value->nodesetval->nodeNr > 1)
		    xmlXPathNodeSetSort(ctxt->value->nodesetval);
                *last =
                    ctxt->value->nodesetval->nodeTab[ctxt->value->
                                                     nodesetval->nodeNr -
                                                     1];
            }
            cur =
                xmlXPathCompOpEvalLast(ctxt, &comp->steps[op->ch2], last);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL)
                && (ctxt->value->type == XPATH_NODESET)
                && (ctxt->value->nodesetval != NULL)
                && (ctxt->value->nodesetval->nodeNr >= 1)) { /* TODO: NOP ? */
            }

            arg2 = valuePop(ctxt);
            arg1 = valuePop(ctxt);
            if ((arg1 == NULL) || (arg1->type != XPATH_NODESET) ||
                (arg2 == NULL) || (arg2->type != XPATH_NODESET)) {
	        xmlXPathReleaseObject(ctxt->context, arg1);
	        xmlXPathReleaseObject(ctxt->context, arg2);
                XP_ERROR0(XPATH_INVALID_TYPE);
            }

            arg1->nodesetval = xmlXPathNodeSetMerge(arg1->nodesetval,
                                                    arg2->nodesetval);
            valuePush(ctxt, arg1);
	    xmlXPathReleaseObject(ctxt->context, arg2);
            /* optimizer */
	    if (total > cur)
		xmlXPathCompSwap(op);
            return (total + cur);
        case XPATH_OP_ROOT:
            xmlXPathRoot(ctxt);
            return (0);
        case XPATH_OP_NODE:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
	    valuePush(ctxt, xmlXPathCacheNewNodeSet(ctxt->context,
		ctxt->context->node));
            return (total);
        case XPATH_OP_COLLECT:{
                if (op->ch1 == -1)
                    return (0);

                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
		CHECK_ERROR0;

                total += xmlXPathNodeCollectAndTest(ctxt, op, NULL, last, 0);
                return (total);
            }
        case XPATH_OP_VALUE:
            valuePush(ctxt,
                      xmlXPathCacheObjectCopy(ctxt->context,
			(xmlXPathObjectPtr) op->value4));
            return (0);
        case XPATH_OP_SORT:
            if (op->ch1 != -1)
                total +=
                    xmlXPathCompOpEvalLast(ctxt, &comp->steps[op->ch1],
                                           last);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL)
                && (ctxt->value->type == XPATH_NODESET)
                && (ctxt->value->nodesetval != NULL)
		&& (ctxt->value->nodesetval->nodeNr > 1))
                xmlXPathNodeSetSort(ctxt->value->nodesetval);
            return (total);
        default:
            return (xmlXPathCompOpEval(ctxt, op));