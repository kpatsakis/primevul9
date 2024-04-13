XkbWriteGeomShapes(char *wire,XkbGeometryPtr geom,Bool swap)
{
int			i;
XkbShapePtr		shape;
xkbShapeWireDesc *	shapeWire;

    for (i=0,shape=geom->shapes;i<geom->num_shapes;i++,shape++) {
	register int 		o;
	XkbOutlinePtr		ol;
	xkbOutlineWireDesc *	olWire;
	shapeWire= (xkbShapeWireDesc *)wire;
	shapeWire->name= shape->name;
	shapeWire->nOutlines= shape->num_outlines;
	if (shape->primary!=NULL)
	     shapeWire->primaryNdx= XkbOutlineIndex(shape,shape->primary);
	else shapeWire->primaryNdx= XkbNoShape;
	if (shape->approx!=NULL)
	     shapeWire->approxNdx= XkbOutlineIndex(shape,shape->approx);
	else shapeWire->approxNdx= XkbNoShape;
	if (swap) {
	    register int n;
	    swapl(&shapeWire->name,n);
	}
	wire= (char *)&shapeWire[1];
	for (o=0,ol=shape->outlines;o<shape->num_outlines;o++,ol++) {
	    register int	p;
	    XkbPointPtr		pt;
	    xkbPointWireDesc *	ptWire;
	    olWire= (xkbOutlineWireDesc *)wire;
	    olWire->nPoints= ol->num_points;
	    olWire->cornerRadius= ol->corner_radius;
	    wire= (char *)&olWire[1];
	    ptWire= (xkbPointWireDesc *)wire;
	    for (p=0,pt=ol->points;p<ol->num_points;p++,pt++) {
		ptWire[p].x= pt->x;
		ptWire[p].y= pt->y;
		if (swap) {
		    register int n;
		    swaps(&ptWire[p].x,n);
		    swaps(&ptWire[p].y,n);
		}
	    }
	    wire= (char *)&ptWire[ol->num_points];
	}
    }
    return wire;
}