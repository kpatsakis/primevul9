static int read_int32_info (WavpackStream *wps, WavpackMetadata *wpmd)
{
    int bytecnt = wpmd->byte_length;
    char *byteptr = (char *)wpmd->data;

    if (bytecnt != 4)
        return FALSE;

    wps->int32_sent_bits = *byteptr++;
    wps->int32_zeros = *byteptr++;
    wps->int32_ones = *byteptr++;
    wps->int32_dups = *byteptr;

    return TRUE;
}