bjc_put_hi_lo(gp_file *file, int value)
{
    gp_fputc(((value & 0xffff) >> 8), file);
    gp_fputc(value & 0xff, file);
}