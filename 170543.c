static int sieve_find_script(const char *user, const char *domain,
                             const char *script, char *fname, size_t size)
{
    char *ext = NULL;

    if (!user && !script) {
        return -1;
    }

    if (user && strlen(user) > 900) {
        return -1;
    }

    if (sieve_usehomedir && user) { /* look in homedir */
        struct passwd *pent = getpwnam(user);

        if (pent == NULL) {
            return -1;
        }

        /* check ~USERNAME/.sieve */
        snprintf(fname, size, "%s/%s", pent->pw_dir, script ? script : ".sieve");
    } else { /* look in sieve_dir */
        size_t len = strlcpy(fname, sieve_dir, size);

        if (domain) {
            char dhash = (char) dir_hash_c(domain, config_fulldirhash);
            len += snprintf(fname+len, size-len, "%s%c/%s",
                            FNAME_DOMAINDIR, dhash, domain);
        }

        if (!user) { /* global script */
            len = strlcat(fname, "/global/", size);
        }
        else {
            char hash = (char) dir_hash_c(user, config_fulldirhash);
            len += snprintf(fname+len, size-len, "/%c/%s/", hash, user);

            if (!script) { /* default script */
                char *bc_fname;

                strlcat(fname, "defaultbc", size);

                bc_fname = sieve_getdefaultbcfname(fname);
                if (bc_fname) {
                    sieve_rebuild(NULL, bc_fname, 0, NULL);
                    free(bc_fname);
                }

                return 0;
            }
        }

        snprintf(fname+len, size-len, "%s.bc", script);
    }

    /* don't do this for ~username ones */
    ext = strrchr(fname, '.');
    if (ext && !strcmp(ext, ".bc"))
        sieve_rebuild(NULL, fname, 0, NULL);

    return 0;
}