xmlFACompareAtoms(xmlRegAtomPtr atom1, xmlRegAtomPtr atom2, int deep) {
    int ret = 1;

    if (atom1 == atom2)
	return(1);
    if ((atom1 == NULL) || (atom2 == NULL))
	return(0);

    if ((atom1->type == XML_REGEXP_ANYCHAR) ||
        (atom2->type == XML_REGEXP_ANYCHAR))
	return(1);

    if (atom1->type > atom2->type) {
	xmlRegAtomPtr tmp;
	tmp = atom1;
	atom1 = atom2;
	atom2 = tmp;
    }
    if (atom1->type != atom2->type) {
        ret = xmlFACompareAtomTypes(atom1->type, atom2->type);
	/* if they can't intersect at the type level break now */
	if (ret == 0)
	    return(0);
    }
    switch (atom1->type) {
        case XML_REGEXP_STRING:
            if (!deep)
                ret = (atom1->valuep != atom2->valuep);
            else
                ret = xmlRegStrEqualWildcard((xmlChar *)atom1->valuep,
                                             (xmlChar *)atom2->valuep);
	    break;
        case XML_REGEXP_EPSILON:
	    goto not_determinist;
        case XML_REGEXP_CHARVAL:
	    if (atom2->type == XML_REGEXP_CHARVAL) {
		ret = (atom1->codepoint == atom2->codepoint);
	    } else {
	        ret = xmlRegCheckCharacter(atom2, atom1->codepoint);
		if (ret < 0)
		    ret = 1;
	    }
	    break;
        case XML_REGEXP_RANGES:
	    if (atom2->type == XML_REGEXP_RANGES) {
	        int i, j, res;
		xmlRegRangePtr r1, r2;

		/*
		 * need to check that none of the ranges eventually matches
		 */
		for (i = 0;i < atom1->nbRanges;i++) {
		    for (j = 0;j < atom2->nbRanges;j++) {
			r1 = atom1->ranges[i];
			r2 = atom2->ranges[j];
			res = xmlFACompareRanges(r1, r2);
			if (res == 1) {
			    ret = 1;
			    goto done;
			}
		    }
		}
		ret = 0;
	    }
	    break;
	default:
	    goto not_determinist;
    }
done:
    if (atom1->neg != atom2->neg) {
        ret = !ret;
    }
    if (ret == 0)
        return(0);
not_determinist:
    return(1);
}