int GetInterval(cmsFloat64Number In, const cmsUInt16Number LutTable[], const struct _cms_interp_struc* p)
{
    int i;
    int y0, y1;

    // A 1 point table is not allowed
    if (p -> Domain[0] < 1) return -1;

    // Let's see if ascending or descending.
    if (LutTable[0] < LutTable[p ->Domain[0]]) {

        // Table is overall ascending
        for (i=p->Domain[0]-1; i >=0; --i) {

            y0 = LutTable[i];
            y1 = LutTable[i+1];

            if (y0 <= y1) { // Increasing
                if (In >= y0 && In <= y1) return i;
            }
            else
                if (y1 < y0) { // Decreasing
                    if (In >= y1 && In <= y0) return i;
                }
        }
    }
    else {
        // Table is overall descending
        for (i=0; i < (int) p -> Domain[0]; i++) {

            y0 = LutTable[i];
            y1 = LutTable[i+1];

            if (y0 <= y1) { // Increasing
                if (In >= y0 && In <= y1) return i;
            }
            else
                if (y1 < y0) { // Decreasing
                    if (In >= y1 && In <= y0) return i;
                }
        }
    }

    return -1;
}