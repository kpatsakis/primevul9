bjc_put_command(gp_file *file, char command, int count)
{   char tmp[3] = { '\033', '(', ' '};
    tmp[2] = command;
    gp_fwrite( tmp, 3, 1, file);
    bjc_put_lo_hi(file, count);
}