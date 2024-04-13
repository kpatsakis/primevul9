bjc_put_image_format(gp_file *file, char depth, char format, char ink)
{
    bjc_put_command(file, 't', 3);
    gp_fputc(depth, file);
    gp_fputc(format, file);
    gp_fputc(ink, file);
}