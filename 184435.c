static int read_sample_rate (WavpackContext *wpc, WavpackMetadata *wpmd)
{
    int bytecnt = wpmd->byte_length;
    unsigned char *byteptr = (unsigned char *)wpmd->data;

    if (bytecnt == 3 || bytecnt == 4) {
        wpc->config.sample_rate = (int32_t) *byteptr++;
        wpc->config.sample_rate |= (int32_t) *byteptr++ << 8;
        wpc->config.sample_rate |= (int32_t) *byteptr++ << 16;

        // for sampling rates > 16777215 (non-audio probably, or ...)

        if (bytecnt == 4)
            wpc->config.sample_rate |= (int32_t) (*byteptr & 0x7f) << 24;
    }

    return TRUE;
}