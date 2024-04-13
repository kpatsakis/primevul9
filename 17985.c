XkbSendIndicatorMap(	ClientPtr			client,
			XkbIndicatorPtr			indicators,
			xkbGetIndicatorMapReply *	rep)
{
int 			length;
CARD8 *			map;
register int		i;
register unsigned	bit;

    length = rep->length*4;
    if (length>0) {
	CARD8 *to;
	to= map= (CARD8 *)xalloc(length);
	if (map) {
	    xkbIndicatorMapWireDesc  *wire = (xkbIndicatorMapWireDesc *)to;
	    for (i=0,bit=1;i<XkbNumIndicators;i++,bit<<=1) {
		if (rep->which&bit) {
		    wire->flags= indicators->maps[i].flags;
		    wire->whichGroups= indicators->maps[i].which_groups;
		    wire->groups= indicators->maps[i].groups;
		    wire->whichMods= indicators->maps[i].which_mods;
		    wire->mods= indicators->maps[i].mods.mask;
		    wire->realMods= indicators->maps[i].mods.real_mods;
		    wire->virtualMods= indicators->maps[i].mods.vmods;
		    wire->ctrls= indicators->maps[i].ctrls;
		    if (client->swapped) {
			register int n;
			swaps(&wire->virtualMods,n);
			swapl(&wire->ctrls,n);
		    }
		    wire++;
		}
	    }
	    to = (CARD8 *)wire;
	    if ((to-map)!=length) {
		client->errorValue = _XkbErrCode2(0xff,length);
		return BadLength;
	    }
	}
	else return BadAlloc;
    }
    else map = NULL;
    if (client->swapped) {
	swaps(&rep->sequenceNumber,i);
	swapl(&rep->length,i);
	swapl(&rep->which,i);
	swapl(&rep->realIndicators,i);
    }
    WriteToClient(client, SIZEOF(xkbGetIndicatorMapReply), (char *)rep);
    if (map) {
	WriteToClient(client, length, (char *)map);
	xfree((char *)map);
    }
    return client->noClientException;
}