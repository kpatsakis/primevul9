xmlXPathIsInf(double val) {
#ifdef isinf
    return isinf(val) ? (val > 0 ? 1 : -1) : 0;
#else
    if (val >= INFINITY)
        return 1;
    if (val <= -INFINITY)
        return -1;
    return 0;
#endif
}