ImagingJpeg2KDecode(Imaging im, ImagingCodecState state, UINT8 *buf, Py_ssize_t bytes) {
    if (bytes) {
        state->errcode = IMAGING_CODEC_BROKEN;
        state->state = J2K_STATE_FAILED;
        return -1;
    }

    if (state->state == J2K_STATE_DONE || state->state == J2K_STATE_FAILED) {
        return -1;
    }

    if (state->state == J2K_STATE_START) {
        state->state = J2K_STATE_DECODING;

        return j2k_decode_entry(im, state);
    }

    if (state->state == J2K_STATE_DECODING) {
        state->errcode = IMAGING_CODEC_BROKEN;
        state->state = J2K_STATE_FAILED;
        return -1;
    }
    return -1;
}