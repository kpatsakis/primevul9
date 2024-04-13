xmlRegExecPushStringInternal(xmlRegExecCtxtPtr exec, const xmlChar *value,
	                     void *data, int compound) {
    xmlRegTransPtr trans;
    xmlRegAtomPtr atom;
    int ret;
    int final = 0;
    int progress = 1;

    if (exec == NULL)
	return(-1);
    if (exec->comp == NULL)
	return(-1);
    if (exec->status != 0)
	return(exec->status);

    if (exec->comp->compact != NULL)
	return(xmlRegCompactPushString(exec, exec->comp, value, data));

    if (value == NULL) {
        if (exec->state->type == XML_REGEXP_FINAL_STATE)
	    return(1);
	final = 1;
    }

#ifdef DEBUG_PUSH
    printf("value pushed: %s\n", value);
#endif
    /*
     * If we have an active rollback stack push the new value there
     * and get back to where we were left
     */
    if ((value != NULL) && (exec->inputStackNr > 0)) {
	xmlFARegExecSaveInputString(exec, value, data);
	value = exec->inputStack[exec->index].value;
	data = exec->inputStack[exec->index].data;
#ifdef DEBUG_PUSH
	printf("value loaded: %s\n", value);
#endif
    }

    while ((exec->status == 0) &&
	   ((value != NULL) ||
	    ((final == 1) &&
	     (exec->state->type != XML_REGEXP_FINAL_STATE)))) {

	/*
	 * End of input on non-terminal state, rollback, however we may
	 * still have epsilon like transition for counted transitions
	 * on counters, in that case don't break too early.
	 */
	if ((value == NULL) && (exec->counts == NULL))
	    goto rollback;

	exec->transcount = 0;
	for (;exec->transno < exec->state->nbTrans;exec->transno++) {
	    trans = &exec->state->trans[exec->transno];
	    if (trans->to < 0)
		continue;
	    atom = trans->atom;
	    ret = 0;
	    if (trans->count == REGEXP_ALL_LAX_COUNTER) {
		int i;
		int count;
		xmlRegTransPtr t;
		xmlRegCounterPtr counter;

		ret = 0;

#ifdef DEBUG_PUSH
		printf("testing all lax %d\n", trans->count);
#endif
		/*
		 * Check all counted transitions from the current state
		 */
		if ((value == NULL) && (final)) {
		    ret = 1;
		} else if (value != NULL) {
		    for (i = 0;i < exec->state->nbTrans;i++) {
			t = &exec->state->trans[i];
			if ((t->counter < 0) || (t == trans))
			    continue;
			counter = &exec->comp->counters[t->counter];
			count = exec->counts[t->counter];
			if ((count < counter->max) &&
		            (t->atom != NULL) &&
			    (xmlStrEqual(value, t->atom->valuep))) {
			    ret = 0;
			    break;
			}
			if ((count >= counter->min) &&
			    (count < counter->max) &&
			    (t->atom != NULL) &&
			    (xmlStrEqual(value, t->atom->valuep))) {
			    ret = 1;
			    break;
			}
		    }
		}
	    } else if (trans->count == REGEXP_ALL_COUNTER) {
		int i;
		int count;
		xmlRegTransPtr t;
		xmlRegCounterPtr counter;

		ret = 1;

#ifdef DEBUG_PUSH
		printf("testing all %d\n", trans->count);
#endif
		/*
		 * Check all counted transitions from the current state
		 */
		for (i = 0;i < exec->state->nbTrans;i++) {
                    t = &exec->state->trans[i];
		    if ((t->counter < 0) || (t == trans))
			continue;
                    counter = &exec->comp->counters[t->counter];
		    count = exec->counts[t->counter];
		    if ((count < counter->min) || (count > counter->max)) {
			ret = 0;
			break;
		    }
		}
	    } else if (trans->count >= 0) {
		int count;
		xmlRegCounterPtr counter;

		/*
		 * A counted transition.
		 */

		count = exec->counts[trans->count];
		counter = &exec->comp->counters[trans->count];
#ifdef DEBUG_PUSH
		printf("testing count %d: val %d, min %d, max %d\n",
		       trans->count, count, counter->min,  counter->max);
#endif
		ret = ((count >= counter->min) && (count <= counter->max));
	    } else if (atom == NULL) {
		fprintf(stderr, "epsilon transition left at runtime\n");
		exec->status = -2;
		break;
	    } else if (value != NULL) {
		ret = xmlRegStrEqualWildcard(atom->valuep, value);
		if (atom->neg) {
		    ret = !ret;
		    if (!compound)
		        ret = 0;
		}
		if ((ret == 1) && (trans->counter >= 0)) {
		    xmlRegCounterPtr counter;
		    int count;

		    count = exec->counts[trans->counter];
		    counter = &exec->comp->counters[trans->counter];
		    if (count >= counter->max)
			ret = 0;
		}

		if ((ret == 1) && (atom->min > 0) && (atom->max > 0)) {
		    xmlRegStatePtr to = exec->comp->states[trans->to];

		    /*
		     * this is a multiple input sequence
		     */
		    if (exec->state->nbTrans > exec->transno + 1) {
			if (exec->inputStackNr <= 0) {
			    xmlFARegExecSaveInputString(exec, value, data);
			}
			xmlFARegExecSave(exec);
		    }
		    exec->transcount = 1;
		    do {
			/*
			 * Try to progress as much as possible on the input
			 */
			if (exec->transcount == atom->max) {
			    break;
			}
			exec->index++;
			value = exec->inputStack[exec->index].value;
			data = exec->inputStack[exec->index].data;
#ifdef DEBUG_PUSH
			printf("value loaded: %s\n", value);
#endif

			/*
			 * End of input: stop here
			 */
			if (value == NULL) {
			    exec->index --;
			    break;
			}
			if (exec->transcount >= atom->min) {
			    int transno = exec->transno;
			    xmlRegStatePtr state = exec->state;

			    /*
			     * The transition is acceptable save it
			     */
			    exec->transno = -1; /* trick */
			    exec->state = to;
			    if (exec->inputStackNr <= 0) {
				xmlFARegExecSaveInputString(exec, value, data);
			    }
			    xmlFARegExecSave(exec);
			    exec->transno = transno;
			    exec->state = state;
			}
			ret = xmlStrEqual(value, atom->valuep);
			exec->transcount++;
		    } while (ret == 1);
		    if (exec->transcount < atom->min)
			ret = 0;

		    /*
		     * If the last check failed but one transition was found
		     * possible, rollback
		     */
		    if (ret < 0)
			ret = 0;
		    if (ret == 0) {
			goto rollback;
		    }
		}
	    }
	    if (ret == 1) {
		if ((exec->callback != NULL) && (atom != NULL) &&
			(data != NULL)) {
		    exec->callback(exec->data, atom->valuep,
			           atom->data, data);
		}
		if (exec->state->nbTrans > exec->transno + 1) {
		    if (exec->inputStackNr <= 0) {
			xmlFARegExecSaveInputString(exec, value, data);
		    }
		    xmlFARegExecSave(exec);
		}
		if (trans->counter >= 0) {
#ifdef DEBUG_PUSH
		    printf("Increasing count %d\n", trans->counter);
#endif
		    exec->counts[trans->counter]++;
		}
		if ((trans->count >= 0) &&
		    (trans->count < REGEXP_ALL_COUNTER)) {
#ifdef DEBUG_REGEXP_EXEC
		    printf("resetting count %d on transition\n",
		           trans->count);
#endif
		    exec->counts[trans->count] = 0;
		}
#ifdef DEBUG_PUSH
		printf("entering state %d\n", trans->to);
#endif
                if ((exec->comp->states[trans->to] != NULL) &&
		    (exec->comp->states[trans->to]->type ==
		     XML_REGEXP_SINK_STATE)) {
		    /*
		     * entering a sink state, save the current state as error
		     * state.
		     */
		    if (exec->errString != NULL)
			xmlFree(exec->errString);
		    exec->errString = xmlStrdup(value);
		    exec->errState = exec->state;
		    memcpy(exec->errCounts, exec->counts,
			   exec->comp->nbCounters * sizeof(int));
		}
		exec->state = exec->comp->states[trans->to];
		exec->transno = 0;
		if (trans->atom != NULL) {
		    if (exec->inputStack != NULL) {
			exec->index++;
			if (exec->index < exec->inputStackNr) {
			    value = exec->inputStack[exec->index].value;
			    data = exec->inputStack[exec->index].data;
#ifdef DEBUG_PUSH
			    printf("value loaded: %s\n", value);
#endif
			} else {
			    value = NULL;
			    data = NULL;
#ifdef DEBUG_PUSH
			    printf("end of input\n");
#endif
			}
		    } else {
			value = NULL;
			data = NULL;
#ifdef DEBUG_PUSH
			printf("end of input\n");
#endif
		    }
		}
		goto progress;
	    } else if (ret < 0) {
		exec->status = -4;
		break;
	    }
	}
	if ((exec->transno != 0) || (exec->state->nbTrans == 0)) {
rollback:
            /*
	     * if we didn't yet rollback on the current input
	     * store the current state as the error state.
	     */
	    if ((progress) && (exec->state != NULL) &&
	        (exec->state->type != XML_REGEXP_SINK_STATE)) {
	        progress = 0;
		if (exec->errString != NULL)
		    xmlFree(exec->errString);
		exec->errString = xmlStrdup(value);
		exec->errState = exec->state;
		memcpy(exec->errCounts, exec->counts,
		       exec->comp->nbCounters * sizeof(int));
	    }

	    /*
	     * Failed to find a way out
	     */
	    exec->determinist = 0;
	    xmlFARegExecRollBack(exec);
	    if ((exec->inputStack != NULL ) && (exec->status == 0)) {
		value = exec->inputStack[exec->index].value;
		data = exec->inputStack[exec->index].data;
#ifdef DEBUG_PUSH
		printf("value loaded: %s\n", value);
#endif
	    }
	}
	continue;
progress:
        progress = 1;
	continue;
    }
    if (exec->status == 0) {
        return(exec->state->type == XML_REGEXP_FINAL_STATE);
    }
#ifdef DEBUG_ERR
    if (exec->status < 0) {
	testerr(exec);
    }
#endif
    return(exec->status);
}