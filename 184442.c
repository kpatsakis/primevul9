static int read_channel_info (WavpackContext *wpc, WavpackMetadata *wpmd)
{
    int bytecnt = wpmd->byte_length, shift = 0, mask_bits;
    unsigned char *byteptr = (unsigned char *)wpmd->data;
    uint32_t mask = 0;

    if (!bytecnt || bytecnt > 7)
        return FALSE;

    if (!wpc->config.num_channels) {

        // if bytecnt is 6 or 7 we are using new configuration with "unlimited" streams

        if (bytecnt >= 6) {
            wpc->config.num_channels = (byteptr [0] | ((byteptr [2] & 0xf) << 8)) + 1;
            wpc->max_streams = (byteptr [1] | ((byteptr [2] & 0xf0) << 4)) + 1;

            if (wpc->config.num_channels < wpc->max_streams)
                return FALSE;

            byteptr += 3;
            mask = *byteptr++;
            mask |= (uint32_t) *byteptr++ << 8;
            mask |= (uint32_t) *byteptr++ << 16;

            if (bytecnt == 7)                           // this was introduced in 5.0
                mask |= (uint32_t) *byteptr << 24;
        }
        else {
            wpc->config.num_channels = *byteptr++;

            while (--bytecnt) {
                mask |= (uint32_t) *byteptr++ << shift;
                shift += 8;
            }
        }

        if (wpc->config.num_channels > wpc->max_streams * 2)
            return FALSE;

        wpc->config.channel_mask = mask;

        for (mask_bits = 0; mask; mask >>= 1)
            if ((mask & 1) && ++mask_bits > wpc->config.num_channels)
                return FALSE;
    }

    return TRUE;
}