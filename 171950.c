MONGO_EXPORT void bson_swap_endian32( void *outp, const void *inp ) {
    const char *in = ( const char * )inp;
    char *out = ( char * )outp;

    out[0] = in[3];
    out[1] = in[2];
    out[2] = in[1];
    out[3] = in[0];
}