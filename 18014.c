XkbComputeGetCompatMapReplySize(	XkbCompatMapPtr 	compat,
					xkbGetCompatMapReply *	rep)
{
unsigned	 size,nGroups;

    nGroups= 0;
    if (rep->groups!=0) {
	register int i,bit;
	for (i=0,bit=1;i<XkbNumKbdGroups;i++,bit<<=1) {
	    if (rep->groups&bit)
		nGroups++;
	}
    }
    size= nGroups*SIZEOF(xkbModsWireDesc);
    size+= (rep->nSI*SIZEOF(xkbSymInterpretWireDesc));
    rep->length= size/4;
    return Success;
}