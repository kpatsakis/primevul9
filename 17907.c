GetComponentSpec(unsigned char **pWire,Bool allowExpr,int *errRtrn)
{
int		len;
register int	i;
unsigned char	*wire,*str,*tmp,*legal;

    if (allowExpr)	legal= &componentExprLegal[0];
    else		legal= &componentSpecLegal[0];

    wire= *pWire;
    len= (*(unsigned char *)wire++);
    if (len>0) {
	str= (unsigned char *)_XkbCalloc(1, len+1);
	if (str) {
	    tmp= str;
	    for (i=0;i<len;i++) {
		if (legal[(*wire)/8]&(1<<((*wire)%8)))
		    *tmp++= *wire++;
		else wire++;
	    }
	    if (tmp!=str)
		*tmp++= '\0';
	    else {
		_XkbFree(str);
		str= NULL;
	    }
	}
	else {
	    *errRtrn= BadAlloc;
	}
    }
    else {
	str= NULL;
    }
    *pWire= wire;
    return (char *)str;
}