ComputeDeviceLedInfoSize(	DeviceIntPtr		dev,
				unsigned int		what,
				XkbSrvLedInfoPtr	sli)
{
int			nNames,nMaps;
register unsigned 	n,bit;

    if (sli==NULL)
	return 0;
    nNames= nMaps= 0;
    if ((what&XkbXI_IndicatorNamesMask)==0)
	sli->namesPresent= 0;
    if ((what&XkbXI_IndicatorMapsMask)==0)
	sli->mapsPresent= 0;

    for (n=0,bit=1;n<XkbNumIndicators;n++,bit<<=1) {
	if (sli->names && sli->names[n]!=None) {
	    sli->namesPresent|= bit;
	    nNames++;
	}
	if (sli->maps && XkbIM_InUse(&sli->maps[n])) {
	    sli->mapsPresent|= bit;
	    nMaps++;
	}
    }
    return (nNames*4)+(nMaps*SIZEOF(xkbIndicatorMapWireDesc));
}