XkbWriteGeomKeyAliases(char *wire,XkbGeometryPtr geom,Bool swap)
{
register int sz;
    
    sz= geom->num_key_aliases*(XkbKeyNameLength*2);
    if (sz>0) {
	memcpy(wire,(char *)geom->key_aliases,sz);
	wire+= sz;
    }
    return wire;
}