xmlRegExecErrInfo(xmlRegExecCtxtPtr exec, const xmlChar **string,
                  int *nbval, int *nbneg, xmlChar **values, int *terminal) {
    if (exec == NULL)
        return(-1);
    if (string != NULL) {
        if (exec->status != 0)
	    *string = exec->errString;
	else
	    *string = NULL;
    }
    return(xmlRegExecGetValues(exec, 1, nbval, nbneg, values, terminal));
}