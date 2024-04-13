CheckKeySyms(	ClientPtr		client,
		XkbDescPtr		xkb,
		xkbSetMapReq *		req,
		int			nTypes,
		CARD8 *	 		mapWidths,
		CARD16 *	 	symsPerKey,
		xkbSymMapWireDesc **	wireRtrn,
		int *			errorRtrn)
{
register unsigned	i;
XkbSymMapPtr		map;
xkbSymMapWireDesc*	wire = *wireRtrn;

    if (!(XkbKeySymsMask&req->present))
	return 1;
    CHK_REQ_KEY_RANGE2(0x11,req->firstKeySym,req->nKeySyms,req,(*errorRtrn),0);
    map = &xkb->map->key_sym_map[xkb->min_key_code];
    for (i=xkb->min_key_code;i<(unsigned)req->firstKeySym;i++,map++) {
	register int g,ng,w;
	ng= XkbNumGroups(map->group_info);
	for (w=g=0;g<ng;g++) {
	    if (map->kt_index[g]>=(unsigned)nTypes) {
		*errorRtrn = _XkbErrCode4(0x13,i,g,map->kt_index[g]);
		return 0;
	    }
	    if (mapWidths[map->kt_index[g]]>w)
		w= mapWidths[map->kt_index[g]];
	}
	symsPerKey[i] = w*ng;
    }
    for (i=0;i<req->nKeySyms;i++) {
	KeySym *pSyms;
	register unsigned nG;
	if (client->swapped) {
	    swaps(&wire->nSyms,nG);
	}
	nG = XkbNumGroups(wire->groupInfo);
	if (nG>XkbNumKbdGroups) {
	    *errorRtrn = _XkbErrCode3(0x14,i+req->firstKeySym,nG);
	    return 0;
	}
	if (nG>0) {
	    register int g,w;
	    for (g=w=0;g<nG;g++) {
		if (wire->ktIndex[g]>=(unsigned)nTypes) {
		    *errorRtrn= _XkbErrCode4(0x15,i+req->firstKeySym,g,
		    					   wire->ktIndex[g]);
		    return 0;
		}
		if (mapWidths[wire->ktIndex[g]]>w)
		    w= mapWidths[wire->ktIndex[g]];
	    }
	    if (wire->width!=w) {
		*errorRtrn= _XkbErrCode3(0x16,i+req->firstKeySym,wire->width);
		return 0;
	    }
	    w*= nG;
	    symsPerKey[i+req->firstKeySym] = w;
	    if (w!=wire->nSyms) {
		*errorRtrn=_XkbErrCode4(0x16,i+req->firstKeySym,wire->nSyms,w);
		return 0;
	    }
	}
	else if (wire->nSyms!=0) {
	    *errorRtrn = _XkbErrCode3(0x17,i+req->firstKeySym,wire->nSyms);
	    return 0;
	}
	pSyms = (KeySym *)&wire[1];
	wire = (xkbSymMapWireDesc *)&pSyms[wire->nSyms];
    }

    map = &xkb->map->key_sym_map[i];
    for (;i<=(unsigned)xkb->max_key_code;i++,map++) {
	register int g,nG,w;
	nG= XkbKeyNumGroups(xkb,i);
	for (w=g=0;g<nG;g++)  {
	    if (map->kt_index[g]>=(unsigned)nTypes) {
		*errorRtrn = _XkbErrCode4(0x18,i,g,map->kt_index[g]);
		return 0;
	    }
	    if (mapWidths[map->kt_index[g]]>w)
		    w= mapWidths[map->kt_index[g]];
	}
	symsPerKey[i] = w*nG;
    }
    *wireRtrn = wire;
    return 1;
}