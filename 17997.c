CheckKeyTypes(	ClientPtr	client,
		XkbDescPtr	xkb,
		xkbSetMapReq *	req,
		xkbKeyTypeWireDesc **wireRtrn,
		int	 *	nMapsRtrn,
		CARD8 *		mapWidthRtrn)
{
unsigned		nMaps;
register unsigned	i,n;
register CARD8 *	map;
register xkbKeyTypeWireDesc	*wire = *wireRtrn;

    if (req->firstType>((unsigned)xkb->map->num_types)) {
	*nMapsRtrn = _XkbErrCode3(0x01,req->firstType,xkb->map->num_types);
	return 0;
    }
    if (req->flags&XkbSetMapResizeTypes) {
	nMaps = req->firstType+req->nTypes;
	if (nMaps<XkbNumRequiredTypes) {  /* canonical types must be there */
	    *nMapsRtrn= _XkbErrCode4(0x02,req->firstType,req->nTypes,4);
	    return 0;
	}
    }
    else if (req->present&XkbKeyTypesMask) {
	nMaps = xkb->map->num_types;
	if ((req->firstType+req->nTypes)>nMaps) {
	    *nMapsRtrn = req->firstType+req->nTypes;
	    return 0;
	}
    }
    else {
	*nMapsRtrn = xkb->map->num_types;
	for (i=0;i<xkb->map->num_types;i++) {
	    mapWidthRtrn[i] = xkb->map->types[i].num_levels;
	}
	return 1;
    }

    for (i=0;i<req->firstType;i++) {
	mapWidthRtrn[i] = xkb->map->types[i].num_levels;
    }
    for (i=0;i<req->nTypes;i++) {
	unsigned	width;
	if (client->swapped) {
	    register int s;
	    swaps(&wire->virtualMods,s);
	}
	n= i+req->firstType;
	width= wire->numLevels;
	if (width<1) {
	    *nMapsRtrn= _XkbErrCode3(0x04,n,width);
	    return 0;
	}
	else if ((n==XkbOneLevelIndex)&&(width!=1)) { /* must be width 1 */
	    *nMapsRtrn= _XkbErrCode3(0x05,n,width);
	    return 0;
	}
	else if ((width!=2)&&
		 ((n==XkbTwoLevelIndex)||(n==XkbKeypadIndex)||
		  (n==XkbAlphabeticIndex))) {
	    /* TWO_LEVEL, ALPHABETIC and KEYPAD must be width 2 */
	    *nMapsRtrn= _XkbErrCode3(0x05,n,width);
	    return 0;
	}
	if (wire->nMapEntries>0) {
	    xkbKTSetMapEntryWireDesc *	mapWire;
	    xkbModsWireDesc *		preWire;
	    mapWire= (xkbKTSetMapEntryWireDesc *)&wire[1];
	    preWire= (xkbModsWireDesc *)&mapWire[wire->nMapEntries];
	    for (n=0;n<wire->nMapEntries;n++) {
		if (client->swapped) {
		    register int s;
		    swaps(&mapWire[n].virtualMods,s);
		}
		if (mapWire[n].realMods&(~wire->realMods)) {
		    *nMapsRtrn= _XkbErrCode4(0x06,n,mapWire[n].realMods,
						 wire->realMods);
		    return 0;
		}
		if (mapWire[n].virtualMods&(~wire->virtualMods)) {
		    *nMapsRtrn= _XkbErrCode3(0x07,n,mapWire[n].virtualMods);
		    return 0;
		}
		if (mapWire[n].level>=wire->numLevels) {
		    *nMapsRtrn= _XkbErrCode4(0x08,n,wire->numLevels,
						 mapWire[n].level);
		    return 0;
		}
		if (wire->preserve) {
		    if (client->swapped) {
			register int s;
			swaps(&preWire[n].virtualMods,s);
		    }
		    if (preWire[n].realMods&(~mapWire[n].realMods)) {
			*nMapsRtrn= _XkbErrCode4(0x09,n,preWire[n].realMods,
							mapWire[n].realMods);
			return 0;
		    }
		    if (preWire[n].virtualMods&(~mapWire[n].virtualMods)) {
			*nMapsRtrn=_XkbErrCode3(0x0a,n,preWire[n].virtualMods);
			return 0;
		    }
		}
	    }
	    if (wire->preserve)
		 map= (CARD8 *)&preWire[wire->nMapEntries];
	    else map= (CARD8 *)&mapWire[wire->nMapEntries];
	}
	else map= (CARD8 *)&wire[1];
	mapWidthRtrn[i+req->firstType] = wire->numLevels;
	wire= (xkbKeyTypeWireDesc *)map;
    }
    for (i=req->firstType+req->nTypes;i<nMaps;i++) {
	mapWidthRtrn[i] = xkb->map->types[i].num_levels;
    }
    *nMapsRtrn = nMaps;
    *wireRtrn = wire;
    return 1;
}