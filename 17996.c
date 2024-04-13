_CheckSetSections( 	XkbGeometryPtr		geom,
			xkbSetGeometryReq *	req,
			char **			wire_inout,
			ClientPtr		client)
{
Status			status;
register int		s;
char *			wire;
xkbSectionWireDesc *	sWire;
XkbSectionPtr		section;

    wire= *wire_inout;
    if (req->nSections<1)
	return Success;
    sWire= (xkbSectionWireDesc *)wire;
    for (s=0;s<req->nSections;s++) {
	register int		r;
	xkbRowWireDesc *	rWire;
	if (client->swapped) {
	    register int n;
	    swapl(&sWire->name,n);
	    swaps(&sWire->top,n);
	    swaps(&sWire->left,n);
	    swaps(&sWire->width,n);
	    swaps(&sWire->height,n);
	    swaps(&sWire->angle,n);
	}
	CHK_ATOM_ONLY(sWire->name);
	section= XkbAddGeomSection(geom,sWire->name,sWire->nRows,
					sWire->nDoodads,sWire->nOverlays);
	if (!section)
	    return BadAlloc;
	section->priority=	sWire->priority;
	section->top=		sWire->top;
	section->left=		sWire->left;
	section->width=		sWire->width;
	section->height=	sWire->height;
	section->angle=		sWire->angle;
	rWire= (xkbRowWireDesc *)&sWire[1];
	for (r=0;r<sWire->nRows;r++) {
	    register int	k;
	    XkbRowPtr		row;
	    xkbKeyWireDesc *	kWire;
	    if (client->swapped) {
		register int n;
		swaps(&rWire->top,n);
		swaps(&rWire->left,n);
	    }
	    row= XkbAddGeomRow(section,rWire->nKeys);
	    if (!row)
		return BadAlloc;
	    row->top= rWire->top;
	    row->left= rWire->left;
	    row->vertical= rWire->vertical;
	    kWire= (xkbKeyWireDesc *)&rWire[1];
	    for (k=0;k<rWire->nKeys;k++) {
		XkbKeyPtr	key;
		key= XkbAddGeomKey(row);
		if (!key)
		    return BadAlloc;
		memcpy(key->name.name,kWire[k].name,XkbKeyNameLength);
		key->gap= kWire[k].gap;
		key->shape_ndx= kWire[k].shapeNdx;
		key->color_ndx= kWire[k].colorNdx;
		if (key->shape_ndx>=geom->num_shapes) {
		    client->errorValue= _XkbErrCode3(0x10,key->shape_ndx,
							  geom->num_shapes);
		    return BadMatch;
		}
		if (key->color_ndx>=geom->num_colors) {
		    client->errorValue= _XkbErrCode3(0x11,key->color_ndx,
							  geom->num_colors);
		    return BadMatch;
		}
	    }
	    rWire= (xkbRowWireDesc *)&kWire[rWire->nKeys];
	}
	wire= (char *)rWire;
	if (sWire->nDoodads>0) {
	    register int d;
	    for (d=0;d<sWire->nDoodads;d++) {
		status=_CheckSetDoodad(&wire,geom,section,client);
		if (status!=Success)
		    return status;
	    }
	}
	if (sWire->nOverlays>0) {
	    register int o;
	    for (o=0;o<sWire->nOverlays;o++) {
		status= _CheckSetOverlay(&wire,geom,section,client);
		if (status!=Success)
		    return status;
	    }
	}
	sWire= (xkbSectionWireDesc *)wire;
    }
    wire= (char *)sWire;
    *wire_inout= wire;
    return Success;
}