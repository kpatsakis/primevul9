HIDDEN void *zlib_init()
{
    z_stream *zstrm = xzmalloc(sizeof(z_stream));

    /* Always use gzip format because IE incorrectly uses raw deflate */
    if (deflateInit2(zstrm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                     16+MAX_WBITS /* gzip */,
                     MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY) != Z_OK) {
        free(zstrm);
        return NULL;
    }
    else {
        accept_encodings |= CE_DEFLATE | CE_GZIP;
        return zstrm;
    }
}