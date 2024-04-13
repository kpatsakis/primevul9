xmlFAEqualAtoms(xmlRegAtomPtr atom1, xmlRegAtomPtr atom2, int deep) {
    int ret = 0;

    if (atom1 == atom2)
	return(1);
    if ((atom1 == NULL) || (atom2 == NULL))
	return(0);

    if (atom1->type != atom2->type)
        return(0);
    switch (atom1->type) {
        case XML_REGEXP_EPSILON:
	    ret = 0;
	    break;
        case XML_REGEXP_STRING:
            if (!deep)
                ret = (atom1->valuep == atom2->valuep);
            else
                ret = xmlStrEqual((xmlChar *)atom1->valuep,
                                  (xmlChar *)atom2->valuep);
	    break;
        case XML_REGEXP_CHARVAL:
	    ret = (atom1->codepoint == atom2->codepoint);
	    break;
	case XML_REGEXP_RANGES:
	    /* too hard to do in the general case */
	    ret = 0;
	default:
	    break;
    }
    return(ret);
}