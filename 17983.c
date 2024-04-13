XkbSendCompatMap(	ClientPtr 		client,
			XkbCompatMapPtr 	compat,
			xkbGetCompatMapReply *	rep)
{
char	*	data;
int		size;

    size= rep->length*4;
    if (size>0) {
	data = (char *)xalloc(size);
	if (data) {
	    register unsigned i,bit;
	    xkbModsWireDesc *	grp;
	    XkbSymInterpretPtr	sym= &compat->sym_interpret[rep->firstSI];
	    xkbSymInterpretWireDesc *wire = (xkbSymInterpretWireDesc *)data;
	    for (i=0;i<rep->nSI;i++,sym++,wire++) {
		wire->sym= sym->sym;
		wire->mods= sym->mods;
		wire->match= sym->match;
		wire->virtualMod= sym->virtual_mod;
		wire->flags= sym->flags;
		memcpy((char*)&wire->act,(char*)&sym->act,sz_xkbActionWireDesc);
		if (client->swapped) {
		    register int n;
		    swapl(&wire->sym,n);
		}
	    }
	    if (rep->groups) {
		grp = (xkbModsWireDesc *)wire;
		for (i=0,bit=1;i<XkbNumKbdGroups;i++,bit<<=1) {
		    if (rep->groups&bit) {
			grp->mask= compat->groups[i].mask;
			grp->realMods= compat->groups[i].real_mods;
			grp->virtualMods= compat->groups[i].vmods;
			if (client->swapped) {
			    register int n;
			    swaps(&grp->virtualMods,n);
			}
			grp++;
		    }
		}
		wire= (xkbSymInterpretWireDesc*)grp;
	    }
	}
	else return BadAlloc;
    }
    else data= NULL;

    if (client->swapped) {
	register int n;
	swaps(&rep->sequenceNumber,n);
	swapl(&rep->length,n);
	swaps(&rep->firstSI,n);
	swaps(&rep->nSI,n);
	swaps(&rep->nTotalSI,n);
    }

    WriteToClient(client, SIZEOF(xkbGetCompatMapReply), (char *)rep);
    if (data) {
	WriteToClient(client, size, data);
	xfree((char *)data);
    }
    return client->noClientException;
}