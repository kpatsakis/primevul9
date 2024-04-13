xmlRegExecGetValues(xmlRegExecCtxtPtr exec, int err,
                    int *nbval, int *nbneg,
		    xmlChar **values, int *terminal) {
    int maxval;
    int nb = 0;

    if ((exec == NULL) || (nbval == NULL) || (nbneg == NULL) ||
        (values == NULL) || (*nbval <= 0))
        return(-1);

    maxval = *nbval;
    *nbval = 0;
    *nbneg = 0;
    if ((exec->comp != NULL) && (exec->comp->compact != NULL)) {
        xmlRegexpPtr comp;
	int target, i, state;

        comp = exec->comp;

	if (err) {
	    if (exec->errStateNo == -1) return(-1);
	    state = exec->errStateNo;
	} else {
	    state = exec->index;
	}
	if (terminal != NULL) {
	    if (comp->compact[state * (comp->nbstrings + 1)] ==
	        XML_REGEXP_FINAL_STATE)
		*terminal = 1;
	    else
		*terminal = 0;
	}
	for (i = 0;(i < comp->nbstrings) && (nb < maxval);i++) {
	    target = comp->compact[state * (comp->nbstrings + 1) + i + 1];
	    if ((target > 0) && (target <= comp->nbstates) &&
	        (comp->compact[(target - 1) * (comp->nbstrings + 1)] !=
		 XML_REGEXP_SINK_STATE)) {
	        values[nb++] = comp->stringMap[i];
		(*nbval)++;
	    }
	}
	for (i = 0;(i < comp->nbstrings) && (nb < maxval);i++) {
	    target = comp->compact[state * (comp->nbstrings + 1) + i + 1];
	    if ((target > 0) && (target <= comp->nbstates) &&
	        (comp->compact[(target - 1) * (comp->nbstrings + 1)] ==
		 XML_REGEXP_SINK_STATE)) {
	        values[nb++] = comp->stringMap[i];
		(*nbneg)++;
	    }
	}
    } else {
        int transno;
	xmlRegTransPtr trans;
	xmlRegAtomPtr atom;
	xmlRegStatePtr state;

	if (terminal != NULL) {
	    if (exec->state->type == XML_REGEXP_FINAL_STATE)
		*terminal = 1;
	    else
		*terminal = 0;
	}

	if (err) {
	    if (exec->errState == NULL) return(-1);
	    state = exec->errState;
	} else {
	    if (exec->state == NULL) return(-1);
	    state = exec->state;
	}
	for (transno = 0;
	     (transno < state->nbTrans) && (nb < maxval);
	     transno++) {
	    trans = &state->trans[transno];
	    if (trans->to < 0)
		continue;
	    atom = trans->atom;
	    if ((atom == NULL) || (atom->valuep == NULL))
		continue;
	    if (trans->count == REGEXP_ALL_LAX_COUNTER) {
	        /* this should not be reached but ... */
	        TODO;
	    } else if (trans->count == REGEXP_ALL_COUNTER) {
	        /* this should not be reached but ... */
	        TODO;
	    } else if (trans->counter >= 0) {
		xmlRegCounterPtr counter = NULL;
		int count;

		if (err)
		    count = exec->errCounts[trans->counter];
		else
		    count = exec->counts[trans->counter];
		if (exec->comp != NULL)
		    counter = &exec->comp->counters[trans->counter];
		if ((counter == NULL) || (count < counter->max)) {
		    if (atom->neg)
			values[nb++] = (xmlChar *) atom->valuep2;
		    else
			values[nb++] = (xmlChar *) atom->valuep;
		    (*nbval)++;
		}
	    } else {
                if ((exec->comp != NULL) && (exec->comp->states[trans->to] != NULL) &&
		    (exec->comp->states[trans->to]->type !=
		     XML_REGEXP_SINK_STATE)) {
		    if (atom->neg)
			values[nb++] = (xmlChar *) atom->valuep2;
		    else
			values[nb++] = (xmlChar *) atom->valuep;
		    (*nbval)++;
		}
	    }
	}
	for (transno = 0;
	     (transno < state->nbTrans) && (nb < maxval);
	     transno++) {
	    trans = &state->trans[transno];
	    if (trans->to < 0)
		continue;
	    atom = trans->atom;
	    if ((atom == NULL) || (atom->valuep == NULL))
		continue;
	    if (trans->count == REGEXP_ALL_LAX_COUNTER) {
	        continue;
	    } else if (trans->count == REGEXP_ALL_COUNTER) {
	        continue;
	    } else if (trans->counter >= 0) {
	        continue;
	    } else {
                if ((exec->comp->states[trans->to] != NULL) &&
		    (exec->comp->states[trans->to]->type ==
		     XML_REGEXP_SINK_STATE)) {
		    if (atom->neg)
			values[nb++] = (xmlChar *) atom->valuep2;
		    else
			values[nb++] = (xmlChar *) atom->valuep;
		    (*nbneg)++;
		}
	    }
	}
    }
    return(0);
}