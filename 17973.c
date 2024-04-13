XkbComputeGetNamesReplySize(XkbDescPtr xkb,xkbGetNamesReply *rep)
{
register unsigned	which,length;
register int		i;

    rep->minKeyCode= xkb->min_key_code;
    rep->maxKeyCode= xkb->max_key_code;
    which= rep->which;
    length= 0;
    if (xkb->names!=NULL) {
	 if (which&XkbKeycodesNameMask)		length++;
	 if (which&XkbGeometryNameMask)		length++;
	 if (which&XkbSymbolsNameMask)		length++;
	 if (which&XkbPhysSymbolsNameMask)	length++;
	 if (which&XkbTypesNameMask)		length++;
	 if (which&XkbCompatNameMask)		length++;
    }
    else which&= ~XkbComponentNamesMask;

    if (xkb->map!=NULL) {
	if (which&XkbKeyTypeNamesMask)
	    length+= xkb->map->num_types;
	rep->nTypes= xkb->map->num_types;
	if (which&XkbKTLevelNamesMask) {
	    XkbKeyTypePtr	pType = xkb->map->types;
	    int			nKTLevels = 0;

	    length+= XkbPaddedSize(xkb->map->num_types)/4;
	    for (i=0;i<xkb->map->num_types;i++,pType++) {
		if (pType->level_names!=NULL)
		    nKTLevels+= pType->num_levels;
	    }
	    rep->nKTLevels= nKTLevels;
	    length+= nKTLevels;
	}
    }
    else {
	rep->nTypes=    0;
	rep->nKTLevels= 0;
	which&= ~(XkbKeyTypeNamesMask|XkbKTLevelNamesMask);
    }

    rep->minKeyCode= xkb->min_key_code;
    rep->maxKeyCode= xkb->max_key_code;
    rep->indicators= 0;
    rep->virtualMods= 0;
    rep->groupNames= 0;
    if (xkb->names!=NULL) {
	if (which&XkbIndicatorNamesMask) {
	    int nLeds;
	    rep->indicators= 
		_XkbCountAtoms(xkb->names->indicators,XkbNumIndicators,&nLeds);
	    length+= nLeds;
	    if (nLeds==0)
		which&= ~XkbIndicatorNamesMask;
	}

	if (which&XkbVirtualModNamesMask) {
	    int nVMods;
	    rep->virtualMods= 
		_XkbCountAtoms(xkb->names->vmods,XkbNumVirtualMods,&nVMods);
	    length+= nVMods;
	    if (nVMods==0)
		which&= ~XkbVirtualModNamesMask;
	}

	if (which&XkbGroupNamesMask) {
	    int nGroups;
	    rep->groupNames=
		_XkbCountAtoms(xkb->names->groups,XkbNumKbdGroups,&nGroups);
	    length+= nGroups;
	    if (nGroups==0)
		which&= ~XkbGroupNamesMask;
	}

	if ((which&XkbKeyNamesMask)&&(xkb->names->keys))
	     length+= rep->nKeys;
	else which&= ~XkbKeyNamesMask;

	if ((which&XkbKeyAliasesMask)&&
	    (xkb->names->key_aliases)&&(xkb->names->num_key_aliases>0)) {
	    rep->nKeyAliases= xkb->names->num_key_aliases;
	    length+= rep->nKeyAliases*2;
	} 
	else {
	    which&= ~XkbKeyAliasesMask;
	    rep->nKeyAliases= 0;
	}

	if ((which&XkbRGNamesMask)&&(xkb->names->num_rg>0))
	     length+= xkb->names->num_rg;
	else which&= ~XkbRGNamesMask;
    }
    else {
	which&= ~(XkbIndicatorNamesMask|XkbVirtualModNamesMask);
	which&= ~(XkbGroupNamesMask|XkbKeyNamesMask|XkbKeyAliasesMask);
	which&= ~XkbRGNamesMask;
    }

    rep->length= length;
    rep->which= which;
    return Success;
}