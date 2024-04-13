SendDeviceLedInfo(	XkbSrvLedInfoPtr	sli,
			ClientPtr		client)
{
xkbDeviceLedsWireDesc	wire;
int			length;

    length= 0;
    wire.ledClass= 		sli->class;
    wire.ledID= 		sli->id;
    wire.namesPresent= 		sli->namesPresent;
    wire.mapsPresent=   	sli->mapsPresent;
    wire.physIndicators= 	sli->physIndicators;
    wire.state=			sli->effectiveState;
    if (client->swapped) {
	register int n;
	swaps(&wire.ledClass,n);
	swaps(&wire.ledID,n);
	swapl(&wire.namesPresent,n);
	swapl(&wire.mapsPresent,n);
	swapl(&wire.physIndicators,n);
	swapl(&wire.state,n);
    }
    WriteToClient(client,SIZEOF(xkbDeviceLedsWireDesc),(char *)&wire);
    length+= SIZEOF(xkbDeviceLedsWireDesc);
    if (sli->namesPresent|sli->mapsPresent) {
	register unsigned i,bit;
	if (sli->namesPresent) {
	    CARD32	awire;
	    for (i=0,bit=1;i<XkbNumIndicators;i++,bit<<=1) {
		if (sli->namesPresent&bit) {
		    awire= (CARD32)sli->names[i];
		    if (client->swapped) {
			register int n;
			swapl(&awire,n);
		    }
		    WriteToClient(client,4,(char *)&awire);
		    length+= 4;
		}
	    }
	}
	if (sli->mapsPresent) {
	    for (i=0,bit=1;i<XkbNumIndicators;i++,bit<<=1) {
		xkbIndicatorMapWireDesc	iwire;
		if (sli->mapsPresent&bit) {
		    iwire.flags= 	sli->maps[i].flags;
		    iwire.whichGroups=	sli->maps[i].which_groups;
		    iwire.groups=	sli->maps[i].groups;
		    iwire.whichMods=	sli->maps[i].which_mods;
		    iwire.mods=		sli->maps[i].mods.mask;
		    iwire.realMods=	sli->maps[i].mods.real_mods;
		    iwire.virtualMods=	sli->maps[i].mods.vmods;
		    iwire.ctrls= 	sli->maps[i].ctrls;
		    if (client->swapped) {
			register int n;
			swaps(&iwire.virtualMods,n);
			swapl(&iwire.ctrls,n);
		    }
		    WriteToClient(client,SIZEOF(xkbIndicatorMapWireDesc),
								(char *)&iwire);
		    length+= SIZEOF(xkbIndicatorMapWireDesc);
		}
	    }
	}
    }
    return length;
}