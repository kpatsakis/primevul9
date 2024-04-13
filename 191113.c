gs_main_lib_open(gs_main_instance * minst, const char *file_name, ref * pfile)
{
    /* This is a separate procedure only to avoid tying up */
    /* extra stack space while running the file. */
    i_ctx_t *i_ctx_p = minst->i_ctx_p;
#define maxfn 2048
    char fn[maxfn];
    uint len;

    return lib_file_open(&minst->lib_path, imemory,
                         NULL, /* Don't check permissions here, because permlist
                                  isn't ready running init files. */
                          file_name, strlen(file_name), fn, maxfn, &len, pfile);
}