XkbComputeGetGeometryReplySize(	XkbGeometryPtr		geom,
				xkbGetGeometryReply *	rep,
				Atom			name)
{
int	len;

    if (geom!=NULL) {
	len= XkbSizeCountedString(geom->label_font);
	len+= XkbSizeGeomProperties(geom);
	len+= XkbSizeGeomColors(geom);
	len+= XkbSizeGeomShapes(geom);
	len+= XkbSizeGeomSections(geom);
	len+= XkbSizeGeomDoodads(geom->num_doodads,geom->doodads);
	len+= XkbSizeGeomKeyAliases(geom);
	rep->length= len/4;
	rep->found= True;
	rep->name= geom->name;
	rep->widthMM= geom->width_mm;
	rep->heightMM= geom->height_mm;
	rep->nProperties= geom->num_properties;
	rep->nColors= geom->num_colors;
	rep->nShapes= geom->num_shapes;
	rep->nSections= geom->num_sections;
	rep->nDoodads= geom->num_doodads;
	rep->nKeyAliases= geom->num_key_aliases;
	rep->baseColorNdx= XkbGeomColorIndex(geom,geom->base_color);
	rep->labelColorNdx= XkbGeomColorIndex(geom,geom->label_color);
    }
    else {
	rep->length= 0;
	rep->found= False;
	rep->name= name;
	rep->widthMM= rep->heightMM= 0;
	rep->nProperties= rep->nColors= rep->nShapes= 0;
	rep->nSections= rep->nDoodads= 0;
	rep->nKeyAliases= 0;
	rep->labelColorNdx= rep->baseColorNdx= 0;
    }
    return Success;
}