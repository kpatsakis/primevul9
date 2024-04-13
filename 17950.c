XkbSizeGeomProperties(XkbGeometryPtr geom)
{
register int 	i,size;
XkbPropertyPtr	prop;
    
    for (size=i=0,prop=geom->properties;i<geom->num_properties;i++,prop++) {
	size+= XkbSizeCountedString(prop->name);
	size+= XkbSizeCountedString(prop->value);
    }
    return size;
}