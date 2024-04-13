CheckKeyActions(	XkbDescPtr	xkb,
			xkbSetMapReq *	req,
			int		nTypes,
			CARD8 *		mapWidths,
			CARD16 *	symsPerKey,
			CARD8 **	wireRtrn,
			int *		nActsRtrn)
{
int			 nActs;
CARD8 *			 wire = *wireRtrn;
register unsigned	 i;

    if (!(XkbKeyActionsMask&req->present))
	return 1;
    CHK_REQ_KEY_RANGE2(0x21,req->firstKeyAct,req->nKeyActs,req,(*nActsRtrn),0);
    for (nActs=i=0;i<req->nKeyActs;i++) {
	if (wire[0]!=0) {
	    if (wire[0]==symsPerKey[i+req->firstKeyAct])
		nActs+= wire[0];
	    else {
		*nActsRtrn= _XkbErrCode3(0x23,i+req->firstKeyAct,wire[0]);
		return 0;
	    }
	}
	wire++;
    }
    if (req->nKeyActs%4)
	wire+= 4-(req->nKeyActs%4);
    *wireRtrn = (CARD8 *)(((XkbAnyAction *)wire)+nActs);
    *nActsRtrn = nActs;
    return 1;
}