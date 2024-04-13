_XkbCopyMaskedAtoms(	Atom	*wire,
    			Atom	*dest,
			int   	 nAtoms,
			CARD32	 present)
{
register int i,bit;

    for (i=0,bit=1;(i<nAtoms)&&(present);i++,bit<<=1) {
	if ((present&bit)==0)
	    continue;
	dest[i]= *wire++;
    }
    return wire;
}