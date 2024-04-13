XkbSizeGeomSections(XkbGeometryPtr geom)
{
register int 	i,size;
XkbSectionPtr	section;

    for (i=size=0,section=geom->sections;i<geom->num_sections;i++,section++) {
	size+= SIZEOF(xkbSectionWireDesc);
	if (section->rows) {
	    int		r;
	    XkbRowPtr	row;
	    for (r=0,row=section->rows;r<section->num_rows;row++,r++) {
		size+= SIZEOF(xkbRowWireDesc);
		size+= row->num_keys*SIZEOF(xkbKeyWireDesc);
	    }
	}
	if (section->doodads)
	    size+= XkbSizeGeomDoodads(section->num_doodads,section->doodads);
	if (section->overlays) {
	    int			o;
	    XkbOverlayPtr	ol;
	    for (o=0,ol=section->overlays;o<section->num_overlays;o++,ol++) {
		int			r;
		XkbOverlayRowPtr	row;
		size+= SIZEOF(xkbOverlayWireDesc);
		for (r=0,row=ol->rows;r<ol->num_rows;r++,row++) {
		   size+= SIZEOF(xkbOverlayRowWireDesc);
		   size+= row->num_keys*SIZEOF(xkbOverlayKeyWireDesc);
		}
	    }
	}
    }
    return size;
}