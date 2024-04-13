XkbSendGeometry(	ClientPtr		client,
			XkbGeometryPtr		geom,
			xkbGetGeometryReply *	rep,
			Bool			freeGeom)
{
    char	*desc,*start;
    int		 len;

    if (geom!=NULL) {
	len= rep->length*4;
	start= desc= (char *)xalloc(len);
	if (!start)
	    return BadAlloc;
	desc=  XkbWriteCountedString(desc,geom->label_font,client->swapped);
	if ( rep->nProperties>0 )
	    desc = XkbWriteGeomProperties(desc,geom,client->swapped);
	if ( rep->nColors>0 )
	    desc = XkbWriteGeomColors(desc,geom,client->swapped);
	if ( rep->nShapes>0 )
	    desc = XkbWriteGeomShapes(desc,geom,client->swapped);
	if ( rep->nSections>0 )
	    desc = XkbWriteGeomSections(desc,geom,client->swapped);
	if ( rep->nDoodads>0 )
	    desc = XkbWriteGeomDoodads(desc,geom->num_doodads,geom->doodads,
							  client->swapped);
	if ( rep->nKeyAliases>0 )
	    desc = XkbWriteGeomKeyAliases(desc,geom,client->swapped);
	if ((desc-start)!=(len)) {
	    ErrorF("[xkb] BOGUS LENGTH in XkbSendGeometry, expected %d, got %ld\n",
			len, (unsigned long)(desc-start));
	}
    }
    else {
	len= 0;
	start= NULL;
    }
    if (client->swapped) {
	register int n;
	swaps(&rep->sequenceNumber,n);
	swapl(&rep->length,n);
	swapl(&rep->name,n);
	swaps(&rep->widthMM,n);
	swaps(&rep->heightMM,n);
	swaps(&rep->nProperties,n);
	swaps(&rep->nColors,n);
	swaps(&rep->nShapes,n);
	swaps(&rep->nSections,n);
	swaps(&rep->nDoodads,n);
	swaps(&rep->nKeyAliases,n);
    }
    WriteToClient(client, SIZEOF(xkbGetGeometryReply), (char *)rep);
    if (len>0)
	WriteToClient(client, len, start);
    if (start!=NULL)
	xfree((char *)start);
    if (freeGeom)
	XkbFreeGeometry(geom,XkbGeomAllMask,True);
    return client->noClientException;
}