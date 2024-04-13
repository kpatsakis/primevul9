XkbWriteCountedString(char *wire,char *str,Bool swap)
{
CARD16	len,*pLen;

    len= (str?strlen(str):0);
    pLen= (CARD16 *)wire;
    *pLen= len;
    if (swap) {
	register int n;
	swaps(pLen,n);
    }
    memcpy(&wire[2],str,len);
    wire+= ((2+len+3)/4)*4;
    return wire;
}