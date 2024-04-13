wc_is_ucs_alnum(wc_uint32 ucs)
{
    return (wc_is_ucs_alpha(ucs) || wc_is_ucs_digit(ucs));
}