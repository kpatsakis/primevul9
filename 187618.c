static void zlib_done(z_stream *zstrm)
{
    if (zstrm) {
        deflateEnd(zstrm);
        free(zstrm);
    }
}