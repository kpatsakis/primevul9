xmlAutomataSetFinalState(xmlAutomataPtr am, xmlAutomataStatePtr state) {
    if ((am == NULL) || (state == NULL))
	return(-1);
    state->type = XML_REGEXP_FINAL_STATE;
    return(0);
}