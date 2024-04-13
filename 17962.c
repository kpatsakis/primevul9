_XkbCheckMaskedAtoms(CARD32 *wire,int nAtoms,CARD32 present,int swapped,
								Atom *pError)
{
register unsigned i,bit;

    for (i=0,bit=1;(i<nAtoms)&&(present);i++,bit<<=1) {
	if ((present&bit)==0)
	    continue;
	if (swapped) {
	    register int n;
	    swapl(wire,n);
	}
	if ((((Atom)*wire)!=None)&&(!ValidAtom(((Atom)*wire)))) {
	    *pError= (Atom)*wire;
	    return NULL;
	}
	wire++;
    }
    return wire;
}