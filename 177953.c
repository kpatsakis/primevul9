bjc_put_raster_resolution(gp_file *file, int x_resolution, int y_resolution)
{
    if (x_resolution == y_resolution) {
        bjc_put_command(file, 'd', 2);
    } else {
        bjc_put_command(file, 'd', 4);
        bjc_put_hi_lo(file, y_resolution);
    }
    bjc_put_hi_lo(file, x_resolution);
}