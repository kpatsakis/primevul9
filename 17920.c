XkbWriteKeyTypes(	XkbDescPtr		xkb,
			xkbGetMapReply *	rep,
			char *			buf,
			ClientPtr 		client)
{
    XkbKeyTypePtr	type;
    unsigned		i;
    xkbKeyTypeWireDesc *wire;

    type= &xkb->map->types[rep->firstType];
    for (i=0;i<rep->nTypes;i++,type++) {
	register unsigned n;
	wire= (xkbKeyTypeWireDesc *)buf;
	wire->mask = type->mods.mask;
	wire->realMods = type->mods.real_mods;
	wire->virtualMods = type->mods.vmods;
	wire->numLevels = type->num_levels;
	wire->nMapEntries = type->map_count;
	wire->preserve = (type->preserve!=NULL);
	if (client->swapped) {
	    register int n;
	    swaps(&wire->virtualMods,n);
	}	

	buf= (char *)&wire[1];
	if (wire->nMapEntries>0) {
	    xkbKTMapEntryWireDesc *	wire;
	    XkbKTMapEntryPtr		entry;
	    wire= (xkbKTMapEntryWireDesc *)buf;
	    entry= type->map;
	    for (n=0;n<type->map_count;n++,wire++,entry++) {
		wire->active= entry->active;
		wire->mask= entry->mods.mask;
		wire->level= entry->level;
		wire->realMods= entry->mods.real_mods;
		wire->virtualMods= entry->mods.vmods;
		if (client->swapped) {
		    register int n;
		    swaps(&wire->virtualMods,n);
		}
	    }
	    buf= (char *)wire;
	    if (type->preserve!=NULL) {
		xkbModsWireDesc *	pwire;
		XkbModsPtr		preserve;
		pwire= (xkbModsWireDesc *)buf;
		preserve= type->preserve;
		for (n=0;n<type->map_count;n++,pwire++,preserve++) {
		    pwire->mask= preserve->mask;
		    pwire->realMods= preserve->real_mods;
		    pwire->virtualMods= preserve->vmods;
		    if (client->swapped) {
			register int n;
			swaps(&pwire->virtualMods,n);
		    }
		}
		buf= (char *)pwire;
	    }
	}
    }
    return buf;
}