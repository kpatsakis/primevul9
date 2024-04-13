static void dump_file_info (WavpackContext *wpc, char *name, FILE *dst, int parameter)
{
    char str [80];
    int item_id;

    str [0] = 0;

    if (parameter == 0) {
        for (item_id = 1; item_id <= 9; ++item_id) {
            dump_file_item (wpc, str, item_id);
            strcat (str, ";");
        }

        if (name && *name != '-')
            fprintf (dst, "%s%s\n", str, name);
        else
            fprintf (dst, "%s\n", str);
    }
    else if (parameter < 10) {
        dump_file_item (wpc, str, parameter);
        fprintf (dst, "%s\n", str);
    }
    else if (parameter == 10 && name && *name != '-')
        fprintf (dst, "%s\n", name);
    else
        fprintf (dst, "\n");
}