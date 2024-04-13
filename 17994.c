_CheckSetOverlay(	char **		wire_inout,
			XkbGeometryPtr	geom,
			XkbSectionPtr	section,
			ClientPtr	client)
{
register int		r;
char *			wire;
XkbOverlayPtr		ol;
xkbOverlayWireDesc *	olWire;
xkbOverlayRowWireDesc *	rWire;

    wire= *wire_inout;
    olWire= (xkbOverlayWireDesc *)wire;
    if (client->swapped) {
	register int n;
	swapl(&olWire->name,n);
    }
    CHK_ATOM_ONLY(olWire->name);
    ol= XkbAddGeomOverlay(section,olWire->name,olWire->nRows);
    rWire= (xkbOverlayRowWireDesc *)&olWire[1];
    for (r=0;r<olWire->nRows;r++) {
	register int		k;
	xkbOverlayKeyWireDesc *	kWire;
	XkbOverlayRowPtr	row;

	if (rWire->rowUnder>section->num_rows) {
	    client->errorValue= _XkbErrCode4(0x20,r,section->num_rows,
							rWire->rowUnder);
	    return BadMatch;
	}
	row= XkbAddGeomOverlayRow(ol,rWire->rowUnder,rWire->nKeys);
	kWire= (xkbOverlayKeyWireDesc *)&rWire[1];
	for (k=0;k<rWire->nKeys;k++,kWire++) {
	    if (XkbAddGeomOverlayKey(ol,row,
	    		(char *)kWire->over,(char *)kWire->under)==NULL) {
		client->errorValue= _XkbErrCode3(0x21,r,k);
		return BadMatch;
	    }	
	}
	rWire= (xkbOverlayRowWireDesc *)kWire;
    }
    olWire= (xkbOverlayWireDesc *)rWire;
    wire= (char *)olWire;
    *wire_inout= wire;
    return Success;
}