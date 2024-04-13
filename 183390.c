cmsBool CMSEXPORT cmsPipelineEvalReverseFloat(cmsFloat32Number Target[],
                                              cmsFloat32Number Result[],
                                              cmsFloat32Number Hint[],
                                              const cmsPipeline* lut)
{
    cmsUInt32Number  i, j;
    cmsFloat64Number  error, LastError = 1E20;
    cmsFloat32Number  fx[4], x[4], xd[4], fxd[4];
    cmsVEC3 tmp, tmp2;
    cmsMAT3 Jacobian;
    
    // Only 3->3 and 4->3 are supported
    if (lut ->InputChannels != 3 && lut ->InputChannels != 4) return FALSE;
    if (lut ->OutputChannels != 3) return FALSE;
   
    // Take the hint as starting point if specified
    if (Hint == NULL) {

        // Begin at any point, we choose 1/3 of CMY axis
        x[0] = x[1] = x[2] = 0.3f;
    }
    else {

        // Only copy 3 channels from hint...
        for (j=0; j < 3; j++)
            x[j] = Hint[j];
    }

    // If Lut is 4-dimensions, then grab target[3], which is fixed
    if (lut ->InputChannels == 4) {
        x[3] = Target[3];
    }
    else x[3] = 0; // To keep lint happy


    // Iterate
    for (i = 0; i < INVERSION_MAX_ITERATIONS; i++) {

        // Get beginning fx
        cmsPipelineEvalFloat(x, fx, lut);

        // Compute error
        error = EuclideanDistance(fx, Target, 3);

        // If not convergent, return last safe value
        if (error >= LastError)
            break;

        // Keep latest values
        LastError     = error;
        for (j=0; j < lut ->InputChannels; j++)
                Result[j] = x[j];

        // Found an exact match?
        if (error <= 0)
            break;

        // Obtain slope (the Jacobian)
        for (j = 0; j < 3; j++) {

            xd[0] = x[0];
            xd[1] = x[1];
            xd[2] = x[2];
            xd[3] = x[3];  // Keep fixed channel

            IncDelta(&xd[j]);

            cmsPipelineEvalFloat(xd, fxd, lut);

            Jacobian.v[0].n[j] = ((fxd[0] - fx[0]) / JACOBIAN_EPSILON);
            Jacobian.v[1].n[j] = ((fxd[1] - fx[1]) / JACOBIAN_EPSILON);
            Jacobian.v[2].n[j] = ((fxd[2] - fx[2]) / JACOBIAN_EPSILON);
        }

        // Solve system
        tmp2.n[0] = fx[0] - Target[0];
        tmp2.n[1] = fx[1] - Target[1];
        tmp2.n[2] = fx[2] - Target[2];

        if (!_cmsMAT3solve(&tmp, &Jacobian, &tmp2))
            return FALSE;

        // Move our guess
        x[0] -= (cmsFloat32Number) tmp.n[0];
        x[1] -= (cmsFloat32Number) tmp.n[1];
        x[2] -= (cmsFloat32Number) tmp.n[2];

        // Some clipping....
        for (j=0; j < 3; j++) {
            if (x[j] < 0) x[j] = 0;
            else
                if (x[j] > 1.0) x[j] = 1.0;
        }
    }

    return TRUE;
}