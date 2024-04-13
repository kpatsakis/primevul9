static void correlate_init(QuicFamily *family, int bpc)
{
    const unsigned int pixelbitmask = bppmask[bpc];
    unsigned long int s;

    //spice_assert(bpc <= 8);

    for (s = 0; s <= pixelbitmask; s++) {
        if (s & 0x01) {
            family->xlatL2U[s] = pixelbitmask - (s >> 1);
        } else {
            family->xlatL2U[s] = (s >> 1);
        }
    }
}