xmlAutomataSetFlags(xmlAutomataPtr am, int flags) {
    if (am == NULL)
	return;
    am->flags |= flags;
}