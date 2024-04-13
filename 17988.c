SetKeyTypes(	XkbDescPtr		xkb,
		xkbSetMapReq *		req,
		xkbKeyTypeWireDesc *	wire,
		XkbChangesPtr		changes)
{
register unsigned	i;
unsigned		first,last;
CARD8			*map;

    if ((unsigned)(req->firstType+req->nTypes)>xkb->map->size_types) {
	i= req->firstType+req->nTypes;
	if (XkbAllocClientMap(xkb,XkbKeyTypesMask,i)!=Success) {
	    return NULL;
	}
    }
    if ((unsigned)(req->firstType+req->nTypes)>xkb->map->num_types)
	xkb->map->num_types= req->firstType+req->nTypes;

    for (i=0;i<req->nTypes;i++) {
	XkbKeyTypePtr		pOld;
	register unsigned 	n;

	if (XkbResizeKeyType(xkb,i+req->firstType,wire->nMapEntries,
				wire->preserve,wire->numLevels)!=Success) {
	    return NULL;
	}
	pOld = &xkb->map->types[i+req->firstType];
	map = (CARD8 *)&wire[1];

	pOld->mods.real_mods = wire->realMods;
	pOld->mods.vmods= wire->virtualMods;
	pOld->num_levels = wire->numLevels;
	pOld->map_count= wire->nMapEntries;

	pOld->mods.mask= pOld->mods.real_mods|
					XkbMaskForVMask(xkb,pOld->mods.vmods);

	if (wire->nMapEntries) {
	    xkbKTSetMapEntryWireDesc *mapWire;
	    xkbModsWireDesc *preWire;
	    unsigned tmp;
	    mapWire= (xkbKTSetMapEntryWireDesc *)map;
	    preWire= (xkbModsWireDesc *)&mapWire[wire->nMapEntries];
	    for (n=0;n<wire->nMapEntries;n++) {
		pOld->map[n].active= 1;
		pOld->map[n].mods.mask= mapWire[n].realMods;
		pOld->map[n].mods.real_mods= mapWire[n].realMods;
		pOld->map[n].mods.vmods= mapWire[n].virtualMods;
		pOld->map[n].level= mapWire[n].level;
		if (mapWire[n].virtualMods!=0) {
		    tmp= XkbMaskForVMask(xkb,mapWire[n].virtualMods);
		    pOld->map[n].active= (tmp!=0);
		    pOld->map[n].mods.mask|= tmp;
		}
		if (wire->preserve) {
		    pOld->preserve[n].real_mods= preWire[n].realMods;
		    pOld->preserve[n].vmods= preWire[n].virtualMods;
		    tmp= XkbMaskForVMask(xkb,preWire[n].virtualMods);
		    pOld->preserve[n].mask= preWire[n].realMods|tmp;
		}
	    }
	    if (wire->preserve)
		 map= (CARD8 *)&preWire[wire->nMapEntries];
	    else map= (CARD8 *)&mapWire[wire->nMapEntries];
	}
	else map= (CARD8 *)&wire[1];
	wire = (xkbKeyTypeWireDesc *)map;
    }
    first= req->firstType;
    last= first+req->nTypes-1; /* last changed type */
    if (changes->map.changed&XkbKeyTypesMask) {
	int oldLast;
	oldLast= changes->map.first_type+changes->map.num_types-1;
	if (changes->map.first_type<first)
	    first= changes->map.first_type;
	if (oldLast>last)
	    last= oldLast;
    }
    changes->map.changed|= XkbKeyTypesMask;
    changes->map.first_type = first;
    changes->map.num_types = (last-first)+1;
    return (char *)wire;
}