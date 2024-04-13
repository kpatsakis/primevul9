XkbWriteGeomDoodads(char *wire,int num_doodads,XkbDoodadPtr doodad,Bool swap)
{
register int		i;
xkbDoodadWireDesc *	doodadWire;

    for (i=0;i<num_doodads;i++,doodad++) {
	doodadWire= (xkbDoodadWireDesc *)wire;
	wire= (char *)&doodadWire[1];
	bzero(doodadWire,SIZEOF(xkbDoodadWireDesc));
	doodadWire->any.name= doodad->any.name;
	doodadWire->any.type= doodad->any.type;
	doodadWire->any.priority= doodad->any.priority;
	doodadWire->any.top= doodad->any.top;
	doodadWire->any.left= doodad->any.left;
	if (swap) {
	    register int n;
	    swapl(&doodadWire->any.name,n);
	    swaps(&doodadWire->any.top,n);
	    swaps(&doodadWire->any.left,n);
	}
	switch (doodad->any.type) {
	    case XkbOutlineDoodad:
	    case XkbSolidDoodad:
		doodadWire->shape.angle= doodad->shape.angle;
		doodadWire->shape.colorNdx= doodad->shape.color_ndx;
		doodadWire->shape.shapeNdx= doodad->shape.shape_ndx;
		if (swap) {
		    register int n;
		    swaps(&doodadWire->shape.angle,n);
		}
		break;
	    case XkbTextDoodad:
		doodadWire->text.angle= doodad->text.angle;
		doodadWire->text.width= doodad->text.width;
		doodadWire->text.height= doodad->text.height;
		doodadWire->text.colorNdx= doodad->text.color_ndx;
		if (swap) {
		    register int n;
		    swaps(&doodadWire->text.angle,n);
		    swaps(&doodadWire->text.width,n);
		    swaps(&doodadWire->text.height,n);
		}
		wire= XkbWriteCountedString(wire,doodad->text.text,swap);
		wire= XkbWriteCountedString(wire,doodad->text.font,swap);
		break;
	    case XkbIndicatorDoodad:
		doodadWire->indicator.shapeNdx= doodad->indicator.shape_ndx;
		doodadWire->indicator.onColorNdx=doodad->indicator.on_color_ndx;
		doodadWire->indicator.offColorNdx=
						doodad->indicator.off_color_ndx;
		break;
	    case XkbLogoDoodad:
		doodadWire->logo.angle= doodad->logo.angle;
		doodadWire->logo.colorNdx= doodad->logo.color_ndx;
		doodadWire->logo.shapeNdx= doodad->logo.shape_ndx;
		wire= XkbWriteCountedString(wire,doodad->logo.logo_name,swap);
		break;
	    default:
		ErrorF("[xkb] Unknown doodad type %d in XkbWriteGeomDoodads\n",
			doodad->any.type);
		ErrorF("[xkb] Ignored\n");
		break;
	}
    }
    return wire;
}