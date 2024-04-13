xsltApplySequenceConstructor(xsltTransformContextPtr ctxt,
			     xmlNodePtr contextNode, xmlNodePtr list,
			     xsltTemplatePtr templ)
{
    xmlNodePtr oldInsert, oldInst, oldCurInst, oldContextNode;
    xmlNodePtr cur, insert, copy = NULL;
    int level = 0, oldVarsNr;
    xmlDocPtr oldLocalFragmentTop;

#ifdef XSLT_REFACTORED
    xsltStylePreCompPtr info;
#endif

#ifdef WITH_DEBUGGER
    int addCallResult = 0;
    xmlNodePtr debuggedNode = NULL;
#endif

    if (ctxt == NULL)
	return;

#ifdef WITH_DEBUGGER
    if (ctxt->debugStatus != XSLT_DEBUG_NONE) {
	debuggedNode =
	    xsltDebuggerStartSequenceConstructor(ctxt, contextNode,
		list, templ, &addCallResult);
	if (debuggedNode == NULL)
	    return;
    }
#endif

    if (list == NULL)
        return;
    CHECK_STOPPED;

    /*
    * Check for infinite recursion: stop if the maximum of nested templates
    * is excceeded. Adjust xsltMaxDepth if you need more.
    */
    if (ctxt->depth >= ctxt->maxTemplateDepth) {
        xsltTransformError(ctxt, NULL, list,
	    "xsltApplySequenceConstructor: A potential infinite template "
            "recursion was detected.\n"
	    "You can adjust xsltMaxDepth (--maxdepth) in order to "
	    "raise the maximum number of nested template calls and "
	    "variables/params (currently set to %d).\n",
	    ctxt->maxTemplateDepth);
        xsltDebug(ctxt, contextNode, list, NULL);
	ctxt->state = XSLT_STATE_STOPPED;
        return;
    }
    ctxt->depth++;

    oldLocalFragmentTop = ctxt->localRVT;
    oldInsert = insert = ctxt->insert;
    oldInst = oldCurInst = ctxt->inst;
    oldContextNode = ctxt->node;
    /*
    * Save current number of variables on the stack; new vars are popped when
    * exiting.
    */
    oldVarsNr = ctxt->varsNr;
    /*
    * Process the sequence constructor.
    */
    cur = list;
    while (cur != NULL) {
        if (ctxt->opLimit != 0) {
            if (ctxt->opCount >= ctxt->opLimit) {
		xsltTransformError(ctxt, NULL, cur,
		    "xsltApplySequenceConstructor: "
                    "Operation limit exceeded\n");
	        ctxt->state = XSLT_STATE_STOPPED;
                goto error;
            }
            ctxt->opCount += 1;
        }

        ctxt->inst = cur;

#ifdef WITH_DEBUGGER
        switch (ctxt->debugStatus) {
            case XSLT_DEBUG_RUN_RESTART:
            case XSLT_DEBUG_QUIT:
                break;

        }
#endif
        /*
         * Test; we must have a valid insertion point.
         */
        if (insert == NULL) {

#ifdef WITH_XSLT_DEBUG_PROCESS
            XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
		"xsltApplySequenceConstructor: insert == NULL !\n"));
#endif
            goto error;
        }

#ifdef WITH_DEBUGGER
        if ((ctxt->debugStatus != XSLT_DEBUG_NONE) && (debuggedNode != cur))
            xslHandleDebugger(cur, contextNode, templ, ctxt);
#endif

#ifdef XSLT_REFACTORED
	if (cur->type == XML_ELEMENT_NODE) {
	    info = (xsltStylePreCompPtr) cur->psvi;
	    /*
	    * We expect a compiled representation on:
	    * 1) XSLT instructions of this XSLT version (1.0)
	    *    (with a few exceptions)
	    * 2) Literal result elements
	    * 3) Extension instructions
	    * 4) XSLT instructions of future XSLT versions
	    *    (forwards-compatible mode).
	    */
	    if (info == NULL) {
		/*
		* Handle the rare cases where we don't expect a compiled
		* representation on an XSLT element.
		*/
		if (IS_XSLT_ELEM_FAST(cur) && IS_XSLT_NAME(cur, "message")) {
		    xsltMessage(ctxt, contextNode, cur);
		    goto skip_children;
		}
		/*
		* Something really went wrong:
		*/
		xsltTransformError(ctxt, NULL, cur,
		    "Internal error in xsltApplySequenceConstructor(): "
		    "The element '%s' in the stylesheet has no compiled "
		    "representation.\n",
		    cur->name);
                goto skip_children;
            }

	    if (info->type == XSLT_FUNC_LITERAL_RESULT_ELEMENT) {
		xsltStyleItemLRElementInfoPtr lrInfo =
		    (xsltStyleItemLRElementInfoPtr) info;
		/*
		* Literal result elements
		* --------------------------------------------------------
		*/
#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt, XSLT_TRACE_APPLY_TEMPLATE,
		    xsltGenericDebug(xsltGenericDebugContext,
		    "xsltApplySequenceConstructor: copy literal result "
		    "element '%s'\n", cur->name));
#endif
		/*
		* Copy the raw element-node.
		* OLD: if ((copy = xsltShallowCopyElem(ctxt, cur, insert))
		*     == NULL)
		*   goto error;
		*/
		copy = xmlDocCopyNode(cur, insert->doc, 0);
		if (copy == NULL) {
		    xsltTransformError(ctxt, NULL, cur,
			"Internal error in xsltApplySequenceConstructor(): "
			"Failed to copy literal result element '%s'.\n",
			cur->name);
		    goto error;
		} else {
		    /*
		    * Add the element-node to the result tree.
		    */
		    copy->doc = ctxt->output;
		    copy = xsltAddChild(insert, copy);
		    /*
		    * Create effective namespaces declarations.
		    * OLD: xsltCopyNamespaceList(ctxt, copy, cur->nsDef);
		    */
		    if (lrInfo->effectiveNs != NULL) {
			xsltEffectiveNsPtr effNs = lrInfo->effectiveNs;
			xmlNsPtr ns, lastns = NULL;

			while (effNs != NULL) {
			    /*
			    * Avoid generating redundant namespace
			    * declarations; thus lookup if there is already
			    * such a ns-decl in the result.
			    */
			    ns = xmlSearchNs(copy->doc, copy, effNs->prefix);
			    if ((ns != NULL) &&
				(xmlStrEqual(ns->href, effNs->nsName)))
			    {
				effNs = effNs->next;
				continue;
			    }
			    ns = xmlNewNs(copy, effNs->nsName, effNs->prefix);
			    if (ns == NULL) {
				xsltTransformError(ctxt, NULL, cur,
				    "Internal error in "
				    "xsltApplySequenceConstructor(): "
				    "Failed to copy a namespace "
				    "declaration.\n");
				goto error;
			    }

			    if (lastns == NULL)
				copy->nsDef = ns;
			    else
				lastns->next =ns;
			    lastns = ns;

			    effNs = effNs->next;
			}

		    }
		    /*
		    * NOTE that we don't need to apply ns-alising: this was
		    *  already done at compile-time.
		    */
		    if (cur->ns != NULL) {
			/*
			* If there's no such ns-decl in the result tree,
			* then xsltGetSpecialNamespace() will
			* create a ns-decl on the copied node.
			*/
			copy->ns = xsltGetSpecialNamespace(ctxt, cur,
			    cur->ns->href, cur->ns->prefix, copy);
		    } else {
			/*
			* Undeclare the default namespace if needed.
			* This can be skipped, if the result element has
			*  no ns-decls, in which case the result element
			*  obviously does not declare a default namespace;
			*  AND there's either no parent, or the parent
			*  element is in no namespace; this means there's no
			*  default namespace is scope to care about.
			*
			* REVISIT: This might result in massive
			*  generation of ns-decls if nodes in a default
			*  namespaces are mixed with nodes in no namespace.
			*
			*/
			if (copy->nsDef ||
			    ((insert != NULL) &&
			     (insert->type == XML_ELEMENT_NODE) &&
			     (insert->ns != NULL)))
			{
			    xsltGetSpecialNamespace(ctxt, cur,
				NULL, NULL, copy);
			}
		    }
		}
		/*
		* SPEC XSLT 2.0 "Each attribute of the literal result
		*  element, other than an attribute in the XSLT namespace,
		*  is processed to produce an attribute for the element in
		*  the result tree."
		* NOTE: See bug #341325.
		*/
		if (cur->properties != NULL) {
		    xsltAttrListTemplateProcess(ctxt, copy, cur->properties);
		}
	    } else if (IS_XSLT_ELEM_FAST(cur)) {
		/*
		* XSLT instructions
		* --------------------------------------------------------
		*/
		if (info->type == XSLT_FUNC_UNKOWN_FORWARDS_COMPAT) {
		    /*
		    * We hit an unknown XSLT element.
		    * Try to apply one of the fallback cases.
		    */
		    ctxt->insert = insert;
		    if (!xsltApplyFallbacks(ctxt, contextNode, cur)) {
			xsltTransformError(ctxt, NULL, cur,
			    "The is no fallback behaviour defined for "
			    "the unknown XSLT element '%s'.\n",
			    cur->name);
		    }
		    ctxt->insert = oldInsert;
		} else if (info->func != NULL) {
		    /*
		    * Execute the XSLT instruction.
		    */
		    ctxt->insert = insert;

		    info->func(ctxt, contextNode, cur,
			(xsltElemPreCompPtr) info);

		    /*
		    * Cleanup temporary tree fragments.
		    */
		    if (oldLocalFragmentTop != ctxt->localRVT)
			xsltReleaseLocalRVTs(ctxt, oldLocalFragmentTop);

		    ctxt->insert = oldInsert;
		} else if (info->type == XSLT_FUNC_VARIABLE) {
		    xsltStackElemPtr tmpvar = ctxt->vars;

		    xsltParseStylesheetVariable(ctxt, cur);

		    if (tmpvar != ctxt->vars) {
			/*
			* TODO: Using a @tmpvar is an annoying workaround, but
			*  the current mechanisms do not provide any other way
			*  of knowing if the var was really pushed onto the
			*  stack.
			*/
			ctxt->vars->level = level;
		    }
		} else if (info->type == XSLT_FUNC_MESSAGE) {
		    /*
		    * TODO: Won't be hit, since we don't compile xsl:message.
		    */
		    xsltMessage(ctxt, contextNode, cur);
		} else {
		    xsltTransformError(ctxt, NULL, cur,
			"Unexpected XSLT element '%s'.\n", cur->name);
		}
		goto skip_children;

	    } else {
		xsltTransformFunction func;
		/*
		* Extension intructions (elements)
		* --------------------------------------------------------
		*/
		if (cur->psvi == xsltExtMarker) {
		    /*
		    * The xsltExtMarker was set during the compilation
		    * of extension instructions if there was no registered
		    * handler for this specific extension function at
		    * compile-time.
		    * Libxslt will now lookup if a handler is
		    * registered in the context of this transformation.
		    */
		    func = xsltExtElementLookup(ctxt, cur->name,
                                                cur->ns->href);
		} else
		    func = ((xsltElemPreCompPtr) cur->psvi)->func;

		if (func == NULL) {
		    /*
		    * No handler available.
		    * Try to execute fallback behaviour via xsl:fallback.
		    */
#ifdef WITH_XSLT_DEBUG_PROCESS
		    XSLT_TRACE(ctxt, XSLT_TRACE_APPLY_TEMPLATE,
			xsltGenericDebug(xsltGenericDebugContext,
			    "xsltApplySequenceConstructor: unknown extension %s\n",
			    cur->name));
#endif
		    ctxt->insert = insert;
		    if (!xsltApplyFallbacks(ctxt, contextNode, cur)) {
			xsltTransformError(ctxt, NULL, cur,
			    "Unknown extension instruction '{%s}%s'.\n",
			    cur->ns->href, cur->name);
		    }
		    ctxt->insert = oldInsert;
		} else {
		    /*
		    * Execute the handler-callback.
		    */
#ifdef WITH_XSLT_DEBUG_PROCESS
		    XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
			"xsltApplySequenceConstructor: extension construct %s\n",
			cur->name));
#endif
                    /*
                     * Disable the xsltCopyTextString optimization for
                     * extension elements. Extensions could append text using
                     * xmlAddChild which will free the buffer pointed to by
                     * 'lasttext'. This buffer could later be reallocated with
                     * a different size than recorded in 'lasttsize'. See bug
                     * #777432.
                     */
                    if (cur->psvi == xsltExtMarker) {
                        ctxt->lasttext = NULL;
                    }

		    ctxt->insert = insert;

		    func(ctxt, contextNode, cur, cur->psvi);

		    /*
		    * Cleanup temporary tree fragments.
		    */
		    if (oldLocalFragmentTop != ctxt->localRVT)
			xsltReleaseLocalRVTs(ctxt, oldLocalFragmentTop);

		    ctxt->insert = oldInsert;
		}
		goto skip_children;
	    }

	} else if (XSLT_IS_TEXT_NODE(cur)) {
	    /*
	    * Text
	    * ------------------------------------------------------------
	    */
#ifdef WITH_XSLT_DEBUG_PROCESS
            if (cur->name == xmlStringTextNoenc) {
                XSLT_TRACE(ctxt, XSLT_TRACE_APPLY_TEMPLATE,
		    xsltGenericDebug(xsltGenericDebugContext,
		    "xsltApplySequenceConstructor: copy unescaped text '%s'\n",
		    cur->content));
            } else {
                XSLT_TRACE(ctxt, XSLT_TRACE_APPLY_TEMPLATE,
		    xsltGenericDebug(xsltGenericDebugContext,
		    "xsltApplySequenceConstructor: copy text '%s'\n",
		    cur->content));
            }
#endif
            if (xsltCopyText(ctxt, insert, cur, ctxt->internalized) == NULL)
		goto error;
	}

#else /* XSLT_REFACTORED */

        if (IS_XSLT_ELEM(cur)) {
            /*
             * This is an XSLT node
             */
            xsltStylePreCompPtr info = (xsltStylePreCompPtr) cur->psvi;

            if (info == NULL) {
                if (IS_XSLT_NAME(cur, "message")) {
                    xsltMessage(ctxt, contextNode, cur);
                } else {
                    /*
                     * That's an error try to apply one of the fallback cases
                     */
                    ctxt->insert = insert;
                    if (!xsltApplyFallbacks(ctxt, contextNode, cur)) {
                        xsltGenericError(xsltGenericErrorContext,
			    "xsltApplySequenceConstructor: %s was not compiled\n",
			    cur->name);
                    }
                    ctxt->insert = oldInsert;
                }
                goto skip_children;
            }

            if (info->func != NULL) {
		oldCurInst = ctxt->inst;
		ctxt->inst = cur;
                ctxt->insert = insert;

                info->func(ctxt, contextNode, cur, (xsltElemPreCompPtr) info);

		/*
		* Cleanup temporary tree fragments.
		*/
		if (oldLocalFragmentTop != ctxt->localRVT)
		    xsltReleaseLocalRVTs(ctxt, oldLocalFragmentTop);

                ctxt->insert = oldInsert;
		ctxt->inst = oldCurInst;
                goto skip_children;
            }

            if (IS_XSLT_NAME(cur, "variable")) {
		xsltStackElemPtr tmpvar = ctxt->vars;

		oldCurInst = ctxt->inst;
		ctxt->inst = cur;

		xsltParseStylesheetVariable(ctxt, cur);

		ctxt->inst = oldCurInst;

		if (tmpvar != ctxt->vars) {
		    /*
		    * TODO: Using a @tmpvar is an annoying workaround, but
		    *  the current mechanisms do not provide any other way
		    *  of knowing if the var was really pushed onto the
		    *  stack.
		    */
		    ctxt->vars->level = level;
		}
            } else if (IS_XSLT_NAME(cur, "message")) {
                xsltMessage(ctxt, contextNode, cur);
            } else {
		xsltTransformError(ctxt, NULL, cur,
		    "Unexpected XSLT element '%s'.\n", cur->name);
            }
            goto skip_children;
        } else if ((cur->type == XML_TEXT_NODE) ||
                   (cur->type == XML_CDATA_SECTION_NODE)) {

            /*
             * This text comes from the stylesheet
             * For stylesheets, the set of whitespace-preserving
             * element names consists of just xsl:text.
             */
#ifdef WITH_XSLT_DEBUG_PROCESS
            if (cur->type == XML_CDATA_SECTION_NODE) {
                XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
                                 "xsltApplySequenceConstructor: copy CDATA text %s\n",
                                 cur->content));
            } else if (cur->name == xmlStringTextNoenc) {
                XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
                                 "xsltApplySequenceConstructor: copy unescaped text %s\n",
                                 cur->content));
            } else {
                XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
                                 "xsltApplySequenceConstructor: copy text %s\n",
                                 cur->content));
            }
#endif
            if (xsltCopyText(ctxt, insert, cur, ctxt->internalized) == NULL)
		goto error;
        } else if ((cur->type == XML_ELEMENT_NODE) &&
                   (cur->ns != NULL) && (cur->psvi != NULL)) {
            xsltTransformFunction function;

	    oldCurInst = ctxt->inst;
	    ctxt->inst = cur;
            /*
             * Flagged as an extension element
             */
            if (cur->psvi == xsltExtMarker)
                function = xsltExtElementLookup(ctxt, cur->name,
                                                cur->ns->href);
            else
                function = ((xsltElemPreCompPtr) cur->psvi)->func;

            if (function == NULL) {
                xmlNodePtr child;
                int found = 0;

#ifdef WITH_XSLT_DEBUG_PROCESS
                XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
		    "xsltApplySequenceConstructor: unknown extension %s\n",
                    cur->name));
#endif
                /*
                 * Search if there are fallbacks
                 */
                ctxt->insert = insert;
                child = cur->children;
                while (child != NULL) {
                    if ((IS_XSLT_ELEM(child)) &&
                        (IS_XSLT_NAME(child, "fallback")))
		    {
                        found = 1;
                        xsltApplySequenceConstructor(ctxt, contextNode,
			    child->children, NULL);
                    }
                    child = child->next;
                }
                ctxt->insert = oldInsert;

                if (!found) {
                    xsltTransformError(ctxt, NULL, cur,
			"xsltApplySequenceConstructor: failed to find extension %s\n",
			cur->name);
                }
            } else {
#ifdef WITH_XSLT_DEBUG_PROCESS
                XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
		    "xsltApplySequenceConstructor: extension construct %s\n",
                    cur->name));
#endif

                /*
                 * Disable the xsltCopyTextString optimization for
                 * extension elements. Extensions could append text using
                 * xmlAddChild which will free the buffer pointed to by
                 * 'lasttext'. This buffer could later be reallocated with
                 * a different size than recorded in 'lasttsize'. See bug
                 * #777432.
                 */
                if (cur->psvi == xsltExtMarker) {
	            ctxt->lasttext = NULL;
                }

                ctxt->insert = insert;

                function(ctxt, contextNode, cur, cur->psvi);
		/*
		* Cleanup temporary tree fragments.
		*/
		if (oldLocalFragmentTop != ctxt->localRVT)
		    xsltReleaseLocalRVTs(ctxt, oldLocalFragmentTop);

                ctxt->insert = oldInsert;

            }
	    ctxt->inst = oldCurInst;
            goto skip_children;
        } else if (cur->type == XML_ELEMENT_NODE) {
#ifdef WITH_XSLT_DEBUG_PROCESS
            XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
		"xsltApplySequenceConstructor: copy node %s\n",
                cur->name));
#endif
	    oldCurInst = ctxt->inst;
	    ctxt->inst = cur;

            if ((copy = xsltShallowCopyElem(ctxt, cur, insert, 1)) == NULL)
		goto error;
            /*
             * Add extra namespaces inherited from the current template
             * if we are in the first level children and this is a
	     * "real" template.
             */
            if ((templ != NULL) && (oldInsert == insert) &&
                (ctxt->templ != NULL) && (ctxt->templ->inheritedNs != NULL)) {
                int i;
                xmlNsPtr ns, ret;

                for (i = 0; i < ctxt->templ->inheritedNsNr; i++) {
		    const xmlChar *URI = NULL;
		    xsltStylesheetPtr style;
                    ns = ctxt->templ->inheritedNs[i];

		    /* Note that the XSLT namespace was already excluded
		    * in xsltGetInheritedNsList().
		    */
#if 0
		    if (xmlStrEqual(ns->href, XSLT_NAMESPACE))
			continue;
#endif
		    style = ctxt->style;
		    while (style != NULL) {
			if (style->nsAliases != NULL)
			    URI = (const xmlChar *)
				xmlHashLookup(style->nsAliases, ns->href);
			if (URI != NULL)
			    break;

			style = xsltNextImport(style);
		    }
		    if (URI == UNDEFINED_DEFAULT_NS)
			continue;
		    if (URI == NULL)
			URI = ns->href;
		    /*
		    * TODO: The following will still be buggy for the
		    * non-refactored code.
		    */
		    ret = xmlSearchNs(copy->doc, copy, ns->prefix);
		    if ((ret == NULL) || (!xmlStrEqual(ret->href, URI)))
		    {
			xmlNewNs(copy, URI, ns->prefix);
		    }
                }
		if (copy->ns != NULL) {
		    /*
		     * Fix the node namespace if needed
		     */
		    copy->ns = xsltGetNamespace(ctxt, cur, copy->ns, copy);
		}
            }
	    /*
             * all the attributes are directly inherited
             */
            if (cur->properties != NULL) {
                xsltAttrListTemplateProcess(ctxt, copy, cur->properties);
            }
	    ctxt->inst = oldCurInst;
        }
#endif /* else of XSLT_REFACTORED */

        /*
         * Descend into content in document order.
         */
        if (cur->children != NULL) {
            if (cur->children->type != XML_ENTITY_DECL) {
                cur = cur->children;
		level++;
                if (copy != NULL)
                    insert = copy;
                continue;
            }
        }

skip_children:
	/*
	* If xslt:message was just processed, we might have hit a
	* terminate='yes'; if so, then break the loop and clean up.
	* TODO: Do we need to check this also before trying to descend
	*  into the content?
	*/
	if (ctxt->state == XSLT_STATE_STOPPED)
	    break;
        if (cur->next != NULL) {
            cur = cur->next;
            continue;
        }

        do {
            cur = cur->parent;
	    level--;
	    /*
	    * Pop variables/params (xsl:variable and xsl:param).
	    */
	    if ((ctxt->varsNr > oldVarsNr) && (ctxt->vars->level > level)) {
		xsltLocalVariablePop(ctxt, oldVarsNr, level);
	    }

            insert = insert->parent;
            if (cur == NULL)
                break;
            if (cur == list->parent) {
                cur = NULL;
                break;
            }
            if (cur->next != NULL) {
                cur = cur->next;
                break;
            }
        } while (cur != NULL);
    }

error:
    /*
    * In case of errors: pop remaining variables.
    */
    if (ctxt->varsNr > oldVarsNr)
	xsltLocalVariablePop(ctxt, oldVarsNr, -1);

    ctxt->node = oldContextNode;
    ctxt->inst = oldInst;
    ctxt->insert = oldInsert;

    ctxt->depth--;

#ifdef WITH_DEBUGGER
    if ((ctxt->debugStatus != XSLT_DEBUG_NONE) && (addCallResult)) {
        xslDropCall();
    }
#endif
}