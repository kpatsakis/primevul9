XkbWriteGeomProperties(char *wire,XkbGeometryPtr geom,Bool swap)
{
register int 	i;
register XkbPropertyPtr	prop;
    
    for (i=0,prop=geom->properties;i<geom->num_properties;i++,prop++) {
	wire= XkbWriteCountedString(wire,prop->name,swap);
	wire= XkbWriteCountedString(wire,prop->value,swap);
    }
    return wire;
}