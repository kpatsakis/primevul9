_GetCountedString(char **wire_inout,Bool swap)
{
char *	wire,*str;
CARD16	len,*plen;

    wire= *wire_inout;
    plen= (CARD16 *)wire;
    if (swap) {
	register int n;
	swaps(plen,n);
    }
    len= *plen;
    str= (char *)_XkbAlloc(len+1);
    if (str) {
	memcpy(str,&wire[2],len);
	str[len]= '\0';
    }
    wire+= XkbPaddedSize(len+2);
    *wire_inout= wire;
    return str;
}