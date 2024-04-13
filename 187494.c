cmsToneCurve* CMSEXPORT cmsBuildTabulatedToneCurveFloat(cmsContext ContextID, cmsUInt32Number nEntries, const cmsFloat32Number values[])
{
    cmsCurveSegment Seg[3];

    // A segmented tone curve should have function segments in the first and last positions
    // Initialize segmented curve part up to 0 to constant value = samples[0]
    Seg[0].x0 = MINUS_INF;
    Seg[0].x1 = 0;
    Seg[0].Type = 6;

    Seg[0].Params[0] = 1;
    Seg[0].Params[1] = 0;
    Seg[0].Params[2] = 0;
    Seg[0].Params[3] = values[0];
    Seg[0].Params[4] = 0;

    // From zero to 1
    Seg[1].x0 = 0;
    Seg[1].x1 = 1.0;
    Seg[1].Type = 0;

    Seg[1].nGridPoints = nEntries;
    Seg[1].SampledPoints = (cmsFloat32Number*) values;

	// Final segment is constant = lastsample
	Seg[2].x0 = 1.0;
	Seg[2].x1 = PLUS_INF;
	Seg[2].Type = 6;
	
    Seg[2].Params[0] = 1;
    Seg[2].Params[1] = 0;
    Seg[2].Params[2] = 0;
    Seg[2].Params[3] = values[nEntries-1];
    Seg[2].Params[4] = 0;
	

    return cmsBuildSegmentedToneCurve(ContextID, 3, Seg);
}