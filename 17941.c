_CheckSetGeom(	XkbGeometryPtr		geom,
		xkbSetGeometryReq *	req,
		ClientPtr 		client)
{
register int	i;
Status		status;
char *		wire;

    wire= (char *)&req[1];
    geom->label_font= _GetCountedString(&wire,client->swapped);

    for (i=0;i<req->nProperties;i++) {
	char *name,*val;
	name= _GetCountedString(&wire,client->swapped);
        if (!name)
            return BadAlloc;
	val= _GetCountedString(&wire,client->swapped);
        if (!val) {
            xfree(name);
            return BadAlloc;
        }
	if (XkbAddGeomProperty(geom,name,val)==NULL) {
            xfree(name);
            xfree(val);
	    return BadAlloc;
        }
        xfree(name);
        xfree(val);
    }

    if (req->nColors<2) {
	client->errorValue= _XkbErrCode3(0x01,2,req->nColors);
	return BadValue;
    }
    if (req->baseColorNdx>req->nColors) {
	client->errorValue=_XkbErrCode3(0x03,req->nColors,req->baseColorNdx);
	return BadMatch;
    }
    if (req->labelColorNdx>req->nColors) {
	client->errorValue= _XkbErrCode3(0x03,req->nColors,req->labelColorNdx);
	return BadMatch;
    }
    if (req->labelColorNdx==req->baseColorNdx) {
	client->errorValue= _XkbErrCode3(0x04,req->baseColorNdx,
                                         req->labelColorNdx);
	return BadMatch;
    }

    for (i=0;i<req->nColors;i++) {
	char *name;
	name= _GetCountedString(&wire,client->swapped);
	if (!name)
            return BadAlloc;
        if (!XkbAddGeomColor(geom,name,geom->num_colors)) {
            xfree(name);
	    return BadAlloc;
        }
        xfree(name);
    }
    if (req->nColors!=geom->num_colors) {
	client->errorValue= _XkbErrCode3(0x05,req->nColors,geom->num_colors);
	return BadMatch;
    }
    geom->label_color= &geom->colors[req->labelColorNdx];
    geom->base_color= &geom->colors[req->baseColorNdx];

    if ((status=_CheckSetShapes(geom,req,&wire,client))!=Success)
	return status;

    if ((status=_CheckSetSections(geom,req,&wire,client))!=Success)
	return status;

    for (i=0;i<req->nDoodads;i++) {
	status=_CheckSetDoodad(&wire,geom,NULL,client);
	if (status!=Success)
	    return status;
    }

    for (i=0;i<req->nKeyAliases;i++) {
	if (XkbAddGeomKeyAlias(geom,&wire[XkbKeyNameLength],wire)==NULL)
	    return BadAlloc;
	wire+= 2*XkbKeyNameLength;
    }
    return Success;
}