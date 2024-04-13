bjc_put_initialize(gp_file *file)
{
    bjc_put_bytes(file, (const byte *)"\033@", 2);
}