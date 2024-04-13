xmlRegFreeAtom(xmlRegAtomPtr atom) {
    int i;

    if (atom == NULL)
	return;

    for (i = 0;i < atom->nbRanges;i++)
	xmlRegFreeRange(atom->ranges[i]);
    if (atom->ranges != NULL)
	xmlFree(atom->ranges);
    if ((atom->type == XML_REGEXP_STRING) && (atom->valuep != NULL))
	xmlFree(atom->valuep);
    if ((atom->type == XML_REGEXP_STRING) && (atom->valuep2 != NULL))
	xmlFree(atom->valuep2);
    if ((atom->type == XML_REGEXP_BLOCK_NAME) && (atom->valuep != NULL))
	xmlFree(atom->valuep);
    xmlFree(atom);
}