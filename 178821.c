static inline LineContribType *_gdContributionsCalc(unsigned int line_size, unsigned int src_size, double scale_d,  const interpolation_method pFilter)
{
    double width_d;
    double scale_f_d = 1.0;
    const double filter_width_d = DEFAULT_BOX_RADIUS;
	int windows_size;
	unsigned int u;
	LineContribType *res;

    if (scale_d < 1.0) {
        width_d = filter_width_d / scale_d;
        scale_f_d = scale_d;
    }  else {
        width_d= filter_width_d;
    }

    windows_size = 2 * (int)ceil(width_d) + 1;
    res = _gdContributionsAlloc(line_size, windows_size);

    for (u = 0; u < line_size; u++) {
        const double dCenter = (double)u / scale_d;
        /* get the significant edge points affecting the pixel */
        register int iLeft = MAX(0, (int)floor (dCenter - width_d));
        int iRight = MIN((int)ceil(dCenter + width_d), (int)src_size - 1);
        double dTotalWeight = 0.0;
		int iSrc;

        /* Cut edge points to fit in filter window in case of spill-off */
        if (iRight - iLeft + 1 > windows_size)  {
            if (iLeft < ((int)src_size - 1 / 2))  {
                iLeft++;
            } else {
                iRight--;
            }
        }

        res->ContribRow[u].Left = iLeft;
        res->ContribRow[u].Right = iRight;

        for (iSrc = iLeft; iSrc <= iRight; iSrc++) {
            dTotalWeight += (res->ContribRow[u].Weights[iSrc-iLeft] =  scale_f_d * (*pFilter)(scale_f_d * (dCenter - (double)iSrc)));
        }

		if (dTotalWeight < 0.0) {
			_gdContributionsFree(res);
			return NULL;
		}

        if (dTotalWeight > 0.0) {
            for (iSrc = iLeft; iSrc <= iRight; iSrc++) {
                res->ContribRow[u].Weights[iSrc-iLeft] /= dTotalWeight;
            }
        }
   }
   return res;
}