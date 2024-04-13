epsc_output_run(byte * data, int count, int y_mult,
                char start_graphics, gp_file * prn_stream, int pass)
{
    int xcount = count / y_mult;

    gp_fputc(033, prn_stream);
    if (!(start_graphics & ~3)) {
        gp_fputc("KLYZ"[(int)start_graphics], prn_stream);
    } else {
        gp_fputc('*', prn_stream);
        gp_fputc(start_graphics & ~DD, prn_stream);
    }
    gp_fputc(xcount & 0xff, prn_stream);
    gp_fputc(xcount >> 8, prn_stream);
    if (!pass)
        gp_fwrite((char *)data, 1, count, prn_stream);
    else {                      /* Only write every other column of y_mult bytes. */
        int which = pass;
        byte *dp = data;
        register int i, j;

        for (i = 0; i < xcount; i++, which++)
            for (j = 0; j < y_mult; j++, dp++) {
                gp_fputc(((which & 1) ? *dp : 0), prn_stream);
            }
    }
}