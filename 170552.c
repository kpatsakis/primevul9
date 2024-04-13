void sieve_srs_free(void)
{
    if (srs_engine) {
        srs_free(srs_engine);
        srs_engine = NULL;
    }
}