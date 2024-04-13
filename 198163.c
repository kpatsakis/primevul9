xmlExpFree(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp) {
    if ((exp == NULL) || (exp == forbiddenExp) || (exp == emptyExp))
        return;
    exp->ref--;
    if (exp->ref == 0) {
        unsigned short key;

        /* Unlink it first from the hash table */
	key = exp->key % ctxt->size;
	if (ctxt->table[key] == exp) {
	    ctxt->table[key] = exp->next;
	} else {
	    xmlExpNodePtr tmp;

	    tmp = ctxt->table[key];
	    while (tmp != NULL) {
	        if (tmp->next == exp) {
		    tmp->next = exp->next;
		    break;
		}
	        tmp = tmp->next;
	    }
	}

        if ((exp->type == XML_EXP_SEQ) || (exp->type == XML_EXP_OR)) {
	    xmlExpFree(ctxt, exp->exp_left);
	    xmlExpFree(ctxt, exp->exp_right);
	} else if (exp->type == XML_EXP_COUNT) {
	    xmlExpFree(ctxt, exp->exp_left);
	}
        xmlFree(exp);
	ctxt->nb_nodes--;
    }
}