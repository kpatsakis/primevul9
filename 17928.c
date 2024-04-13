XkbSendNames(ClientPtr client,XkbDescPtr xkb,xkbGetNamesReply *rep)
{
register unsigned 	i,length,which;
char *			start;
char *			desc;
register int            n;

    length= rep->length*4;
    which= rep->which;
    if (client->swapped) {
	swaps(&rep->sequenceNumber,n);
	swapl(&rep->length,n);
	swapl(&rep->which,n);
	swaps(&rep->virtualMods,n);
	swapl(&rep->indicators,n);
    }

    start = desc = (char *)xalloc(length);
    if ( !start )
	return BadAlloc;
    if (xkb->names) {
        if (which&XkbKeycodesNameMask) {
            *((CARD32 *)desc)= xkb->names->keycodes;
            if (client->swapped) {
                swapl(desc,n);
            }
            desc+= 4;
        }
        if (which&XkbGeometryNameMask)  {
            *((CARD32 *)desc)= xkb->names->geometry;
            if (client->swapped) {
                swapl(desc,n);
            }
            desc+= 4;
        }
        if (which&XkbSymbolsNameMask) {
            *((CARD32 *)desc)= xkb->names->symbols;
            if (client->swapped) {
                swapl(desc,n);
            }
            desc+= 4;
        }
        if (which&XkbPhysSymbolsNameMask) {
            register CARD32 *atm= (CARD32 *)desc;
            atm[0]= (CARD32)xkb->names->phys_symbols;
            if (client->swapped) {
                swapl(&atm[0],n);
            }
            desc+= 4;
        }
        if (which&XkbTypesNameMask) {
            *((CARD32 *)desc)= (CARD32)xkb->names->types;
            if (client->swapped) {
                swapl(desc,n);
            }
            desc+= 4;
        }
        if (which&XkbCompatNameMask) {
            *((CARD32 *)desc)= (CARD32)xkb->names->compat;
            if (client->swapped) {
                swapl(desc,n);
            }
            desc+= 4;
        }
        if (which&XkbKeyTypeNamesMask) {
            register CARD32 *atm= (CARD32 *)desc;
            register XkbKeyTypePtr type= xkb->map->types;

            for (i=0;i<xkb->map->num_types;i++,atm++,type++) {
                *atm= (CARD32)type->name;
                if (client->swapped) {
                    swapl(atm,n);
                }
            }
            desc= (char *)atm;
        }
        if (which&XkbKTLevelNamesMask && xkb->map) {
            XkbKeyTypePtr type = xkb->map->types;
            register CARD32 *atm;
            for (i=0;i<rep->nTypes;i++,type++) {
                *desc++ = type->num_levels;
            }
            desc+= XkbPaddedSize(rep->nTypes)-rep->nTypes;

            atm= (CARD32 *)desc;
            type = xkb->map->types;
            for (i=0;i<xkb->map->num_types;i++,type++) {
                register unsigned l;
                if (type->level_names) {
                    for (l=0;l<type->num_levels;l++,atm++) {
                        *atm= type->level_names[l];
                        if (client->swapped) {
                            swapl(atm,n);
                        }
                    }
                    desc+= type->num_levels*4;
                }
            }
        }
        if (which&XkbIndicatorNamesMask) {
            desc= _XkbWriteAtoms(desc,xkb->names->indicators,XkbNumIndicators,
                                 client->swapped);
        }
        if (which&XkbVirtualModNamesMask) {
            desc= _XkbWriteAtoms(desc,xkb->names->vmods,XkbNumVirtualMods,
                                 client->swapped);
        }
        if (which&XkbGroupNamesMask) {
            desc= _XkbWriteAtoms(desc,xkb->names->groups,XkbNumKbdGroups,
                                 client->swapped);
        }
        if (which&XkbKeyNamesMask) {
            for (i=0;i<rep->nKeys;i++,desc+= sizeof(XkbKeyNameRec)) {
                *((XkbKeyNamePtr)desc)= xkb->names->keys[i+rep->firstKey];
            }
        }
        if (which&XkbKeyAliasesMask) {
            XkbKeyAliasPtr	pAl;
            pAl= xkb->names->key_aliases;
            for (i=0;i<rep->nKeyAliases;i++,pAl++,desc+=2*XkbKeyNameLength) {
                *((XkbKeyAliasPtr)desc)= *pAl;
            }
        }
        if ((which&XkbRGNamesMask)&&(rep->nRadioGroups>0)) {
            register CARD32	*atm= (CARD32 *)desc;
            for (i=0;i<rep->nRadioGroups;i++,atm++) {
                *atm= (CARD32)xkb->names->radio_groups[i];
                if (client->swapped) {
                    swapl(atm,n);
                }
            }
            desc+= rep->nRadioGroups*4;
        }
    }

    if ((desc-start)!=(length)) {
	ErrorF("[xkb] BOGUS LENGTH in write names, expected %d, got %ld\n",
					length, (unsigned long)(desc-start));
    }
    WriteToClient(client, SIZEOF(xkbGetNamesReply), (char *)rep);
    WriteToClient(client, length, start);
    xfree((char *)start);
    return client->noClientException;
}