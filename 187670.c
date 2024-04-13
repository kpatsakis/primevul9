HIDDEN void *brotli_init()
{
    BrotliEncoderState *brotli = BrotliEncoderCreateInstance(NULL, NULL, NULL);

    if (brotli) {
        BrotliEncoderSetParameter(brotli, BROTLI_PARAM_MODE,
                                  BROTLI_DEFAULT_MODE);
        BrotliEncoderSetParameter(brotli, BROTLI_PARAM_QUALITY,
                                  BROTLI_DEFAULT_QUALITY);
        BrotliEncoderSetParameter(brotli, BROTLI_PARAM_LGWIN,
                                  BROTLI_DEFAULT_WINDOW);
        BrotliEncoderSetParameter(brotli, BROTLI_PARAM_LGBLOCK,
                                  BROTLI_MAX_INPUT_BLOCK_BITS);
    }

    return brotli;
}