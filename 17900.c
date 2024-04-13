SetKeyExplicit(XkbSrvInfoPtr xkbi,xkbSetMapReq *req,CARD8 *wire,
							XkbChangesPtr changes)
{
register unsigned	i,first,last;
XkbServerMapPtr		xkb = xkbi->desc->server;
CARD8 *			start;

    start= wire;
    first= req->firstKeyExplicit;
    last=  req->firstKeyExplicit+req->nKeyExplicit-1;
    bzero(&xkb->explicit[first],req->nKeyExplicit);
    for (i=0;i<req->totalKeyExplicit;i++,wire+= 2) {
	xkb->explicit[wire[0]]= wire[1];
    }
    if (first>0) {
	if (changes->map.changed&XkbExplicitComponentsMask) {
	    int oldLast;
	    oldLast= changes->map.first_key_explicit+
					changes->map.num_key_explicit-1;
	    if (changes->map.first_key_explicit<first)
		first= changes->map.first_key_explicit;
	    if (oldLast>last)
		last= oldLast;
	}
	changes->map.first_key_explicit= first;
	changes->map.num_key_explicit= (last-first)+1;
    }
    wire+= XkbPaddedSize(wire-start)-(wire-start);
    return (char *)wire;
}