int dummyDCTFilter(short *coeffs, tjregion arrayRegion, tjregion planeRegion,
	int componentIndex, int transformIndex, tjtransform *transform)
{
	int i;
	for(i=0; i<arrayRegion.w*arrayRegion.h; i++) coeffs[i]=-coeffs[i];
	return 0;
}