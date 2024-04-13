static VariLenAffix expGolombEncode(uint64_t n, int k)
{
    const uint64_t value = n + (1 << k) - 1;
    const int bits = (int) log2(value) + 1;
    return (VariLenAffix) {
        .type = AffixType_Prefix,
        .value = value,
        .bits = bits + MAX((bits - 1 - k), 0)
    };
}