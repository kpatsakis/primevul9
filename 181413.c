static VariLenAffix affixForIndex(uint64_t index)
{
    VariLenAffix prefix;
    prefix = expGolombEncode(index, EXP_GOLOMB_K);
    return invertAffix(&prefix); /* convert prefix to suffix */
}