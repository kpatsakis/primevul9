ImagingJpeg2KDecodeCleanup(ImagingCodecState state) {
    JPEG2KDECODESTATE *context = (JPEG2KDECODESTATE *)state->context;

    if (context->error_msg) {
        free((void *)context->error_msg);
    }

    context->error_msg = NULL;

    return -1;
}