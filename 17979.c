SetModifierMap(	XkbSrvInfoPtr	xkbi,
		xkbSetMapReq *	req,
		CARD8 *		wire,
		XkbChangesPtr	changes)
{
register unsigned	i,first,last;
XkbClientMapPtr		xkb = xkbi->desc->map;
CARD8 *			start;

    start= wire;
    first= req->firstModMapKey;
    last=  req->firstModMapKey+req->nModMapKeys-1;
    bzero(&xkb->modmap[first],req->nModMapKeys);
    for (i=0;i<req->totalModMapKeys;i++,wire+= 2) {
	xkb->modmap[wire[0]]= wire[1];
    }
    if (first>0) {
	if (changes->map.changed&XkbModifierMapMask) {
	    int oldLast;
	    oldLast= changes->map.first_modmap_key+
						changes->map.num_modmap_keys-1;
	    if (changes->map.first_modmap_key<first)
		first= changes->map.first_modmap_key;
	    if (oldLast>last)
		last= oldLast;
	}
	changes->map.first_modmap_key= first;
	changes->map.num_modmap_keys= (last-first)+1;
    }
    wire+= XkbPaddedSize(wire-start)-(wire-start);
    return (char *)wire;
}