xmlExpHashGetEntry(xmlExpCtxtPtr ctxt, xmlExpNodeType type,
                   xmlExpNodePtr left, xmlExpNodePtr right,
		   const xmlChar *name, int min, int max) {
    unsigned short kbase, key;
    xmlExpNodePtr entry;
    xmlExpNodePtr insert;

    if (ctxt == NULL)
	return(NULL);

    /*
     * Check for duplicate and insertion location.
     */
    if (type == XML_EXP_ATOM) {
	kbase = xmlExpHashNameComputeKey(name);
    } else if (type == XML_EXP_COUNT) {
        /* COUNT reduction rule 1 */
	/* a{1} -> a */
	if (min == max) {
	    if (min == 1) {
		return(left);
	    }
	    if (min == 0) {
		xmlExpFree(ctxt, left);
	        return(emptyExp);
	    }
	}
	if (min < 0) {
	    xmlExpFree(ctxt, left);
	    return(forbiddenExp);
	}
        if (max == -1)
	    kbase = min + 79;
	else
	    kbase = max - min;
	kbase += left->key;
    } else if (type == XML_EXP_OR) {
        /* Forbid reduction rules */
        if (left->type == XML_EXP_FORBID) {
	    xmlExpFree(ctxt, left);
	    return(right);
	}
        if (right->type == XML_EXP_FORBID) {
	    xmlExpFree(ctxt, right);
	    return(left);
	}

        /* OR reduction rule 1 */
	/* a | a reduced to a */
        if (left == right) {
	    left->ref--;
	    return(left);
	}
        /* OR canonicalization rule 1 */
	/* linearize (a | b) | c into a | (b | c) */
        if ((left->type == XML_EXP_OR) && (right->type != XML_EXP_OR)) {
	    xmlExpNodePtr tmp = left;
            left = right;
	    right = tmp;
	}
        /* OR reduction rule 2 */
	/* a | (a | b) and b | (a | b) are reduced to a | b */
        if (right->type == XML_EXP_OR) {
	    if ((left == right->exp_left) ||
	        (left == right->exp_right)) {
		xmlExpFree(ctxt, left);
		return(right);
	    }
	}
        /* OR canonicalization rule 2 */
	/* linearize (a | b) | c into a | (b | c) */
        if (left->type == XML_EXP_OR) {
	    xmlExpNodePtr tmp;

	    /* OR canonicalization rule 2 */
	    if ((left->exp_right->type != XML_EXP_OR) &&
	        (left->exp_right->key < left->exp_left->key)) {
	        tmp = left->exp_right;
		left->exp_right = left->exp_left;
		left->exp_left = tmp;
	    }
	    left->exp_right->ref++;
	    tmp = xmlExpHashGetEntry(ctxt, XML_EXP_OR, left->exp_right, right,
	                             NULL, 0, 0);
	    left->exp_left->ref++;
	    tmp = xmlExpHashGetEntry(ctxt, XML_EXP_OR, left->exp_left, tmp,
	                             NULL, 0, 0);

	    xmlExpFree(ctxt, left);
	    return(tmp);
	}
	if (right->type == XML_EXP_OR) {
	    /* Ordering in the tree */
	    /* C | (A | B) -> A | (B | C) */
	    if (left->key > right->exp_right->key) {
		xmlExpNodePtr tmp;
		right->exp_right->ref++;
		tmp = xmlExpHashGetEntry(ctxt, XML_EXP_OR, right->exp_right,
		                         left, NULL, 0, 0);
		right->exp_left->ref++;
		tmp = xmlExpHashGetEntry(ctxt, XML_EXP_OR, right->exp_left,
		                         tmp, NULL, 0, 0);
		xmlExpFree(ctxt, right);
		return(tmp);
	    }
	    /* Ordering in the tree */
	    /* B | (A | C) -> A | (B | C) */
	    if (left->key > right->exp_left->key) {
		xmlExpNodePtr tmp;
		right->exp_right->ref++;
		tmp = xmlExpHashGetEntry(ctxt, XML_EXP_OR, left,
		                         right->exp_right, NULL, 0, 0);
		right->exp_left->ref++;
		tmp = xmlExpHashGetEntry(ctxt, XML_EXP_OR, right->exp_left,
		                         tmp, NULL, 0, 0);
		xmlExpFree(ctxt, right);
		return(tmp);
	    }
	}
	/* we know both types are != XML_EXP_OR here */
        else if (left->key > right->key) {
	    xmlExpNodePtr tmp = left;
            left = right;
	    right = tmp;
	}
	kbase = xmlExpHashComputeKey(type, left, right);
    } else if (type == XML_EXP_SEQ) {
        /* Forbid reduction rules */
        if (left->type == XML_EXP_FORBID) {
	    xmlExpFree(ctxt, right);
	    return(left);
	}
        if (right->type == XML_EXP_FORBID) {
	    xmlExpFree(ctxt, left);
	    return(right);
	}
        /* Empty reduction rules */
        if (right->type == XML_EXP_EMPTY) {
	    return(left);
	}
        if (left->type == XML_EXP_EMPTY) {
	    return(right);
	}
	kbase = xmlExpHashComputeKey(type, left, right);
    } else
        return(NULL);

    key = kbase % ctxt->size;
    if (ctxt->table[key] != NULL) {
	for (insert = ctxt->table[key]; insert != NULL;
	     insert = insert->next) {
	    if ((insert->key == kbase) &&
	        (insert->type == type)) {
		if (type == XML_EXP_ATOM) {
		    if (name == insert->exp_str) {
			insert->ref++;
			return(insert);
		    }
		} else if (type == XML_EXP_COUNT) {
		    if ((insert->exp_min == min) && (insert->exp_max == max) &&
		        (insert->exp_left == left)) {
			insert->ref++;
			left->ref--;
			return(insert);
		    }
		} else if ((insert->exp_left == left) &&
			   (insert->exp_right == right)) {
		    insert->ref++;
		    left->ref--;
		    right->ref--;
		    return(insert);
		}
	    }
	}
    }

    entry = xmlExpNewNode(ctxt, type);
    if (entry == NULL)
        return(NULL);
    entry->key = kbase;
    if (type == XML_EXP_ATOM) {
	entry->exp_str = name;
	entry->c_max = 1;
    } else if (type == XML_EXP_COUNT) {
        entry->exp_min = min;
        entry->exp_max = max;
	entry->exp_left = left;
	if ((min == 0) || (IS_NILLABLE(left)))
	    entry->info |= XML_EXP_NILABLE;
	if (max < 0)
	    entry->c_max = -1;
	else
	    entry->c_max = max * entry->exp_left->c_max;
    } else {
	entry->exp_left = left;
	entry->exp_right = right;
	if (type == XML_EXP_OR) {
	    if ((IS_NILLABLE(left)) || (IS_NILLABLE(right)))
		entry->info |= XML_EXP_NILABLE;
	    if ((entry->exp_left->c_max == -1) ||
	        (entry->exp_right->c_max == -1))
		entry->c_max = -1;
	    else if (entry->exp_left->c_max > entry->exp_right->c_max)
	        entry->c_max = entry->exp_left->c_max;
	    else
	        entry->c_max = entry->exp_right->c_max;
	} else {
	    if ((IS_NILLABLE(left)) && (IS_NILLABLE(right)))
		entry->info |= XML_EXP_NILABLE;
	    if ((entry->exp_left->c_max == -1) ||
	        (entry->exp_right->c_max == -1))
		entry->c_max = -1;
	    else
	        entry->c_max = entry->exp_left->c_max + entry->exp_right->c_max;
	}
    }
    entry->ref = 1;
    if (ctxt->table[key] != NULL)
        entry->next = ctxt->table[key];

    ctxt->table[key] = entry;
    ctxt->nbElems++;

    return(entry);
}