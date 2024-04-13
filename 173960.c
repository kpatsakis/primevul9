SPICE_CONSTRUCTOR_FUNC(quic_global_init)
{
    family_init(&family_8bpc, 8, DEFmaxclen);
    family_init(&family_5bpc, 5, DEFmaxclen);
}