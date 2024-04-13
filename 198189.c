xmlFARegExec(xmlRegexpPtr comp, const xmlChar *content) {
    xmlRegExecCtxt execval;
    xmlRegExecCtxtPtr exec = &execval;
    int ret, codepoint = 0, len, deter;

    exec->inputString = content;
    exec->index = 0;
    exec->nbPush = 0;
    exec->determinist = 1;
    exec->maxRollbacks = 0;
    exec->nbRollbacks = 0;
    exec->rollbacks = NULL;
    exec->status = 0;
    exec->comp = comp;
    exec->state = comp->states[0];
    exec->transno = 0;
    exec->transcount = 0;
    exec->inputStack = NULL;
    exec->inputStackMax = 0;
    if (comp->nbCounters > 0) {
	exec->counts = (int *) xmlMalloc(comp->nbCounters * sizeof(int));
	if (exec->counts == NULL) {
	    xmlRegexpErrMemory(NULL, "running regexp");
	    return(-1);
	}
        memset(exec->counts, 0, comp->nbCounters * sizeof(int));
    } else
	exec->counts = NULL;
    while ((exec->status == 0) && (exec->state != NULL) &&
	   ((exec->inputString[exec->index] != 0) ||
	    ((exec->state != NULL) &&
	     (exec->state->type != XML_REGEXP_FINAL_STATE)))) {
	xmlRegTransPtr trans;
	xmlRegAtomPtr atom;

	/*
	 * If end of input on non-terminal state, rollback, however we may
	 * still have epsilon like transition for counted transitions
	 * on counters, in that case don't break too early.  Additionally,
	 * if we are working on a range like "AB{0,2}", where B is not present,
	 * we don't want to break.
	 */
	len = 1;
	if ((exec->inputString[exec->index] == 0) && (exec->counts == NULL)) {
	    /*
	     * if there is a transition, we must check if
	     *  atom allows minOccurs of 0
	     */
	    if (exec->transno < exec->state->nbTrans) {
	        trans = &exec->state->trans[exec->transno];
		if (trans->to >=0) {
		    atom = trans->atom;
		    if (!((atom->min == 0) && (atom->max > 0)))
		        goto rollback;
		}
	    } else
	        goto rollback;
	}

	exec->transcount = 0;
	for (;exec->transno < exec->state->nbTrans;exec->transno++) {
	    trans = &exec->state->trans[exec->transno];
	    if (trans->to < 0)
		continue;
	    atom = trans->atom;
	    ret = 0;
	    deter = 1;
	    if (trans->count >= 0) {
		int count;
		xmlRegCounterPtr counter;

		if (exec->counts == NULL) {
		    exec->status = -1;
		    goto error;
		}
		/*
		 * A counted transition.
		 */

		count = exec->counts[trans->count];
		counter = &exec->comp->counters[trans->count];
#ifdef DEBUG_REGEXP_EXEC
		printf("testing count %d: val %d, min %d, max %d\n",
		       trans->count, count, counter->min,  counter->max);
#endif
		ret = ((count >= counter->min) && (count <= counter->max));
		if ((ret) && (counter->min != counter->max))
		    deter = 0;
	    } else if (atom == NULL) {
		fprintf(stderr, "epsilon transition left at runtime\n");
		exec->status = -2;
		break;
	    } else if (exec->inputString[exec->index] != 0) {
                codepoint = CUR_SCHAR(&(exec->inputString[exec->index]), len);
		ret = xmlRegCheckCharacter(atom, codepoint);
		if ((ret == 1) && (atom->min >= 0) && (atom->max > 0)) {
		    xmlRegStatePtr to = comp->states[trans->to];

		    /*
		     * this is a multiple input sequence
		     * If there is a counter associated increment it now.
		     * before potentially saving and rollback
		     * do not increment if the counter is already over the
		     * maximum limit in which case get to next transition
		     */
		    if (trans->counter >= 0) {
			xmlRegCounterPtr counter;

			if ((exec->counts == NULL) ||
			    (exec->comp == NULL) ||
			    (exec->comp->counters == NULL)) {
			    exec->status = -1;
			    goto error;
			}
			counter = &exec->comp->counters[trans->counter];
			if (exec->counts[trans->counter] >= counter->max)
			    continue; /* for loop on transitions */

#ifdef DEBUG_REGEXP_EXEC
			printf("Increasing count %d\n", trans->counter);
#endif
			exec->counts[trans->counter]++;
		    }
		    if (exec->state->nbTrans > exec->transno + 1) {
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
			exec->index += len;
			/*
			 * End of input: stop here
			 */
			if (exec->inputString[exec->index] == 0) {
			    exec->index -= len;
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
			    xmlFARegExecSave(exec);
			    exec->transno = transno;
			    exec->state = state;
			}
			codepoint = CUR_SCHAR(&(exec->inputString[exec->index]),
				              len);
			ret = xmlRegCheckCharacter(atom, codepoint);
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
		    if (trans->counter >= 0) {
			if (exec->counts == NULL) {
			    exec->status = -1;
			    goto error;
			}
#ifdef DEBUG_REGEXP_EXEC
			printf("Decreasing count %d\n", trans->counter);
#endif
			exec->counts[trans->counter]--;
		    }
		} else if ((ret == 0) && (atom->min == 0) && (atom->max > 0)) {
		    /*
		     * we don't match on the codepoint, but minOccurs of 0
		     * says that's ok.  Setting len to 0 inhibits stepping
		     * over the codepoint.
		     */
		    exec->transcount = 1;
		    len = 0;
		    ret = 1;
		}
	    } else if ((atom->min == 0) && (atom->max > 0)) {
	        /* another spot to match when minOccurs is 0 */
		exec->transcount = 1;
		len = 0;
		ret = 1;
	    }
	    if (ret == 1) {
		if ((trans->nd == 1) ||
		    ((trans->count >= 0) && (deter == 0) &&
		     (exec->state->nbTrans > exec->transno + 1))) {
#ifdef DEBUG_REGEXP_EXEC
		    if (trans->nd == 1)
		        printf("Saving on nd transition atom %d for %c at %d\n",
			       trans->atom->no, codepoint, exec->index);
		    else
		        printf("Saving on counted transition count %d for %c at %d\n",
			       trans->count, codepoint, exec->index);
#endif
		    xmlFARegExecSave(exec);
		}
		if (trans->counter >= 0) {
		    xmlRegCounterPtr counter;

                    /* make sure we don't go over the counter maximum value */
		    if ((exec->counts == NULL) ||
			(exec->comp == NULL) ||
			(exec->comp->counters == NULL)) {
			exec->status = -1;
			goto error;
		    }
		    counter = &exec->comp->counters[trans->counter];
		    if (exec->counts[trans->counter] >= counter->max)
			continue; /* for loop on transitions */
#ifdef DEBUG_REGEXP_EXEC
		    printf("Increasing count %d\n", trans->counter);
#endif
		    exec->counts[trans->counter]++;
		}
		if ((trans->count >= 0) &&
		    (trans->count < REGEXP_ALL_COUNTER)) {
		    if (exec->counts == NULL) {
		        exec->status = -1;
			goto error;
		    }
#ifdef DEBUG_REGEXP_EXEC
		    printf("resetting count %d on transition\n",
		           trans->count);
#endif
		    exec->counts[trans->count] = 0;
		}
#ifdef DEBUG_REGEXP_EXEC
		printf("entering state %d\n", trans->to);
#endif
		exec->state = comp->states[trans->to];
		exec->transno = 0;
		if (trans->atom != NULL) {
		    exec->index += len;
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
	     * Failed to find a way out
	     */
	    exec->determinist = 0;
#ifdef DEBUG_REGEXP_EXEC
	    printf("rollback from state %d on %d:%c\n", exec->state->no,
	           codepoint,codepoint);
#endif
	    xmlFARegExecRollBack(exec);
	}
progress:
	continue;
    }
error:
    if (exec->rollbacks != NULL) {
	if (exec->counts != NULL) {
	    int i;

	    for (i = 0;i < exec->maxRollbacks;i++)
		if (exec->rollbacks[i].counts != NULL)
		    xmlFree(exec->rollbacks[i].counts);
	}
	xmlFree(exec->rollbacks);
    }
    if (exec->state == NULL)
        return(-1);
    if (exec->counts != NULL)
	xmlFree(exec->counts);
    if (exec->status == 0)
	return(1);
    if (exec->status == -1) {
	if (exec->nbPush > MAX_PUSH)
	    return(-1);
	return(0);
    }
    return(exec->status);
}