XkbWriteVirtualModMap(XkbDescPtr xkb,xkbGetMapReply *rep,char *buf,
							ClientPtr client)
{
unsigned		i;
xkbVModMapWireDesc *	wire;
unsigned short *	pMap;

    wire= (xkbVModMapWireDesc *)buf;
    pMap= &xkb->server->vmodmap[rep->firstVModMapKey];
    for (i=0;i<rep->nVModMapKeys-1;i++,pMap++) {
	if (*pMap!=0) {
	    wire->key= i+rep->firstVModMapKey;
	    wire->vmods= *pMap;
	    wire++;
	}
    }
    return (char *)wire;
}