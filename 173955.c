static void decorrelate_init(QuicFamily *family, int bpc)
{
    const unsigned int pixelbitmask = bppmask[bpc];
    const unsigned int pixelbitmaskshr = pixelbitmask >> 1;
    unsigned int s;

    //spice_assert(bpc <= 8);

    for (s = 0; s <= pixelbitmask; s++) {
        if (s <= pixelbitmaskshr) {
            family->xlatU2L[s] = s << 1;
        } else {
            family->xlatU2L[s] = ((pixelbitmask - s) << 1) + 1;
        }
    }
}