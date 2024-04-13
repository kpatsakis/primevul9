static VariLenAffix invertAffix(const VariLenAffix *affix)
{
    return (VariLenAffix) {
        .type =
            (affix->type == AffixType_Suffix) ?
                AffixType_Prefix : AffixType_Suffix,
        .value =
            mirror64bit(affix->value) >>
            ((sizeof(affix->value) * 8) - affix->bits),
        .bits = affix->bits
    };
}