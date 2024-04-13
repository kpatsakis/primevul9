XkbWriteModifierMap(XkbDescPtr xkb,xkbGetMapReply *rep,char *buf,
							ClientPtr client)
{
unsigned	i;
char *		start;
unsigned char *	pMap;

    start= buf;
    pMap= &xkb->map->modmap[rep->firstModMapKey];
    for (i=0;i<rep->nModMapKeys;i++,pMap++) {
	if (*pMap!=0) {
	    *buf++= i+rep->firstModMapKey;
	    *buf++= *pMap;
	}
    }
    i= XkbPaddedSize(buf-start)-(buf-start); /* pad to word boundary */
    return buf+i;
}