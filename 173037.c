static int filter_matches(struct tree *xfm, const char *path) {
    int found = 0;
    list_for_each(f, xfm->children) {
        if (is_incl(f) && fnmatch(f->value, path, fnm_flags) == 0) {
            found = 1;
            break;
        }
    }
    if (! found)
        return 0;
    list_for_each(f, xfm->children) {
        if (is_excl(f) && (fnmatch(f->value, path, fnm_flags) == 0))
            return 0;
    }
    return 1;
}