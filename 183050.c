void Emit1Gamma(cmsIOHANDLER* m, cmsToneCurve* Table)
{
    cmsUInt32Number i;
    cmsFloat64Number gamma;

    if (Table == NULL) return; // Error

    if (Table ->nEntries <= 0) return;  // Empty table

    // Suppress whole if identity
    if (cmsIsToneCurveLinear(Table)) return;

    // Check if is really an exponential. If so, emit "exp"
    gamma = cmsEstimateGamma(Table, 0.001);
     if (gamma > 0) {
            _cmsIOPrintf(m, "{ %g exp } bind ", gamma);
            return;
     }

    _cmsIOPrintf(m, "{ ");

    // Bounds check
    EmitRangeCheck(m);

    // Emit intepolation code

    // PostScript code                      Stack
    // ===============                      ========================
                                            // v
    _cmsIOPrintf(m, " [");

    for (i=0; i < Table->nEntries; i++) {
        _cmsIOPrintf(m, "%d ", Table->Table16[i]);
    }

    _cmsIOPrintf(m, "] ");                        // v tab

    _cmsIOPrintf(m, "dup ");                      // v tab tab
    _cmsIOPrintf(m, "length 1 sub ");             // v tab dom
    _cmsIOPrintf(m, "3 -1 roll ");                // tab dom v
    _cmsIOPrintf(m, "mul ");                      // tab val2
    _cmsIOPrintf(m, "dup ");                      // tab val2 val2
    _cmsIOPrintf(m, "dup ");                      // tab val2 val2 val2
    _cmsIOPrintf(m, "floor cvi ");                // tab val2 val2 cell0
    _cmsIOPrintf(m, "exch ");                     // tab val2 cell0 val2
    _cmsIOPrintf(m, "ceiling cvi ");              // tab val2 cell0 cell1
    _cmsIOPrintf(m, "3 index ");                  // tab val2 cell0 cell1 tab
    _cmsIOPrintf(m, "exch ");                     // tab val2 cell0 tab cell1
    _cmsIOPrintf(m, "get ");                      // tab val2 cell0 y1
    _cmsIOPrintf(m, "4 -1 roll ");                // val2 cell0 y1 tab
    _cmsIOPrintf(m, "3 -1 roll ");                // val2 y1 tab cell0
    _cmsIOPrintf(m, "get ");                      // val2 y1 y0
    _cmsIOPrintf(m, "dup ");                      // val2 y1 y0 y0
    _cmsIOPrintf(m, "3 1 roll ");                 // val2 y0 y1 y0
    _cmsIOPrintf(m, "sub ");                      // val2 y0 (y1-y0)
    _cmsIOPrintf(m, "3 -1 roll ");                // y0 (y1-y0) val2
    _cmsIOPrintf(m, "dup ");                      // y0 (y1-y0) val2 val2
    _cmsIOPrintf(m, "floor cvi ");                // y0 (y1-y0) val2 floor(val2)
    _cmsIOPrintf(m, "sub ");                      // y0 (y1-y0) rest
    _cmsIOPrintf(m, "mul ");                      // y0 t1
    _cmsIOPrintf(m, "add ");                      // y
    _cmsIOPrintf(m, "65535 div ");                // result

    _cmsIOPrintf(m, " } bind ");
}