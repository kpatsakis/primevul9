XkbWriteGeomOverlay(char *wire,XkbOverlayPtr ol,Bool swap)
{
register int		r;
XkbOverlayRowPtr	row;
xkbOverlayWireDesc *	olWire;

   olWire= (xkbOverlayWireDesc *)wire;
   olWire->name= ol->name;
   olWire->nRows= ol->num_rows;
   if (swap) {
	register int n;
	swapl(&olWire->name,n);
   }
   wire= (char *)&olWire[1];
   for (r=0,row=ol->rows;r<ol->num_rows;r++,row++) {
   	unsigned int		k;
	XkbOverlayKeyPtr	key;
	xkbOverlayRowWireDesc *	rowWire;
	rowWire= (xkbOverlayRowWireDesc *)wire;
	rowWire->rowUnder= row->row_under;
	rowWire->nKeys= row->num_keys;
	wire= (char *)&rowWire[1];
	for (k=0,key=row->keys;k<row->num_keys;k++,key++) {
	    xkbOverlayKeyWireDesc *	keyWire;
	    keyWire= (xkbOverlayKeyWireDesc *)wire;
	    memcpy(keyWire->over,key->over.name,XkbKeyNameLength);
	    memcpy(keyWire->under,key->under.name,XkbKeyNameLength);
	    wire= (char *)&keyWire[1];
	}
   }
   return wire;
}