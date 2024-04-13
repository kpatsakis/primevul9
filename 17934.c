XkbSizeGeomShapes(XkbGeometryPtr geom)
{
register int		i,size;
register XkbShapePtr	shape;

    for (i=size=0,shape=geom->shapes;i<geom->num_shapes;i++,shape++) {
	register int		n;
	register XkbOutlinePtr	ol;
	size+= SIZEOF(xkbShapeWireDesc);
	for (n=0,ol=shape->outlines;n<shape->num_outlines;n++,ol++) {
	    size+= SIZEOF(xkbOutlineWireDesc);
	    size+= ol->num_points*SIZEOF(xkbPointWireDesc);
	}
    }
    return size;
}