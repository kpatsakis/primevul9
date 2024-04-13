xmlRegFreeState(xmlRegStatePtr state) {
    if (state == NULL)
	return;

    if (state->trans != NULL)
	xmlFree(state->trans);
    if (state->transTo != NULL)
	xmlFree(state->transTo);
    xmlFree(state);
}