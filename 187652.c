static void brotli_done(BrotliEncoderState *brotli)
{
    if (brotli) BrotliEncoderDestroyInstance(brotli);
}