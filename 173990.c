static unsigned int cnt_l_zeroes(const unsigned int bits)
{
    spice_extra_assert(bits != 0);
#if defined(__GNUC__) && __GNUC__ >= 4
    return __builtin_clz(bits);
#else
    if (bits & 0xff800000) {
        return lzeroes[bits >> 24];
    } else if (bits & 0xffff8000) {
        return 8 + lzeroes[(bits >> 16) & 0x000000ff];
    } else if (bits & 0xffffff80) {
        return 16 + lzeroes[(bits >> 8) & 0x000000ff];
    } else {
        return 24 + lzeroes[bits & 0x000000ff];
    }
#endif
}