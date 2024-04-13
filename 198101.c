xmlAutomataIsDeterminist(xmlAutomataPtr am) {
    int ret;

    if (am == NULL)
	return(-1);

    ret = xmlFAComputesDeterminism(am);
    return(ret);
}