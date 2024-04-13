bjc_put_cmyk_image(gp_file *file, char component,
                   const byte *data, int count)
{
    bjc_put_command(file, 'A', count + 1);
    gp_fputc(component, file);
    bjc_put_bytes(file, data, count);
}