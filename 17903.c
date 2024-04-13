SetVirtualMods(XkbSrvInfoPtr xkbi,xkbSetMapReq *req,CARD8 *wire,
						XkbChangesPtr changes)
{
register int 		i,bit,nMods;
XkbServerMapPtr		srv = xkbi->desc->server;

    if (((req->present&XkbVirtualModsMask)==0)||(req->virtualMods==0))
	return (char *)wire;
    for (i=nMods=0,bit=1;i<XkbNumVirtualMods;i++,bit<<=1) {
	if (req->virtualMods&bit) {
	    if (srv->vmods[i]!=wire[nMods]) {
		changes->map.changed|= XkbVirtualModsMask;
		changes->map.vmods|= bit;
		srv->vmods[i]= wire[nMods];
	    }
	    nMods++;
	}
    }
    return (char *)(wire+XkbPaddedSize(nMods));
}