xmlAutomataCompile(xmlAutomataPtr am) {
    xmlRegexpPtr ret;

    if ((am == NULL) || (am->error != 0)) return(NULL);
    xmlFAEliminateEpsilonTransitions(am);
    /* xmlFAComputesDeterminism(am); */
    ret = xmlRegEpxFromParse(am);

    return(ret);
}