xmlXPathIsNaN(double val) {
#ifdef isnan
    return isnan(val);
#else
    return !(val == val);
#endif
}