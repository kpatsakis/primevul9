static int init_wv_bitstream (WavpackStream *wps, WavpackMetadata *wpmd)
{
    if (!wpmd->byte_length || (wpmd->byte_length & 1))
        return FALSE;

    bs_open_read (&wps->wvbits, wpmd->data, (unsigned char *) wpmd->data + wpmd->byte_length);
    return TRUE;
}