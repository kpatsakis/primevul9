XkbSizeGeomColors(XkbGeometryPtr geom)
{
register int 		i,size;
register XkbColorPtr	color;

    for (i=size=0,color=geom->colors;i<geom->num_colors;i++,color++) {
	size+= XkbSizeCountedString(color->spec);
    }
    return size;
}