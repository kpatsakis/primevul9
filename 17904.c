_XkbCountAtoms(Atom *atoms,int maxAtoms,int *count)
{
register unsigned int i,bit,nAtoms;
register CARD32 atomsPresent;

    for (i=nAtoms=atomsPresent=0,bit=1;i<maxAtoms;i++,bit<<=1) {
	if (atoms[i]!=None) {
	    atomsPresent|= bit;
	    nAtoms++;
	}
    }
    if (count)
	*count= nAtoms;
    return atomsPresent;
}