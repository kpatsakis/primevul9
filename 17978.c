XkbSendMap(ClientPtr client,XkbDescPtr xkb,xkbGetMapReply *rep)
{
unsigned	i,len;
char		*desc,*start;

    len= (rep->length*4)-(SIZEOF(xkbGetMapReply)-SIZEOF(xGenericReply));
    start= desc= (char *)xalloc(len);
    if (!start)
	return BadAlloc;
    if ( rep->nTypes>0 )
	desc = XkbWriteKeyTypes(xkb,rep,desc,client);
    if ( rep->nKeySyms>0 )
	desc = XkbWriteKeySyms(xkb,rep,desc,client);
    if ( rep->nKeyActs>0 )
	desc = XkbWriteKeyActions(xkb,rep,desc,client);
    if ( rep->totalKeyBehaviors>0 )
	desc = XkbWriteKeyBehaviors(xkb,rep,desc,client);
    if ( rep->virtualMods ) {
	register int sz,bit;
	for (i=sz=0,bit=1;i<XkbNumVirtualMods;i++,bit<<=1) {
	    if (rep->virtualMods&bit) {
		desc[sz++]= xkb->server->vmods[i];
	    }
	}
	desc+= XkbPaddedSize(sz);
    }
    if ( rep->totalKeyExplicit>0 )
	desc= XkbWriteExplicit(xkb,rep,desc,client);
    if ( rep->totalModMapKeys>0 )
	desc= XkbWriteModifierMap(xkb,rep,desc,client);
    if ( rep->totalVModMapKeys>0 )
	desc= XkbWriteVirtualModMap(xkb,rep,desc,client);
    if ((desc-start)!=(len)) {
	ErrorF("[xkb] BOGUS LENGTH in write keyboard desc, expected %d, got %ld\n",
					len, (unsigned long)(desc-start));
    }
    if (client->swapped) {
	register int n;
	swaps(&rep->sequenceNumber,n);
	swapl(&rep->length,n);
	swaps(&rep->present,n);
	swaps(&rep->totalSyms,n);
	swaps(&rep->totalActs,n);
    }
    WriteToClient(client, (i=SIZEOF(xkbGetMapReply)), (char *)rep);
    WriteToClient(client, len, start);
    xfree((char *)start);
    return client->noClientException;
}