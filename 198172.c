xmlFreeAutomata(xmlAutomataPtr am) {
    if (am == NULL)
	return;
    xmlRegFreeParserCtxt(am);
}