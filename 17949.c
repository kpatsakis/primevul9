XkbWriteGeomColors(char *wire,XkbGeometryPtr geom,Bool swap)
{
register int		i;
register XkbColorPtr	color;

    for (i=0,color=geom->colors;i<geom->num_colors;i++,color++) {
	wire= XkbWriteCountedString(wire,color->spec,swap);
    }
    return wire;
}