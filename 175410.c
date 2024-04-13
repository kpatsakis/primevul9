
static int
xmlXPathNodeCollectAndTest(xmlXPathParserContextPtr ctxt,
                           xmlXPathStepOpPtr op,
			   xmlNodePtr * first, xmlNodePtr * last,
			   int toBool)
{

#define XP_TEST_HIT \
    if (hasAxisRange != 0) { \
	if (++pos == maxPos) { \
	    if (addNode(seq, cur) < 0) \
	        ctxt->error = XPATH_MEMORY_ERROR; \
	    goto axis_range_end; } \
    } else { \
	if (addNode(seq, cur) < 0) \
	    ctxt->error = XPATH_MEMORY_ERROR; \
	if (breakOnFirstHit) goto first_hit; }

#define XP_TEST_HIT_NS \
    if (hasAxisRange != 0) { \
	if (++pos == maxPos) { \
	    hasNsNodes = 1; \
	    if (xmlXPathNodeSetAddNs(seq, xpctxt->node, (xmlNsPtr) cur) < 0) \
	        ctxt->error = XPATH_MEMORY_ERROR; \
	goto axis_range_end; } \
    } else { \
	hasNsNodes = 1; \
	if (xmlXPathNodeSetAddNs(seq, xpctxt->node, (xmlNsPtr) cur) < 0) \
	    ctxt->error = XPATH_MEMORY_ERROR; \
	if (breakOnFirstHit) goto first_hit; }

    xmlXPathAxisVal axis = (xmlXPathAxisVal) op->value;
    xmlXPathTestVal test = (xmlXPathTestVal) op->value2;
    xmlXPathTypeVal type = (xmlXPathTypeVal) op->value3;
    const xmlChar *prefix = op->value4;
    const xmlChar *name = op->value5;
    const xmlChar *URI = NULL;

#ifdef DEBUG_STEP
    int nbMatches = 0, prevMatches = 0;
#endif
    int total = 0, hasNsNodes = 0;
    /* The popped object holding the context nodes */
    xmlXPathObjectPtr obj;
    /* The set of context nodes for the node tests */
    xmlNodeSetPtr contextSeq;
    int contextIdx;
    xmlNodePtr contextNode;
    /* The final resulting node set wrt to all context nodes */
    xmlNodeSetPtr outSeq;
    /*
    * The temporary resulting node set wrt 1 context node.
    * Used to feed predicate evaluation.
    */
    xmlNodeSetPtr seq;
    xmlNodePtr cur;
    /* First predicate operator */
    xmlXPathStepOpPtr predOp;
    int maxPos; /* The requested position() (when a "[n]" predicate) */
    int hasPredicateRange, hasAxisRange, pos, size, newSize;
    int breakOnFirstHit;

    xmlXPathTraversalFunction next = NULL;
    int (*addNode) (xmlNodeSetPtr, xmlNodePtr);
    xmlXPathNodeSetMergeFunction mergeAndClear;
    xmlNodePtr oldContextNode;
    xmlXPathContextPtr xpctxt = ctxt->context;


    CHECK_TYPE0(XPATH_NODESET);
    obj = valuePop(ctxt);
    /*
    * Setup namespaces.
    */
    if (prefix != NULL) {
        URI = xmlXPathNsLookup(xpctxt, prefix);
        if (URI == NULL) {
	    xmlXPathReleaseObject(xpctxt, obj);
            XP_ERROR0(XPATH_UNDEF_PREFIX_ERROR);
	}
    }
    /*
    * Setup axis.
    *
    * MAYBE FUTURE TODO: merging optimizations:
    * - If the nodes to be traversed wrt to the initial nodes and
    *   the current axis cannot overlap, then we could avoid searching
    *   for duplicates during the merge.
    *   But the question is how/when to evaluate if they cannot overlap.
    *   Example: if we know that for two initial nodes, the one is
    *   not in the ancestor-or-self axis of the other, then we could safely
    *   avoid a duplicate-aware merge, if the axis to be traversed is e.g.
    *   the descendant-or-self axis.
    */
    mergeAndClear = xmlXPathNodeSetMergeAndClear;
    switch (axis) {
        case AXIS_ANCESTOR:
            first = NULL;
            next = xmlXPathNextAncestor;
            break;
        case AXIS_ANCESTOR_OR_SELF:
            first = NULL;
            next = xmlXPathNextAncestorOrSelf;
            break;
        case AXIS_ATTRIBUTE:
            first = NULL;
	    last = NULL;
            next = xmlXPathNextAttribute;
	    mergeAndClear = xmlXPathNodeSetMergeAndClearNoDupls;
            break;
        case AXIS_CHILD:
	    last = NULL;
	    if (((test == NODE_TEST_NAME) || (test == NODE_TEST_ALL)) &&
		(type == NODE_TYPE_NODE))
	    {
		/*
		* Optimization if an element node type is 'element'.
		*/
		next = xmlXPathNextChildElement;
	    } else
		next = xmlXPathNextChild;
	    mergeAndClear = xmlXPathNodeSetMergeAndClearNoDupls;
            break;
        case AXIS_DESCENDANT:
	    last = NULL;
            next = xmlXPathNextDescendant;
            break;
        case AXIS_DESCENDANT_OR_SELF:
	    last = NULL;
            next = xmlXPathNextDescendantOrSelf;
            break;
        case AXIS_FOLLOWING:
	    last = NULL;
            next = xmlXPathNextFollowing;
            break;
        case AXIS_FOLLOWING_SIBLING:
	    last = NULL;
            next = xmlXPathNextFollowingSibling;
            break;
        case AXIS_NAMESPACE:
            first = NULL;
	    last = NULL;
            next = (xmlXPathTraversalFunction) xmlXPathNextNamespace;
	    mergeAndClear = xmlXPathNodeSetMergeAndClearNoDupls;
            break;
        case AXIS_PARENT:
            first = NULL;
            next = xmlXPathNextParent;
            break;
        case AXIS_PRECEDING:
            first = NULL;
            next = xmlXPathNextPrecedingInternal;
            break;
        case AXIS_PRECEDING_SIBLING:
            first = NULL;
            next = xmlXPathNextPrecedingSibling;
            break;
        case AXIS_SELF:
            first = NULL;
	    last = NULL;
            next = xmlXPathNextSelf;
	    mergeAndClear = xmlXPathNodeSetMergeAndClearNoDupls;
            break;
    }

#ifdef DEBUG_STEP
    xmlXPathDebugDumpStepAxis(op,
	(obj->nodesetval != NULL) ? obj->nodesetval->nodeNr : 0);
#endif

    if (next == NULL) {
	xmlXPathReleaseObject(xpctxt, obj);
        return(0);
    }
    contextSeq = obj->nodesetval;
    if ((contextSeq == NULL) || (contextSeq->nodeNr <= 0)) {
	xmlXPathReleaseObject(xpctxt, obj);
        valuePush(ctxt, xmlXPathCacheWrapNodeSet(xpctxt, NULL));
        return(0);
    }
    /*
    * Predicate optimization ---------------------------------------------
    * If this step has a last predicate, which contains a position(),
    * then we'll optimize (although not exactly "position()", but only
    * the  short-hand form, i.e., "[n]".
    *
    * Example - expression "/foo[parent::bar][1]":
    *
    * COLLECT 'child' 'name' 'node' foo    -- op (we are here)
    *   ROOT                               -- op->ch1
    *   PREDICATE                          -- op->ch2 (predOp)
    *     PREDICATE                          -- predOp->ch1 = [parent::bar]
    *       SORT
    *         COLLECT  'parent' 'name' 'node' bar
    *           NODE
    *     ELEM Object is a number : 1        -- predOp->ch2 = [1]
    *
    */
    maxPos = 0;
    predOp = NULL;
    hasPredicateRange = 0;
    hasAxisRange = 0;
    if (op->ch2 != -1) {
	/*
	* There's at least one predicate. 16 == XPATH_OP_PREDICATE
	*/
	predOp = &ctxt->comp->steps[op->ch2];
	if (xmlXPathIsPositionalPredicate(ctxt, predOp, &maxPos)) {
	    if (predOp->ch1 != -1) {
		/*
		* Use the next inner predicate operator.
		*/
		predOp = &ctxt->comp->steps[predOp->ch1];
		hasPredicateRange = 1;
	    } else {
		/*
		* There's no other predicate than the [n] predicate.
		*/
		predOp = NULL;
		hasAxisRange = 1;
	    }
	}
    }
    breakOnFirstHit = ((toBool) && (predOp == NULL)) ? 1 : 0;
    /*
    * Axis traversal -----------------------------------------------------
    */
    /*
     * 2.3 Node Tests
     *  - For the attribute axis, the principal node type is attribute.
     *  - For the namespace axis, the principal node type is namespace.
     *  - For other axes, the principal node type is element.
     *
     * A node test * is true for any node of the
     * principal node type. For example, child::* will
     * select all element children of the context node
     */
    oldContextNode = xpctxt->node;
    addNode = xmlXPathNodeSetAddUnique;
    outSeq = NULL;
    seq = NULL;
    contextNode = NULL;
    contextIdx = 0;


    while (((contextIdx < contextSeq->nodeNr) || (contextNode != NULL)) &&
           (ctxt->error == XPATH_EXPRESSION_OK)) {
	xpctxt->node = contextSeq->nodeTab[contextIdx++];

	if (seq == NULL) {
	    seq = xmlXPathNodeSetCreate(NULL);
	    if (seq == NULL) {
		total = 0;
		goto error;
	    }
	}
	/*
	* Traverse the axis and test the nodes.
	*/
	pos = 0;
	cur = NULL;
	hasNsNodes = 0;
        do {
            cur = next(ctxt, cur);
            if (cur == NULL)
                break;

	    /*
	    * QUESTION TODO: What does the "first" and "last" stuff do?
	    */
            if ((first != NULL) && (*first != NULL)) {
		if (*first == cur)
		    break;
		if (((total % 256) == 0) &&
#ifdef XP_OPTIMIZED_NON_ELEM_COMPARISON
		    (xmlXPathCmpNodesExt(*first, cur) >= 0))
#else
		    (xmlXPathCmpNodes(*first, cur) >= 0))
#endif
		{
		    break;
		}
	    }
	    if ((last != NULL) && (*last != NULL)) {
		if (*last == cur)
		    break;
		if (((total % 256) == 0) &&
#ifdef XP_OPTIMIZED_NON_ELEM_COMPARISON
		    (xmlXPathCmpNodesExt(cur, *last) >= 0))
#else
		    (xmlXPathCmpNodes(cur, *last) >= 0))
#endif
		{
		    break;
		}
	    }

            total++;

#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, " %s", cur->name);
#endif

	    switch (test) {
                case NODE_TEST_NONE:
		    total = 0;
                    STRANGE
		    goto error;
                case NODE_TEST_TYPE:
		    if (type == NODE_TYPE_NODE) {
			switch (cur->type) {
			    case XML_DOCUMENT_NODE:
			    case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
			    case XML_DOCB_DOCUMENT_NODE:
#endif
			    case XML_ELEMENT_NODE:
			    case XML_ATTRIBUTE_NODE:
			    case XML_PI_NODE:
			    case XML_COMMENT_NODE:
			    case XML_CDATA_SECTION_NODE:
			    case XML_TEXT_NODE:
				XP_TEST_HIT
				break;
			    case XML_NAMESPACE_DECL: {
				if (axis == AXIS_NAMESPACE) {
				    XP_TEST_HIT_NS
				} else {
	                            hasNsNodes = 1;
				    XP_TEST_HIT
				}
				break;
                            }
			    default:
				break;
			}
		    } else if (cur->type == (xmlElementType) type) {
			if (cur->type == XML_NAMESPACE_DECL)
			    XP_TEST_HIT_NS
			else
			    XP_TEST_HIT
		    } else if ((type == NODE_TYPE_TEXT) &&
			 (cur->type == XML_CDATA_SECTION_NODE))
		    {
			XP_TEST_HIT
		    }
		    break;
                case NODE_TEST_PI:
                    if ((cur->type == XML_PI_NODE) &&
                        ((name == NULL) || xmlStrEqual(name, cur->name)))
		    {
			XP_TEST_HIT
                    }
                    break;
                case NODE_TEST_ALL:
                    if (axis == AXIS_ATTRIBUTE) {
                        if (cur->type == XML_ATTRIBUTE_NODE)
			{
                            if (prefix == NULL)
			    {
				XP_TEST_HIT
                            } else if ((cur->ns != NULL) &&
				(xmlStrEqual(URI, cur->ns->href)))
			    {
				XP_TEST_HIT
                            }
                        }
                    } else if (axis == AXIS_NAMESPACE) {
                        if (cur->type == XML_NAMESPACE_DECL)
			{
			    XP_TEST_HIT_NS
                        }
                    } else {
                        if (cur->type == XML_ELEMENT_NODE) {
                            if (prefix == NULL)
			    {
				XP_TEST_HIT

                            } else if ((cur->ns != NULL) &&
				(xmlStrEqual(URI, cur->ns->href)))
			    {
				XP_TEST_HIT
                            }
                        }
                    }
                    break;
                case NODE_TEST_NS:{
                        TODO;
                        break;
                    }
                case NODE_TEST_NAME:
                    if (axis == AXIS_ATTRIBUTE) {
                        if (cur->type != XML_ATTRIBUTE_NODE)
			    break;
		    } else if (axis == AXIS_NAMESPACE) {
                        if (cur->type != XML_NAMESPACE_DECL)
			    break;
		    } else {
		        if (cur->type != XML_ELEMENT_NODE)
			    break;
		    }
                    switch (cur->type) {
                        case XML_ELEMENT_NODE:
                            if (xmlStrEqual(name, cur->name)) {
                                if (prefix == NULL) {
                                    if (cur->ns == NULL)
				    {
					XP_TEST_HIT
                                    }
                                } else {
                                    if ((cur->ns != NULL) &&
                                        (xmlStrEqual(URI, cur->ns->href)))
				    {
					XP_TEST_HIT
                                    }
                                }
                            }
                            break;
                        case XML_ATTRIBUTE_NODE:{
                                xmlAttrPtr attr = (xmlAttrPtr) cur;

                                if (xmlStrEqual(name, attr->name)) {
                                    if (prefix == NULL) {
                                        if ((attr->ns == NULL) ||
                                            (attr->ns->prefix == NULL))
					{
					    XP_TEST_HIT
                                        }
                                    } else {
                                        if ((attr->ns != NULL) &&
                                            (xmlStrEqual(URI,
					      attr->ns->href)))
					{
					    XP_TEST_HIT
                                        }
                                    }
                                }
                                break;
                            }
                        case XML_NAMESPACE_DECL:
                            if (cur->type == XML_NAMESPACE_DECL) {
                                xmlNsPtr ns = (xmlNsPtr) cur;

                                if ((ns->prefix != NULL) && (name != NULL)
                                    && (xmlStrEqual(ns->prefix, name)))
				{
				    XP_TEST_HIT_NS
                                }
                            }
                            break;
                        default:
                            break;
                    }
                    break;
	    } /* switch(test) */
        } while ((cur != NULL) && (ctxt->error == XPATH_EXPRESSION_OK));

	goto apply_predicates;

axis_range_end: /* ----------------------------------------------------- */
	/*
	* We have a "/foo[n]", and position() = n was reached.
	* Note that we can have as well "/foo/::parent::foo[1]", so
	* a duplicate-aware merge is still needed.
	* Merge with the result.
	*/
	if (outSeq == NULL) {
	    outSeq = seq;
	    seq = NULL;
	} else
	    outSeq = mergeAndClear(outSeq, seq, 0);
	/*
	* Break if only a true/false result was requested.
	*/
	if (toBool)
	    break;
	continue;

first_hit: /* ---------------------------------------------------------- */
	/*
	* Break if only a true/false result was requested and
	* no predicates existed and a node test succeeded.
	*/
	if (outSeq == NULL) {
	    outSeq = seq;
	    seq = NULL;
	} else
	    outSeq = mergeAndClear(outSeq, seq, 0);
	break;

#ifdef DEBUG_STEP
	if (seq != NULL)
	    nbMatches += seq->nodeNr;
#endif

apply_predicates: /* --------------------------------------------------- */
        if (ctxt->error != XPATH_EXPRESSION_OK)
	    goto error;

        /*
	* Apply predicates.
	*/
        if ((predOp != NULL) && (seq->nodeNr > 0)) {
	    /*
	    * E.g. when we have a "/foo[some expression][n]".
	    */
	    /*
	    * QUESTION TODO: The old predicate evaluation took into
	    *  account location-sets.
	    *  (E.g. ctxt->value->type == XPATH_LOCATIONSET)
	    *  Do we expect such a set here?
	    *  All what I learned now from the evaluation semantics
	    *  does not indicate that a location-set will be processed
	    *  here, so this looks OK.
	    */
	    /*
	    * Iterate over all predicates, starting with the outermost
	    * predicate.
	    * TODO: Problem: we cannot execute the inner predicates first
	    *  since we cannot go back *up* the operator tree!
	    *  Options we have:
	    *  1) Use of recursive functions (like is it currently done
	    *     via xmlXPathCompOpEval())
	    *  2) Add a predicate evaluation information stack to the
	    *     context struct
	    *  3) Change the way the operators are linked; we need a
	    *     "parent" field on xmlXPathStepOp
	    *
	    * For the moment, I'll try to solve this with a recursive
	    * function: xmlXPathCompOpEvalPredicate().
	    */
	    size = seq->nodeNr;
	    if (hasPredicateRange != 0)
		newSize = xmlXPathCompOpEvalPositionalPredicate(ctxt,
		    predOp, seq, size, maxPos, maxPos, hasNsNodes);
	    else
		newSize = xmlXPathCompOpEvalPredicate(ctxt,
		    predOp, seq, size, hasNsNodes);

	    if (ctxt->error != XPATH_EXPRESSION_OK) {
		total = 0;
		goto error;
	    }
	    /*
	    * Add the filtered set of nodes to the result node set.
	    */
	    if (newSize == 0) {
		/*
		* The predicates filtered all nodes out.
		*/
		xmlXPathNodeSetClear(seq, hasNsNodes);
	    } else if (seq->nodeNr > 0) {
		/*
		* Add to result set.
		*/
		if (outSeq == NULL) {
		    if (size != newSize) {
			/*
			* We need to merge and clear here, since
			* the sequence will contained NULLed entries.
			*/
			outSeq = mergeAndClear(NULL, seq, 1);
		    } else {
			outSeq = seq;
			seq = NULL;
		    }
		} else
		    outSeq = mergeAndClear(outSeq, seq,
			(size != newSize) ? 1: 0);
		/*
		* Break if only a true/false result was requested.
		*/
		if (toBool)
		    break;
	    }
        } else if (seq->nodeNr > 0) {
	    /*
	    * Add to result set.
	    */
	    if (outSeq == NULL) {
		outSeq = seq;
		seq = NULL;
	    } else {
		outSeq = mergeAndClear(outSeq, seq, 0);
	    }
	}
    }

error:
    if ((obj->boolval) && (obj->user != NULL)) {
	/*
	* QUESTION TODO: What does this do and why?
	* TODO: Do we have to do this also for the "error"
	* cleanup further down?
	*/
	ctxt->value->boolval = 1;
	ctxt->value->user = obj->user;
	obj->user = NULL;
	obj->boolval = 0;
    }
    xmlXPathReleaseObject(xpctxt, obj);

    /*
    * Ensure we return at least an emtpy set.
    */
    if (outSeq == NULL) {
	if ((seq != NULL) && (seq->nodeNr == 0))
	    outSeq = seq;
	else
	    outSeq = xmlXPathNodeSetCreate(NULL);
        /* XXX what if xmlXPathNodeSetCreate returned NULL here? */
    }
    if ((seq != NULL) && (seq != outSeq)) {
	 xmlXPathFreeNodeSet(seq);
    }
    /*
    * Hand over the result. Better to push the set also in
    * case of errors.
    */
    valuePush(ctxt, xmlXPathCacheWrapNodeSet(xpctxt, outSeq));
    /*
    * Reset the context node.
    */
    xpctxt->node = oldContextNode;
    /*
    * When traversing the namespace axis in "toBool" mode, it's
    * possible that tmpNsList wasn't freed.
    */
    if (xpctxt->tmpNsList != NULL) {
        xmlFree(xpctxt->tmpNsList);
        xpctxt->tmpNsList = NULL;
    }

#ifdef DEBUG_STEP
    xmlGenericError(xmlGenericErrorContext,
	"\nExamined %d nodes, found %d nodes at that step\n",
	total, nbMatches);
#endif
