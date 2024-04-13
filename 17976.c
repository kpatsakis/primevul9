_CheckSetShapes( 	XkbGeometryPtr		geom,
			xkbSetGeometryReq *	req,
			char **			wire_inout,
			ClientPtr		client)
{
register int	i;
char *		wire;

    wire= *wire_inout;
    if (req->nShapes<1) {
	client->errorValue= _XkbErrCode2(0x06,req->nShapes);
	return BadValue;
    }
    else {
	xkbShapeWireDesc *	shapeWire;
	XkbShapePtr		shape;
	register int		o;
	shapeWire= (xkbShapeWireDesc *)wire;
	for (i=0;i<req->nShapes;i++) {
	    xkbOutlineWireDesc *	olWire;
	    XkbOutlinePtr		ol;
	    shape= XkbAddGeomShape(geom,shapeWire->name,shapeWire->nOutlines);
	    if (!shape)
		return BadAlloc;
	    olWire= (xkbOutlineWireDesc *)(&shapeWire[1]);
	    for (o=0;o<shapeWire->nOutlines;o++) {
		register int		p;
		XkbPointPtr		pt;
		xkbPointWireDesc *	ptWire;

		ol= XkbAddGeomOutline(shape,olWire->nPoints);
		if (!ol)
		    return BadAlloc;
		ol->corner_radius=	olWire->cornerRadius;
		ptWire= (xkbPointWireDesc *)&olWire[1];
		for (p=0,pt=ol->points;p<olWire->nPoints;p++,pt++) {
		    pt->x= ptWire[p].x;
		    pt->y= ptWire[p].y;
		    if (client->swapped) {
			register int n;
			swaps(&pt->x,n);
			swaps(&pt->y,n);
		    }
		}
		ol->num_points= olWire->nPoints;
		olWire= (xkbOutlineWireDesc *)(&ptWire[olWire->nPoints]);
	    }
	    if (shapeWire->primaryNdx!=XkbNoShape)
		shape->primary= &shape->outlines[shapeWire->primaryNdx];
	    if (shapeWire->approxNdx!=XkbNoShape)
		shape->approx= &shape->outlines[shapeWire->approxNdx];
	    shapeWire= (xkbShapeWireDesc *)olWire;
	}
	wire= (char *)shapeWire;
    }
    if (geom->num_shapes!=req->nShapes) {
	client->errorValue= _XkbErrCode3(0x07,geom->num_shapes,req->nShapes);
	return BadMatch;
    }

    *wire_inout= wire;
    return Success;
}