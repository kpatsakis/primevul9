xmlRegExecPushChar(xmlRegExecCtxtPtr exec, int UCS) {
    xmlRegTransPtr trans;
    xmlRegAtomPtr atom;
    int ret;
    int codepoint, len;

    if (exec == NULL)
	return(-1);
    if (exec->status != 0)
	return(exec->status);

    while ((exec->status == 0) &&
	   ((exec->inputString[exec->index] != 0) ||
	    (exec->state->type != XML_REGEXP_FINAL_STATE))) {

	/*
	 * End of input on non-terminal state, rollback, however we may
	 * still have epsilon like transition for counted transitions
	 * on counters, in that case don't break too early.
	 */
	if ((exec->inputString[exec->index] == 0) && (exec->counts == NULL))
	    goto rollback;

	exec->transcount = 0;
	for (;exec->transno < exec->state->nbTrans;exec->transno++) {
	    trans = &exec->state->trans[exec->transno];
	    if (trans->to < 0)
		continue;
	    atom = trans->atom;
	    ret = 0;
	    if (trans->count >= 0) {
		int count;
		xmlRegCounterPtr counter;

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
	    } else if (atom == NULL) {
		fprintf(stderr, "epsilon transition left at runtime\n");
		exec->status = -2;
		break;
	    } else if (exec->inputString[exec->index] != 0) {
                codepoint = CUR_SCHAR(&(exec->inputString[exec->index]), len);
		ret = xmlRegCheckCharacter(atom, codepoint);
		if ((ret == 1) && (atom->min > 0) && (atom->max > 0)) {
		    xmlRegStatePtr to = exec->comp->states[trans->to];

		    /*
		     * this is a multiple input sequence
		     */
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
		}
	    }
	    if (ret == 1) {
		if (exec->state->nbTrans > exec->transno + 1) {
		    xmlFARegExecSave(exec);
		}
		/*
		 * restart count for expressions like this ((abc){2})*
		 */
		if (trans->count >= 0) {
#ifdef DEBUG_REGEXP_EXEC
		    printf("Reset count %d\n", trans->count);
#endif
		    exec->counts[trans->count] = 0;
		}
		if (trans->counter >= 0) {
#ifdef DEBUG_REGEXP_EXEC
		    printf("Increasing count %d\n", trans->counter);
#endif
		    exec->counts[trans->counter]++;
		}
#ifdef DEBUG_REGEXP_EXEC
		printf("entering state %d\n", trans->to);
#endif
		exec->state = exec->comp->states[trans->to];
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
	    xmlFARegExecRollBack(exec);
	}
progress:
	continue;
    }
}