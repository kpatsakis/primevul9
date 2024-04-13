static int filter_generate(struct tree *xfm, const char *root,
                           int *nmatches, char ***matches) {
    glob_t globbuf;
    int gl_flags = glob_flags;
    int r;
    int ret = 0;
    char **pathv = NULL;
    int pathc = 0;
    int root_prefix = strlen(root) - 1;

    *nmatches = 0;
    *matches = NULL;
    MEMZERO(&globbuf, 1);

    list_for_each(f, xfm->children) {
        char *globpat = NULL;
        if (! is_incl(f))
            continue;
        pathjoin(&globpat, 2, root, f->value);
        r = glob(globpat, gl_flags, NULL, &globbuf);
        free(globpat);

        if (r != 0 && r != GLOB_NOMATCH)
            goto error;
        gl_flags |= GLOB_APPEND;
    }

    pathc = globbuf.gl_pathc;
    int pathind = 0;

    if (ALLOC_N(pathv, pathc) < 0)
        goto error;

    for (int i=0; i < pathc; i++) {
        const char *path = globbuf.gl_pathv[i] + root_prefix;
        bool include = true;

        list_for_each(e, xfm->children) {
            if (! is_excl(e))
                continue;

            if (strchr(e->value, SEP) == NULL)
                path = pathbase(path);
            if ((r = fnmatch(e->value, path, fnm_flags)) == 0) {
                include = false;
            }
        }

        if (include)
            include = is_regular_file(globbuf.gl_pathv[i]);

        if (include) {
            pathv[pathind] = strdup(globbuf.gl_pathv[i]);
            if (pathv[pathind] == NULL)
                goto error;
            pathind += 1;
        }
    }
    pathc = pathind;

    if (REALLOC_N(pathv, pathc) == -1)
        goto error;

    *matches = pathv;
    *nmatches = pathc;
 done:
    globfree(&globbuf);
    return ret;
 error:
    if (pathv != NULL)
        for (int i=0; i < pathc; i++)
            free(pathv[i]);
    free(pathv);
    ret = -1;
    goto done;
}