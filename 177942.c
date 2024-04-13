bjc_put_lo_hi(gp_file *file, int value)
{
    gp_fputc(value & 0xff, file);
    gp_fputc(((value & 0xffff) >> 8), file);
}