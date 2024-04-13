setup_sam(void)
{
    krb5_context ctx = shandle.kdc_err_context;
    return krb5_c_make_random_key(ctx, ENCTYPE_DES_CBC_MD5, &psr_key);
}