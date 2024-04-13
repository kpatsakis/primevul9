bjc_put_print_method(gp_file *file, char color, char media, char quality,
                     char density)
{
    bjc_put_command(file, 'c', 2 + (density != 0));
    gp_fputc(color, file);
    gp_fputc(media | quality, file);
    if (density)
        gp_fputc(density, file);
}