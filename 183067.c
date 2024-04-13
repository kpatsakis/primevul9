void EmitNGamma(cmsIOHANDLER* m, cmsUInt32Number n, cmsToneCurve* g[])
{
    cmsUInt32Number i;

    for( i=0; i < n; i++ )
    {
        if (g[i] == NULL) return; // Error

        if (i > 0 && GammaTableEquals(g[i-1]->Table16, g[i]->Table16, g[i]->nEntries)) {

            _cmsIOPrintf(m, "dup ");
        }
        else {
            Emit1Gamma(m, g[i]);
        }
    }

}