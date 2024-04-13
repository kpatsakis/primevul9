#ifdef XP_OPTIMIZED_FILTER_FIRST
static int
xmlXPathCompOpEvalFilterFirst(xmlXPathParserContextPtr ctxt,
			      xmlXPathStepOpPtr op, xmlNodePtr * first)
{
    int total = 0;
    xmlXPathCompExprPtr comp;
    xmlXPathObjectPtr res;
    xmlXPathObjectPtr obj;
    xmlNodeSetPtr oldset;
    xmlNodePtr oldnode;
    xmlDocPtr oldDoc;
    int i;

    CHECK_ERROR0;
    comp = ctxt->comp;
    /*
    * Optimization for ()[last()] selection i.e. the last elem
    */
    if ((op->ch1 != -1) && (op->ch2 != -1) &&
	(comp->steps[op->ch1].op == XPATH_OP_SORT) &&
	(comp->steps[op->ch2].op == XPATH_OP_SORT)) {
	int f = comp->steps[op->ch2].ch1;

	if ((f != -1) &&
	    (comp->steps[f].op == XPATH_OP_FUNCTION) &&
	    (comp->steps[f].value5 == NULL) &&
	    (comp->steps[f].value == 0) &&
	    (comp->steps[f].value4 != NULL) &&
	    (xmlStrEqual
	    (comp->steps[f].value4, BAD_CAST "last"))) {
	    xmlNodePtr last = NULL;

	    total +=
		xmlXPathCompOpEvalLast(ctxt,
		    &comp->steps[op->ch1],
		    &last);
	    CHECK_ERROR0;
	    /*
	    * The nodeset should be in document order,
	    * Keep only the last value
	    */
	    if ((ctxt->value != NULL) &&
		(ctxt->value->type == XPATH_NODESET) &&
		(ctxt->value->nodesetval != NULL) &&
		(ctxt->value->nodesetval->nodeTab != NULL) &&
		(ctxt->value->nodesetval->nodeNr > 1)) {
		ctxt->value->nodesetval->nodeTab[0] =
		    ctxt->value->nodesetval->nodeTab[ctxt->
		    value->
		    nodesetval->
		    nodeNr -
		    1];
		ctxt->value->nodesetval->nodeNr = 1;
		*first = *(ctxt->value->nodesetval->nodeTab);
	    }
	    return (total);
	}
    }

    if (op->ch1 != -1)
	total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
    CHECK_ERROR0;
    if (op->ch2 == -1)
	return (total);
    if (ctxt->value == NULL)
	return (total);

#ifdef LIBXML_XPTR_ENABLED
    oldnode = ctxt->context->node;
    /*
    * Hum are we filtering the result of an XPointer expression
    */
    if (ctxt->value->type == XPATH_LOCATIONSET) {
	xmlXPathObjectPtr tmp = NULL;
	xmlLocationSetPtr newlocset = NULL;
	xmlLocationSetPtr oldlocset;

	/*
	* Extract the old locset, and then evaluate the result of the
	* expression for all the element in the locset. use it to grow
	* up a new locset.
	*/
	CHECK_TYPE0(XPATH_LOCATIONSET);
	obj = valuePop(ctxt);
	oldlocset = obj->user;
	ctxt->context->node = NULL;

	if ((oldlocset == NULL) || (oldlocset->locNr == 0)) {
	    ctxt->context->contextSize = 0;
	    ctxt->context->proximityPosition = 0;
	    if (op->ch2 != -1)
		total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    res = valuePop(ctxt);
	    if (res != NULL) {
		xmlXPathReleaseObject(ctxt->context, res);
	    }
	    valuePush(ctxt, obj);
	    CHECK_ERROR0;
	    return (total);
	}
	newlocset = xmlXPtrLocationSetCreate(NULL);

	for (i = 0; i < oldlocset->locNr; i++) {
	    /*
	    * Run the evaluation with a node list made of a
	    * single item in the nodelocset.
	    */
	    ctxt->context->node = oldlocset->locTab[i]->user;
	    ctxt->context->contextSize = oldlocset->locNr;
	    ctxt->context->proximityPosition = i + 1;
	    if (tmp == NULL) {
		tmp = xmlXPathCacheNewNodeSet(ctxt->context,
		    ctxt->context->node);
	    } else {
		if (xmlXPathNodeSetAddUnique(tmp->nodesetval,
		                             ctxt->context->node) < 0) {
		    ctxt->error = XPATH_MEMORY_ERROR;
		}
	    }
	    valuePush(ctxt, tmp);
	    if (op->ch2 != -1)
		total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    if (ctxt->error != XPATH_EXPRESSION_OK) {
		xmlXPathFreeObject(obj);
		return(0);
	    }
	    /*
	    * The result of the evaluation need to be tested to
	    * decided whether the filter succeeded or not
	    */
	    res = valuePop(ctxt);
	    if (xmlXPathEvaluatePredicateResult(ctxt, res)) {
		xmlXPtrLocationSetAdd(newlocset,
		    xmlXPathCacheObjectCopy(ctxt->context,
			oldlocset->locTab[i]));
	    }
	    /*
	    * Cleanup
	    */
	    if (res != NULL) {
		xmlXPathReleaseObject(ctxt->context, res);
	    }
	    if (ctxt->value == tmp) {
		valuePop(ctxt);
		xmlXPathNodeSetClear(tmp->nodesetval, 1);
		/*
		* REVISIT TODO: Don't create a temporary nodeset
		* for everly iteration.
		*/
		/* OLD: xmlXPathFreeObject(res); */
	    } else
		tmp = NULL;
	    ctxt->context->node = NULL;
	    /*
	    * Only put the first node in the result, then leave.
	    */
	    if (newlocset->locNr > 0) {
		*first = (xmlNodePtr) oldlocset->locTab[i]->user;
		break;
	    }
	}
	if (tmp != NULL) {
	    xmlXPathReleaseObject(ctxt->context, tmp);
	}
	/*
	* The result is used as the new evaluation locset.
	*/
	xmlXPathReleaseObject(ctxt->context, obj);
	ctxt->context->node = NULL;
	ctxt->context->contextSize = -1;
	ctxt->context->proximityPosition = -1;
	valuePush(ctxt, xmlXPtrWrapLocationSet(newlocset));
	ctxt->context->node = oldnode;
	return (total);
    }
#endif /* LIBXML_XPTR_ENABLED */

    /*
    * Extract the old set, and then evaluate the result of the
    * expression for all the element in the set. use it to grow
    * up a new set.
    */
    CHECK_TYPE0(XPATH_NODESET);
    obj = valuePop(ctxt);
    oldset = obj->nodesetval;

    oldnode = ctxt->context->node;
    oldDoc = ctxt->context->doc;
    ctxt->context->node = NULL;

    if ((oldset == NULL) || (oldset->nodeNr == 0)) {
	ctxt->context->contextSize = 0;
	ctxt->context->proximityPosition = 0;
	/* QUESTION TODO: Why was this code commented out?
	    if (op->ch2 != -1)
		total +=
		    xmlXPathCompOpEval(ctxt,
			&comp->steps[op->ch2]);
	    CHECK_ERROR0;
	    res = valuePop(ctxt);
	    if (res != NULL)
		xmlXPathFreeObject(res);
	*/
	valuePush(ctxt, obj);
	ctxt->context->node = oldnode;
	CHECK_ERROR0;
    } else {
	xmlNodeSetPtr newset;
	xmlXPathObjectPtr tmp = NULL;
	/*
	* Initialize the new set.
	* Also set the xpath document in case things like
	* key() evaluation are attempted on the predicate
	*/
	newset = xmlXPathNodeSetCreate(NULL);
        /* XXX what if xmlXPathNodeSetCreate returned NULL? */

	for (i = 0; i < oldset->nodeNr; i++) {
	    /*
	    * Run the evaluation with a node list made of
	    * a single item in the nodeset.
	    */
	    ctxt->context->node = oldset->nodeTab[i];
	    if ((oldset->nodeTab[i]->type != XML_NAMESPACE_DECL) &&
		(oldset->nodeTab[i]->doc != NULL))
		ctxt->context->doc = oldset->nodeTab[i]->doc;
	    if (tmp == NULL) {
		tmp = xmlXPathCacheNewNodeSet(ctxt->context,
		    ctxt->context->node);
	    } else {
		if (xmlXPathNodeSetAddUnique(tmp->nodesetval,
		                             ctxt->context->node) < 0) {
		    ctxt->error = XPATH_MEMORY_ERROR;
		}
	    }
	    valuePush(ctxt, tmp);
	    ctxt->context->contextSize = oldset->nodeNr;
	    ctxt->context->proximityPosition = i + 1;
	    if (op->ch2 != -1)
		total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    if (ctxt->error != XPATH_EXPRESSION_OK) {
		xmlXPathFreeNodeSet(newset);
		xmlXPathFreeObject(obj);
		return(0);
	    }
	    /*
	    * The result of the evaluation needs to be tested to
	    * decide whether the filter succeeded or not
	    */
	    res = valuePop(ctxt);
	    if (xmlXPathEvaluatePredicateResult(ctxt, res)) {
		if (xmlXPathNodeSetAdd(newset, oldset->nodeTab[i]) < 0)
		    ctxt->error = XPATH_MEMORY_ERROR;
	    }
	    /*
	    * Cleanup
	    */
	    if (res != NULL) {
		xmlXPathReleaseObject(ctxt->context, res);
	    }
	    if (ctxt->value == tmp) {
		valuePop(ctxt);
		/*
		* Don't free the temporary nodeset
		* in order to avoid massive recreation inside this
		* loop.
		*/
		xmlXPathNodeSetClear(tmp->nodesetval, 1);
	    } else
		tmp = NULL;
	    ctxt->context->node = NULL;
	    /*
	    * Only put the first node in the result, then leave.
	    */
	    if (newset->nodeNr > 0) {
		*first = *(newset->nodeTab);
		break;
	    }
	}
	if (tmp != NULL) {
	    xmlXPathReleaseObject(ctxt->context, tmp);
	}
	/*
	* The result is used as the new evaluation set.
	*/
	xmlXPathReleaseObject(ctxt->context, obj);
	ctxt->context->node = NULL;
	ctxt->context->contextSize = -1;
	ctxt->context->proximityPosition = -1;
	/* may want to move this past the '}' later */
	ctxt->context->doc = oldDoc;
	valuePush(ctxt, xmlXPathCacheWrapNodeSet(ctxt->context, newset));
    }
    ctxt->context->node = oldnode;