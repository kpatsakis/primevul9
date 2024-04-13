_XkbSetGeometry(ClientPtr client, DeviceIntPtr dev, xkbSetGeometryReq *stuff)
{
    XkbDescPtr		xkb;
    Bool		new_name;
    xkbNewKeyboardNotify	nkn;
    XkbGeometryPtr	geom,old;
    XkbGeometrySizesRec	sizes;
    Status		status;

    xkb= dev->key->xkbInfo->desc;
    old= xkb->geom;
    xkb->geom= NULL;

    sizes.which=		XkbGeomAllMask;
    sizes.num_properties=	stuff->nProperties;
    sizes.num_colors=	stuff->nColors;
    sizes.num_shapes=	stuff->nShapes;
    sizes.num_sections=	stuff->nSections;
    sizes.num_doodads=	stuff->nDoodads;
    sizes.num_key_aliases=	stuff->nKeyAliases;
    if ((status= XkbAllocGeometry(xkb,&sizes))!=Success) {
        xkb->geom= old;
        return status;
    }
    geom= xkb->geom;
    geom->name= stuff->name;
    geom->width_mm= stuff->widthMM;
    geom->height_mm= stuff->heightMM;
    if ((status= _CheckSetGeom(geom,stuff,client))!=Success) {
        XkbFreeGeometry(geom,XkbGeomAllMask,True);
        xkb->geom= old;
        return status;
    }
    new_name= (xkb->names->geometry!=geom->name);
    xkb->names->geometry= geom->name;
    if (old)
        XkbFreeGeometry(old,XkbGeomAllMask,True);
    if (new_name) {
        xkbNamesNotify	nn;
        bzero(&nn,sizeof(xkbNamesNotify));
        nn.changed= XkbGeometryNameMask;
        XkbSendNamesNotify(dev,&nn);
    }
    nkn.deviceID= nkn.oldDeviceID= dev->id;
    nkn.minKeyCode= nkn.oldMinKeyCode= xkb->min_key_code;
    nkn.maxKeyCode= nkn.oldMaxKeyCode= xkb->max_key_code;
    nkn.requestMajor=	XkbReqCode;
    nkn.requestMinor=	X_kbSetGeometry;
    nkn.changed=	XkbNKN_GeometryMask;
    XkbSendNewKeyboardNotify(dev,&nkn);
    return Success;
}