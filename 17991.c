XkbComputeGetIndicatorMapReplySize(
    XkbIndicatorPtr		indicators,
    xkbGetIndicatorMapReply	*rep)
{
register int 	i,bit;
int		nIndicators;

    rep->realIndicators = indicators->phys_indicators;
    for (i=nIndicators=0,bit=1;i<XkbNumIndicators;i++,bit<<=1) {
	if (rep->which&bit)
	    nIndicators++;
    }
    rep->length = (nIndicators*SIZEOF(xkbIndicatorMapWireDesc))/4;
    return Success;
}