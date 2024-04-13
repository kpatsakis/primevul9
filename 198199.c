xmlRegCompactPushString(xmlRegExecCtxtPtr exec,
	                xmlRegexpPtr comp,
	                const xmlChar *value,
	                void *data) {
    int state = exec->index;
    int i, target;

    if ((comp == NULL) || (comp->compact == NULL) || (comp->stringMap == NULL))
	return(-1);

    if (value == NULL) {
	/*
	 * are we at a final state ?
	 */
	if (comp->compact[state * (comp->nbstrings + 1)] ==
            XML_REGEXP_FINAL_STATE)
	    return(1);
	return(0);
    }

#ifdef DEBUG_PUSH
    printf("value pushed: %s\n", value);
#endif

    /*
     * Examine all outside transitions from current state
     */
    for (i = 0;i < comp->nbstrings;i++) {
	target = comp->compact[state * (comp->nbstrings + 1) + i + 1];
	if ((target > 0) && (target <= comp->nbstates)) {
	    target--; /* to avoid 0 */
	    if (xmlRegStrEqualWildcard(comp->stringMap[i], value)) {
		exec->index = target;
		if ((exec->callback != NULL) && (comp->transdata != NULL)) {
		    exec->callback(exec->data, value,
			  comp->transdata[state * comp->nbstrings + i], data);
		}
#ifdef DEBUG_PUSH
		printf("entering state %d\n", target);
#endif
		if (comp->compact[target * (comp->nbstrings + 1)] ==
		    XML_REGEXP_SINK_STATE)
		    goto error;

		if (comp->compact[target * (comp->nbstrings + 1)] ==
		    XML_REGEXP_FINAL_STATE)
		    return(1);
		return(0);
	    }
	}
    }
    /*
     * Failed to find an exit transition out from current state for the
     * current token
     */
#ifdef DEBUG_PUSH
    printf("failed to find a transition for %s on state %d\n", value, state);
#endif
error:
    if (exec->errString != NULL)
        xmlFree(exec->errString);
    exec->errString = xmlStrdup(value);
    exec->errStateNo = state;
    exec->status = -1;
#ifdef DEBUG_ERR
    testerr(exec);
#endif
    return(-1);
}