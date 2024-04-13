XkbWriteKeySyms(XkbDescPtr xkb,xkbGetMapReply *rep,char *buf,ClientPtr client)
{
register KeySym *	pSym;
XkbSymMapPtr		symMap;
xkbSymMapWireDesc *	outMap;
register unsigned	i;

    symMap = &xkb->map->key_sym_map[rep->firstKeySym];
    for (i=0;i<rep->nKeySyms;i++,symMap++) {
	outMap = (xkbSymMapWireDesc *)buf;
	outMap->ktIndex[0] = symMap->kt_index[0];
	outMap->ktIndex[1] = symMap->kt_index[1];
	outMap->ktIndex[2] = symMap->kt_index[2];
	outMap->ktIndex[3] = symMap->kt_index[3];
	outMap->groupInfo = symMap->group_info;
	outMap->width= symMap->width;
	outMap->nSyms = symMap->width*XkbNumGroups(symMap->group_info);
	buf= (char *)&outMap[1];
	if (outMap->nSyms==0)
	    continue;

	pSym = &xkb->map->syms[symMap->offset];
	memcpy((char *)buf,(char *)pSym,outMap->nSyms*4);
	if (client->swapped) {
	    register int n,nSyms= outMap->nSyms;
	    swaps(&outMap->nSyms,n);
	    while (nSyms-->0) {
		swapl(buf,n);
		buf+= 4;
	    }
	}
	else buf+= outMap->nSyms*4;
    }
    return buf;
}