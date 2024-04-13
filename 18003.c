_XkbWriteAtoms(char *wire,Atom *atoms,int maxAtoms,int swap)
{
register unsigned int i;
Atom *atm;

    atm = (Atom *)wire;
    for (i=0;i<maxAtoms;i++) {
	if (atoms[i]!=None) {
	    *atm= atoms[i];
	    if (swap) {
		register int n;
		swapl(atm,n);
	    }
	    atm++;
	}
    }
    return (char *)atm;
}