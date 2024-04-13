static int init_wvx_bitstream (WavpackStream *wps, WavpackMetadata *wpmd)
{
    unsigned char *cp = (unsigned char *)wpmd->data;

    if (wpmd->byte_length <= 4 || (wpmd->byte_length & 1))
        return FALSE;

    wps->crc_wvx = *cp++;
    wps->crc_wvx |= (uint32_t) *cp++ << 8;
    wps->crc_wvx |= (uint32_t) *cp++ << 16;
    wps->crc_wvx |= (uint32_t) *cp++ << 24;

    bs_open_read (&wps->wvxbits, cp, (unsigned char *) wpmd->data + wpmd->byte_length);
    return TRUE;
}