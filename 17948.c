_XkbSetNames(ClientPtr client, DeviceIntPtr dev, xkbSetNamesReq *stuff)
{
    XkbDescRec		*xkb;
    XkbNamesRec		*names;
    CARD32		*tmp;
    xkbNamesNotify	 nn;

    tmp = (CARD32 *)&stuff[1];
    xkb = dev->key->xkbInfo->desc;
    names = xkb->names;

    if (XkbAllocNames(xkb,stuff->which,stuff->nRadioGroups,
                stuff->nKeyAliases)!=Success) {
        return BadAlloc;
    }

    bzero(&nn,sizeof(xkbNamesNotify));
    nn.changed= stuff->which;
    tmp = (CARD32 *)&stuff[1];
    if (stuff->which&XkbKeycodesNameMask)
        names->keycodes= *tmp++;
    if (stuff->which&XkbGeometryNameMask)
        names->geometry= *tmp++;
    if (stuff->which&XkbSymbolsNameMask)
        names->symbols= *tmp++;
    if (stuff->which&XkbPhysSymbolsNameMask)
        names->phys_symbols= *tmp++;
    if (stuff->which&XkbTypesNameMask)
        names->types= *tmp++;
    if (stuff->which&XkbCompatNameMask)
        names->compat= *tmp++;
    if ((stuff->which&XkbKeyTypeNamesMask)&&(stuff->nTypes>0)) {
        register unsigned i;
        register XkbKeyTypePtr type;

        type= &xkb->map->types[stuff->firstType];
        for (i=0;i<stuff->nTypes;i++,type++) {
            type->name= *tmp++;
        }
        nn.firstType= stuff->firstType;
        nn.nTypes= stuff->nTypes;
    }
    if (stuff->which&XkbKTLevelNamesMask) {
        register XkbKeyTypePtr	type;
        register unsigned i;
        CARD8 *width;

        width = (CARD8 *)tmp;
        tmp= (CARD32 *)(((char *)tmp)+XkbPaddedSize(stuff->nKTLevels));
        type= &xkb->map->types[stuff->firstKTLevel];
        for (i=0;i<stuff->nKTLevels;i++,type++) {
            if (width[i]>0) {
                if (type->level_names) {
                    register unsigned n;
                    for (n=0;n<width[i];n++) {
                        type->level_names[n]= tmp[n];
                    }
                }
                tmp+= width[i];
            }
        }
        nn.firstLevelName= 0;
        nn.nLevelNames= stuff->nTypes;
    }
    if (stuff->which&XkbIndicatorNamesMask) {
        tmp= _XkbCopyMaskedAtoms(tmp,names->indicators,XkbNumIndicators,
                stuff->indicators);
        nn.changedIndicators= stuff->indicators;
    }
    if (stuff->which&XkbVirtualModNamesMask) {
        tmp= _XkbCopyMaskedAtoms(tmp,names->vmods,XkbNumVirtualMods,
                stuff->virtualMods);
        nn.changedVirtualMods= stuff->virtualMods;
    }
    if (stuff->which&XkbGroupNamesMask) {
        tmp= _XkbCopyMaskedAtoms(tmp,names->groups,XkbNumKbdGroups,
                stuff->groupNames);
        nn.changedVirtualMods= stuff->groupNames;
    }
    if (stuff->which&XkbKeyNamesMask) {
        memcpy((char*)&names->keys[stuff->firstKey],(char *)tmp,
                stuff->nKeys*XkbKeyNameLength);
        tmp+= stuff->nKeys;
        nn.firstKey= stuff->firstKey;
        nn.nKeys= stuff->nKeys;
    }
    if (stuff->which&XkbKeyAliasesMask) {
        if (stuff->nKeyAliases>0) {
            register int na= stuff->nKeyAliases;	
            if (XkbAllocNames(xkb,XkbKeyAliasesMask,0,na)!=Success)
                return BadAlloc;
            memcpy((char *)names->key_aliases,(char *)tmp,
                    stuff->nKeyAliases*sizeof(XkbKeyAliasRec));
            tmp+= stuff->nKeyAliases*2;
        }
        else if (names->key_aliases!=NULL) {
            _XkbFree(names->key_aliases);
            names->key_aliases= NULL;
            names->num_key_aliases= 0;
        }
        nn.nAliases= names->num_key_aliases;
    }
    if (stuff->which&XkbRGNamesMask) {
        if (stuff->nRadioGroups>0) {
            register unsigned i,nrg;
            nrg= stuff->nRadioGroups;
            if (XkbAllocNames(xkb,XkbRGNamesMask,nrg,0)!=Success)
                return BadAlloc;

            for (i=0;i<stuff->nRadioGroups;i++) {
                names->radio_groups[i]= tmp[i];
            }
            tmp+= stuff->nRadioGroups;
        }
        else if (names->radio_groups) {
            _XkbFree(names->radio_groups);
            names->radio_groups= NULL;
            names->num_rg= 0;
        }
        nn.nRadioGroups= names->num_rg;
    }
    if (nn.changed) {
        Bool needExtEvent;
        needExtEvent= (nn.changed&XkbIndicatorNamesMask)!=0;
        XkbSendNamesNotify(dev,&nn);
        if (needExtEvent) {
            XkbSrvLedInfoPtr		sli;
            xkbExtensionDeviceNotify	edev;
            register int		i;
            register unsigned		bit;

            sli= XkbFindSrvLedInfo(dev,XkbDfltXIClass,XkbDfltXIId,
                    XkbXI_IndicatorsMask);
            sli->namesPresent= 0;
            for (i=0,bit=1;i<XkbNumIndicators;i++,bit<<=1) {
                if (names->indicators[i]!=None)
                    sli->namesPresent|= bit;
            }
            bzero(&edev,sizeof(xkbExtensionDeviceNotify));
            edev.reason=	XkbXI_IndicatorNamesMask;
            edev.ledClass=	KbdFeedbackClass;
            edev.ledID=		dev->kbdfeed->ctrl.id;
            edev.ledsDefined= 	sli->namesPresent|sli->mapsPresent;
            edev.ledState=	sli->effectiveState;
            edev.firstBtn=	0;
            edev.nBtns=		0;
            edev.supported=	XkbXI_AllFeaturesMask;
            edev.unsupported=	0;
            XkbSendExtensionDeviceNotify(dev,client,&edev);
        }
    }
    return Success;
}