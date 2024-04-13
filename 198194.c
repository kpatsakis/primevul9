xmlRegEpxFromParse(xmlRegParserCtxtPtr ctxt) {
    xmlRegexpPtr ret;

    ret = (xmlRegexpPtr) xmlMalloc(sizeof(xmlRegexp));
    if (ret == NULL) {
	xmlRegexpErrMemory(ctxt, "compiling regexp");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlRegexp));
    ret->string = ctxt->string;
    ret->nbStates = ctxt->nbStates;
    ret->states = ctxt->states;
    ret->nbAtoms = ctxt->nbAtoms;
    ret->atoms = ctxt->atoms;
    ret->nbCounters = ctxt->nbCounters;
    ret->counters = ctxt->counters;
    ret->determinist = ctxt->determinist;
    ret->flags = ctxt->flags;
    if (ret->determinist == -1) {
        xmlRegexpIsDeterminist(ret);
    }

    if ((ret->determinist != 0) &&
	(ret->nbCounters == 0) &&
	(ctxt->negs == 0) &&
	(ret->atoms != NULL) &&
	(ret->atoms[0] != NULL) &&
	(ret->atoms[0]->type == XML_REGEXP_STRING)) {
	int i, j, nbstates = 0, nbatoms = 0;
	int *stateRemap;
	int *stringRemap;
	int *transitions;
	void **transdata;
	xmlChar **stringMap;
        xmlChar *value;

	/*
	 * Switch to a compact representation
	 * 1/ counting the effective number of states left
	 * 2/ counting the unique number of atoms, and check that
	 *    they are all of the string type
	 * 3/ build a table state x atom for the transitions
	 */

	stateRemap = xmlMalloc(ret->nbStates * sizeof(int));
	if (stateRemap == NULL) {
	    xmlRegexpErrMemory(ctxt, "compiling regexp");
	    xmlFree(ret);
	    return(NULL);
	}
	for (i = 0;i < ret->nbStates;i++) {
	    if (ret->states[i] != NULL) {
		stateRemap[i] = nbstates;
		nbstates++;
	    } else {
		stateRemap[i] = -1;
	    }
	}
#ifdef DEBUG_COMPACTION
	printf("Final: %d states\n", nbstates);
#endif
	stringMap = xmlMalloc(ret->nbAtoms * sizeof(char *));
	if (stringMap == NULL) {
	    xmlRegexpErrMemory(ctxt, "compiling regexp");
	    xmlFree(stateRemap);
	    xmlFree(ret);
	    return(NULL);
	}
	stringRemap = xmlMalloc(ret->nbAtoms * sizeof(int));
	if (stringRemap == NULL) {
	    xmlRegexpErrMemory(ctxt, "compiling regexp");
	    xmlFree(stringMap);
	    xmlFree(stateRemap);
	    xmlFree(ret);
	    return(NULL);
	}
	for (i = 0;i < ret->nbAtoms;i++) {
	    if ((ret->atoms[i]->type == XML_REGEXP_STRING) &&
		(ret->atoms[i]->quant == XML_REGEXP_QUANT_ONCE)) {
		value = ret->atoms[i]->valuep;
                for (j = 0;j < nbatoms;j++) {
		    if (xmlStrEqual(stringMap[j], value)) {
			stringRemap[i] = j;
			break;
		    }
		}
		if (j >= nbatoms) {
		    stringRemap[i] = nbatoms;
		    stringMap[nbatoms] = xmlStrdup(value);
		    if (stringMap[nbatoms] == NULL) {
			for (i = 0;i < nbatoms;i++)
			    xmlFree(stringMap[i]);
			xmlFree(stringRemap);
			xmlFree(stringMap);
			xmlFree(stateRemap);
			xmlFree(ret);
			return(NULL);
		    }
		    nbatoms++;
		}
	    } else {
		xmlFree(stateRemap);
		xmlFree(stringRemap);
		for (i = 0;i < nbatoms;i++)
		    xmlFree(stringMap[i]);
		xmlFree(stringMap);
		xmlFree(ret);
		return(NULL);
	    }
	}
#ifdef DEBUG_COMPACTION
	printf("Final: %d atoms\n", nbatoms);
#endif
	transitions = (int *) xmlMalloc((nbstates + 1) *
	                                (nbatoms + 1) * sizeof(int));
	if (transitions == NULL) {
	    xmlFree(stateRemap);
	    xmlFree(stringRemap);
	    xmlFree(stringMap);
	    xmlFree(ret);
	    return(NULL);
	}
	memset(transitions, 0, (nbstates + 1) * (nbatoms + 1) * sizeof(int));

	/*
	 * Allocate the transition table. The first entry for each
	 * state corresponds to the state type.
	 */
	transdata = NULL;

	for (i = 0;i < ret->nbStates;i++) {
	    int stateno, atomno, targetno, prev;
	    xmlRegStatePtr state;
	    xmlRegTransPtr trans;

	    stateno = stateRemap[i];
	    if (stateno == -1)
		continue;
	    state = ret->states[i];

	    transitions[stateno * (nbatoms + 1)] = state->type;

	    for (j = 0;j < state->nbTrans;j++) {
		trans = &(state->trans[j]);
		if ((trans->to == -1) || (trans->atom == NULL))
		    continue;
                atomno = stringRemap[trans->atom->no];
		if ((trans->atom->data != NULL) && (transdata == NULL)) {
		    transdata = (void **) xmlMalloc(nbstates * nbatoms *
			                            sizeof(void *));
		    if (transdata != NULL)
			memset(transdata, 0,
			       nbstates * nbatoms * sizeof(void *));
		    else {
			xmlRegexpErrMemory(ctxt, "compiling regexp");
			break;
		    }
		}
		targetno = stateRemap[trans->to];
		/*
		 * if the same atom can generate transitions to 2 different
		 * states then it means the automata is not determinist and
		 * the compact form can't be used !
		 */
		prev = transitions[stateno * (nbatoms + 1) + atomno + 1];
		if (prev != 0) {
		    if (prev != targetno + 1) {
			ret->determinist = 0;
#ifdef DEBUG_COMPACTION
			printf("Indet: state %d trans %d, atom %d to %d : %d to %d\n",
			       i, j, trans->atom->no, trans->to, atomno, targetno);
			printf("       previous to is %d\n", prev);
#endif
			if (transdata != NULL)
			    xmlFree(transdata);
			xmlFree(transitions);
			xmlFree(stateRemap);
			xmlFree(stringRemap);
			for (i = 0;i < nbatoms;i++)
			    xmlFree(stringMap[i]);
			xmlFree(stringMap);
			goto not_determ;
		    }
		} else {
#if 0
		    printf("State %d trans %d: atom %d to %d : %d to %d\n",
			   i, j, trans->atom->no, trans->to, atomno, targetno);
#endif
		    transitions[stateno * (nbatoms + 1) + atomno + 1] =
			targetno + 1; /* to avoid 0 */
		    if (transdata != NULL)
			transdata[stateno * nbatoms + atomno] =
			    trans->atom->data;
		}
	    }
	}
	ret->determinist = 1;
#ifdef DEBUG_COMPACTION
	/*
	 * Debug
	 */
	for (i = 0;i < nbstates;i++) {
	    for (j = 0;j < nbatoms + 1;j++) {
                printf("%02d ", transitions[i * (nbatoms + 1) + j]);
	    }
	    printf("\n");
	}
	printf("\n");
#endif
	/*
	 * Cleanup of the old data
	 */
	if (ret->states != NULL) {
	    for (i = 0;i < ret->nbStates;i++)
		xmlRegFreeState(ret->states[i]);
	    xmlFree(ret->states);
	}
	ret->states = NULL;
	ret->nbStates = 0;
	if (ret->atoms != NULL) {
	    for (i = 0;i < ret->nbAtoms;i++)
		xmlRegFreeAtom(ret->atoms[i]);
	    xmlFree(ret->atoms);
	}
	ret->atoms = NULL;
	ret->nbAtoms = 0;

	ret->compact = transitions;
	ret->transdata = transdata;
	ret->stringMap = stringMap;
	ret->nbstrings = nbatoms;
	ret->nbstates = nbstates;
	xmlFree(stateRemap);
	xmlFree(stringRemap);
    }
not_determ:
    ctxt->string = NULL;
    ctxt->nbStates = 0;
    ctxt->states = NULL;
    ctxt->nbAtoms = 0;
    ctxt->atoms = NULL;
    ctxt->nbCounters = 0;
    ctxt->counters = NULL;
    return(ret);
}