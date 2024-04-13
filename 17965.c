XkbWriteExplicit(XkbDescPtr xkb,xkbGetMapReply *rep,char *buf,ClientPtr client)
{
unsigned	i;
char *		start;
unsigned char *	pExp;

    start= buf;
    pExp= &xkb->server->explicit[rep->firstKeyExplicit];
    for (i=0;i<rep->nKeyExplicit;i++,pExp++) {
	if (*pExp!=0) {
	    *buf++= i+rep->firstKeyExplicit;
	    *buf++= *pExp;
	}
    }
    i= XkbPaddedSize(buf-start)-(buf-start); /* pad to word boundary */
    return buf+i;
}