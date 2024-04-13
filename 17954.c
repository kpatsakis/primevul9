XkbSizeVirtualMods(XkbDescPtr xkb,xkbGetMapReply *rep)
{
register unsigned i,nMods,bit;

    if (((rep->present&XkbVirtualModsMask)==0)||(rep->virtualMods==0)||
	(!xkb)||(!xkb->server)) {
	rep->present&= ~XkbVirtualModsMask;
	rep->virtualMods= 0;
	return 0;
    }
    for (i=nMods=0,bit=1;i<XkbNumVirtualMods;i++,bit<<=1) {
        if (rep->virtualMods&bit)
	    nMods++;
    }
    return XkbPaddedSize(nMods);
}