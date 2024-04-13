bjc_put_continue_image(gp_file *file, const byte *data, int count)
{
    bjc_put_command(file, 'F', count);
    bjc_put_bytes(file, data, count);
}