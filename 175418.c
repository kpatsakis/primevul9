
static int
xmlXPathCompOpEvalPredicate(xmlXPathParserContextPtr ctxt,
			    xmlXPathStepOpPtr op,
			    xmlNodeSetPtr set,
			    int contextSize,
			    int hasNsNodes)
{
    if (op->ch1 != -1) {
	xmlXPathCompExprPtr comp = ctxt->comp;
	/*
	* Process inner predicates first.
	*/
	if (comp->steps[op->ch1].op != XPATH_OP_PREDICATE) {
	    /*
	    * TODO: raise an internal error.
	    */
	}
	contextSize = xmlXPathCompOpEvalPredicate(ctxt,
	    &comp->steps[op->ch1], set, contextSize, hasNsNodes);
	CHECK_ERROR0;
	if (contextSize <= 0)
	    return(0);
    }
    if (op->ch2 != -1) {
	xmlXPathContextPtr xpctxt = ctxt->context;
	xmlNodePtr contextNode, oldContextNode;
	xmlDocPtr oldContextDoc;
        int oldcs, oldpp;
	int i, res, contextPos = 0, newContextSize;
	xmlXPathStepOpPtr exprOp;
	xmlXPathObjectPtr contextObj = NULL, exprRes = NULL;

#ifdef LIBXML_XPTR_ENABLED
	/*
	* URGENT TODO: Check the following:
	*  We don't expect location sets if evaluating prediates, right?
	*  Only filters should expect location sets, right?
	*/
#endif
	/*
	* SPEC XPath 1.0:
	*  "For each node in the node-set to be filtered, the
	*  PredicateExpr is evaluated with that node as the
	*  context node, with the number of nodes in the
	*  node-set as the context size, and with the proximity
	*  position of the node in the node-set with respect to
	*  the axis as the context position;"
	* @oldset is the node-set" to be filtered.
	*
	* SPEC XPath 1.0:
	*  "only predicates change the context position and
	*  context size (see [2.4 Predicates])."
	* Example:
	*   node-set  context pos
	*    nA         1
	*    nB         2
	*    nC         3
	*   After applying predicate [position() > 1] :
	*   node-set  context pos
	*    nB         1
	*    nC         2
	*/
	oldContextNode = xpctxt->node;
	oldContextDoc = xpctxt->doc;
        oldcs = xpctxt->contextSize;
        oldpp = xpctxt->proximityPosition;
	/*
	* Get the expression of this predicate.
	*/
	exprOp = &ctxt->comp->steps[op->ch2];
	newContextSize = 0;
	for (i = 0; i < set->nodeNr; i++) {
	    if (set->nodeTab[i] == NULL)
		continue;

	    contextNode = set->nodeTab[i];
	    xpctxt->node = contextNode;
	    xpctxt->contextSize = contextSize;
	    xpctxt->proximityPosition = ++contextPos;

	    /*
	    * Also set the xpath document in case things like
	    * key() are evaluated in the predicate.
	    */
	    if ((contextNode->type != XML_NAMESPACE_DECL) &&
		(contextNode->doc != NULL))
		xpctxt->doc = contextNode->doc;
	    /*
	    * Evaluate the predicate expression with 1 context node
	    * at a time; this node is packaged into a node set; this
	    * node set is handed over to the evaluation mechanism.
	    */
	    if (contextObj == NULL)
		contextObj = xmlXPathCacheNewNodeSet(xpctxt, contextNode);
	    else {
		if (xmlXPathNodeSetAddUnique(contextObj->nodesetval,
		    contextNode) < 0) {
		    ctxt->error = XPATH_MEMORY_ERROR;
		    goto evaluation_exit;
		}
	    }

	    valuePush(ctxt, contextObj);

	    res = xmlXPathCompOpEvalToBoolean(ctxt, exprOp, 1);

	    if ((ctxt->error != XPATH_EXPRESSION_OK) || (res == -1)) {
		xmlXPathNodeSetClear(set, hasNsNodes);
		newContextSize = 0;
		goto evaluation_exit;
	    }

	    if (res != 0) {
		newContextSize++;
	    } else {
		/*
		* Remove the entry from the initial node set.
		*/
		set->nodeTab[i] = NULL;
		if (contextNode->type == XML_NAMESPACE_DECL)
		    xmlXPathNodeSetFreeNs((xmlNsPtr) contextNode);
	    }
	    if (ctxt->value == contextObj) {
		/*
		* Don't free the temporary XPath object holding the
		* context node, in order to avoid massive recreation
		* inside this loop.
		*/
		valuePop(ctxt);
		xmlXPathNodeSetClear(contextObj->nodesetval, hasNsNodes);
	    } else {
		/*
		* TODO: The object was lost in the evaluation machinery.
		*  Can this happen? Maybe in internal-error cases.
		*/
		contextObj = NULL;
	    }
	}

	if (contextObj != NULL) {
	    if (ctxt->value == contextObj)
		valuePop(ctxt);
	    xmlXPathReleaseObject(xpctxt, contextObj);
	}
evaluation_exit:
	if (exprRes != NULL)
	    xmlXPathReleaseObject(ctxt->context, exprRes);
	/*
	* Reset/invalidate the context.
	*/
	xpctxt->node = oldContextNode;
	xpctxt->doc = oldContextDoc;
	xpctxt->contextSize = oldcs;
	xpctxt->proximityPosition = oldpp;
	return(newContextSize);
    }