XkbSizeGeomDoodads(int num_doodads,XkbDoodadPtr doodad)
{
register int	i,size;

    for (i=size=0;i<num_doodads;i++,doodad++) {
	size+= SIZEOF(xkbAnyDoodadWireDesc);
	if (doodad->any.type==XkbTextDoodad) {
	    size+= XkbSizeCountedString(doodad->text.text);
	    size+= XkbSizeCountedString(doodad->text.font);
	}
	else if (doodad->any.type==XkbLogoDoodad) {
	    size+= XkbSizeCountedString(doodad->logo.logo_name);
	}
    }
    return size;
}