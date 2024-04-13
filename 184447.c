static int read_float_info (WavpackStream *wps, WavpackMetadata *wpmd)
{
    int bytecnt = wpmd->byte_length;
    char *byteptr = (char *)wpmd->data;

    if (bytecnt != 4)
        return FALSE;

    wps->float_flags = *byteptr++;
    wps->float_shift = *byteptr++;
    wps->float_max_exp = *byteptr++;
    wps->float_norm_exp = *byteptr;
    return TRUE;
}