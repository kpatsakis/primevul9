yang_free_ext_data(struct yang_ext_substmt *substmt)
{
    int i;

    if (!substmt) {
        return;
    }

    free(substmt->ext_substmt);
    if (substmt->ext_modules) {
        for (i = 0; substmt->ext_modules[i]; ++i) {
            free(substmt->ext_modules[i]);
        }
        free(substmt->ext_modules);
    }
    free(substmt);
}