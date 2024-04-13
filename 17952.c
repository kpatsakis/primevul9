XkbSizeKeySyms(XkbDescPtr xkb,xkbGetMapReply *rep)
{
    XkbSymMapPtr	symMap;
    unsigned		i,len;
    unsigned		nSyms,nSymsThisKey;

    if (((rep->present&XkbKeySymsMask)==0)||(rep->nKeySyms<1)||
	(!xkb)||(!xkb->map)||(!xkb->map->key_sym_map)) {
	rep->present&= ~XkbKeySymsMask;
	rep->firstKeySym= rep->nKeySyms= 0;
	rep->totalSyms= 0;
	return 0;
    }
    len= rep->nKeySyms*SIZEOF(xkbSymMapWireDesc);
    symMap = &xkb->map->key_sym_map[rep->firstKeySym];
    for (i=nSyms=0;i<rep->nKeySyms;i++,symMap++) {
	if (symMap->offset!=0) {
	    nSymsThisKey= XkbNumGroups(symMap->group_info)*symMap->width;
	    nSyms+= nSymsThisKey;
	}
    }
    len+= nSyms*4;
    rep->totalSyms= nSyms;
    return len;
}