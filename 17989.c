CheckVirtualMods(	XkbDescRec *	xkb,
			xkbSetMapReq *	req,
			CARD8 **	wireRtrn,
			int *		errorRtrn)
{
register CARD8		*wire = *wireRtrn;
register unsigned 	 i,nMods,bit;

    if (((req->present&XkbVirtualModsMask)==0)||(req->virtualMods==0))
	return 1;
    for (i=nMods=0,bit=1;i<XkbNumVirtualMods;i++,bit<<=1) {
	if (req->virtualMods&bit)
	    nMods++;
    }
    *wireRtrn= (wire+XkbPaddedSize(nMods));
    return 1;
}