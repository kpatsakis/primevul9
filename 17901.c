_CheckSetDoodad(	char **		wire_inout,
			XkbGeometryPtr	geom,
			XkbSectionPtr	section,
			ClientPtr	client)
{
char *			wire;
xkbDoodadWireDesc *	dWire;
XkbDoodadPtr		doodad;

    dWire= (xkbDoodadWireDesc *)(*wire_inout);
    wire= (char *)&dWire[1];
    if (client->swapped) {
	register int n;
	swapl(&dWire->any.name,n);
	swaps(&dWire->any.top,n);
	swaps(&dWire->any.left,n);
	swaps(&dWire->any.angle,n);
    }
    CHK_ATOM_ONLY(dWire->any.name);
    doodad= XkbAddGeomDoodad(geom,section,dWire->any.name);
    if (!doodad)
	return BadAlloc;
    doodad->any.type= dWire->any.type;
    doodad->any.priority= dWire->any.priority;
    doodad->any.top= dWire->any.top;
    doodad->any.left= dWire->any.left;
    doodad->any.angle= dWire->any.angle;
    switch (doodad->any.type) {
	case XkbOutlineDoodad:
	case XkbSolidDoodad:
	    if (dWire->shape.colorNdx>=geom->num_colors) {
		client->errorValue= _XkbErrCode3(0x40,geom->num_colors,
							dWire->shape.colorNdx);
		return BadMatch;
	    }
	    if (dWire->shape.shapeNdx>=geom->num_shapes) {
		client->errorValue= _XkbErrCode3(0x41,geom->num_shapes,
							dWire->shape.shapeNdx);
		return BadMatch;
	    }
	    doodad->shape.color_ndx= dWire->shape.colorNdx;
	    doodad->shape.shape_ndx= dWire->shape.shapeNdx;
	    break;
	case XkbTextDoodad:
	    if (dWire->text.colorNdx>=geom->num_colors) {
		client->errorValue= _XkbErrCode3(0x42,geom->num_colors,
							dWire->text.colorNdx);
		return BadMatch;
	    }
	    if (client->swapped) {
		register int n;
		swaps(&dWire->text.width,n);
		swaps(&dWire->text.height,n);
	    }
	    doodad->text.width= dWire->text.width;
	    doodad->text.height= dWire->text.height;
	    doodad->text.color_ndx= dWire->text.colorNdx;
	    doodad->text.text= _GetCountedString(&wire,client->swapped);
	    doodad->text.font= _GetCountedString(&wire,client->swapped);
	    break;
	case XkbIndicatorDoodad:
	    if (dWire->indicator.onColorNdx>=geom->num_colors) {
		client->errorValue= _XkbErrCode3(0x43,geom->num_colors,
						dWire->indicator.onColorNdx);
		return BadMatch;
	    }
	    if (dWire->indicator.offColorNdx>=geom->num_colors) {
		client->errorValue= _XkbErrCode3(0x44,geom->num_colors,
						dWire->indicator.offColorNdx);
		return BadMatch;
	    }
	    if (dWire->indicator.shapeNdx>=geom->num_shapes) {
		client->errorValue= _XkbErrCode3(0x45,geom->num_shapes,
						dWire->indicator.shapeNdx);
		return BadMatch;
	    }
	    doodad->indicator.shape_ndx= dWire->indicator.shapeNdx;
	    doodad->indicator.on_color_ndx= dWire->indicator.onColorNdx;
	    doodad->indicator.off_color_ndx= dWire->indicator.offColorNdx;
	    break;
	case XkbLogoDoodad:
	    if (dWire->logo.colorNdx>=geom->num_colors) {
		client->errorValue= _XkbErrCode3(0x46,geom->num_colors,
							dWire->logo.colorNdx);
		return BadMatch;
	    }
	    if (dWire->logo.shapeNdx>=geom->num_shapes) {
		client->errorValue= _XkbErrCode3(0x47,geom->num_shapes,
							dWire->logo.shapeNdx);
		return BadMatch;
	    }
	    doodad->logo.color_ndx= dWire->logo.colorNdx;
	    doodad->logo.shape_ndx= dWire->logo.shapeNdx;
	    doodad->logo.logo_name= _GetCountedString(&wire,client->swapped);
	    break;
	default:
	    client->errorValue= _XkbErrCode2(0x4F,dWire->any.type);
	    return BadValue;
    }
    *wire_inout= wire;
    return Success;
}