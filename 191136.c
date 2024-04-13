static char *gs_main_tempnames(gs_main_instance *minst)
{
    i_ctx_t *i_ctx_p = minst->i_ctx_p;
    ref *SAFETY;
    ref *tempfiles;
    ref keyval[2];      /* for key and value */
    char *tempnames = NULL;
    int i;
    int idict;
    int len = 0;
    const byte *data = NULL;
    uint size;
    if (minst->init_done >= 2) {
        if (dict_find_string(systemdict, "SAFETY", &SAFETY) <= 0 ||
            dict_find_string(SAFETY, "tempfiles", &tempfiles) <= 0)
            return NULL;
        /* get lengths of temporary filenames */
        idict = dict_first(tempfiles);
        while ((idict = dict_next(tempfiles, idict, &keyval[0])) >= 0) {
            if (obj_string_data(minst->heap, &keyval[0], &data, &size) >= 0)
                len += size + 1;
        }
        if (len != 0)
            tempnames = (char *)malloc(len+1);
        if (tempnames) {
            memset(tempnames, 0, len+1);
            /* copy temporary filenames */
            idict = dict_first(tempfiles);
            i = 0;
            while ((idict = dict_next(tempfiles, idict, &keyval[0])) >= 0) {
                if (obj_string_data(minst->heap, &keyval[0], &data, &size) >= 0) {
                    memcpy(tempnames+i, (const char *)data, size);
                    i+= size;
                    tempnames[i++] = '\0';
                }
            }
        }
    }
    return tempnames;
}