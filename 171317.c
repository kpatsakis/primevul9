xsltCompilerVarInfoFree(xsltCompilerCtxtPtr cctxt)
{
    xsltVarInfoPtr ivar = cctxt->ivars, ivartmp;    

    while (ivar) {
	ivartmp = ivar;
	ivar = ivar->next;
	xmlFree(ivartmp);
    }
}