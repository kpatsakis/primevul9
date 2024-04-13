bjc_put_raster_skip(gp_file *file, int skip)
{
    bjc_put_command(file, 'e', 2);
    bjc_put_hi_lo(file, skip);
}