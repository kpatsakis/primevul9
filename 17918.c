XkbSizeKeyTypes(XkbDescPtr xkb,xkbGetMapReply *rep)
{
    XkbKeyTypeRec 	*type;
    unsigned		i,len;

    len= 0;
    if (((rep->present&XkbKeyTypesMask)==0)||(rep->nTypes<1)||
	(!xkb)||(!xkb->map)||(!xkb->map->types)) {
	rep->present&= ~XkbKeyTypesMask;
	rep->firstType= rep->nTypes= 0;
	return 0;
    }
    type= &xkb->map->types[rep->firstType];
    for (i=0;i<rep->nTypes;i++,type++){
	len+= SIZEOF(xkbKeyTypeWireDesc);
	if (type->map_count>0) {
	    len+= (type->map_count*SIZEOF(xkbKTMapEntryWireDesc));
	    if (type->preserve)
		len+= (type->map_count*SIZEOF(xkbModsWireDesc));
	}
    }
    return len;
}