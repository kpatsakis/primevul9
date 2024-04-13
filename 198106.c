xmlExpCtxtNbCons(xmlExpCtxtPtr ctxt) {
    if (ctxt == NULL)
        return(-1);
    return(ctxt->nb_cons);
}