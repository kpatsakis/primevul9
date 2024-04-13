bjc_put_bytes(gp_file *file, const byte *data, int count)
{
    gp_fwrite(data, count, 1, file);
}