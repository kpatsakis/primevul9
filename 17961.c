SetVirtualModMap(	XkbSrvInfoPtr		xkbi,
			xkbSetMapReq *		req,
			xkbVModMapWireDesc *	wire,
			XkbChangesPtr 		changes)
{
register unsigned	i,first,last;
XkbServerMapPtr		srv = xkbi->desc->server;

    first= req->firstVModMapKey;
    last=  req->firstVModMapKey+req->nVModMapKeys-1;
    bzero(&srv->vmodmap[first],req->nVModMapKeys*sizeof(unsigned short));
    for (i=0;i<req->totalVModMapKeys;i++,wire++) {
	srv->vmodmap[wire->key]= wire->vmods;
    }
    if (first>0) {
	if (changes->map.changed&XkbVirtualModMapMask) {
	    int oldLast;
	    oldLast= changes->map.first_vmodmap_key+
					changes->map.num_vmodmap_keys-1;
	    if (changes->map.first_vmodmap_key<first)
		first= changes->map.first_vmodmap_key;
	    if (oldLast>last)
		last= oldLast;
	}
	changes->map.first_vmodmap_key= first;
	changes->map.num_vmodmap_keys= (last-first)+1;
    }
    return (char *)wire;
}