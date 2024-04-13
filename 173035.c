int transform_validate(struct augeas *aug, struct tree *xfm) {
    struct tree *l = NULL;

    for (struct tree *t = xfm->children; t != NULL; ) {
        if (streqv(t->label, "lens")) {
            l = t;
        } else if ((is_incl(t) || (is_excl(t) && strchr(t->value, SEP) != NULL))
                       && t->value[0] != SEP) {
            /* Normalize relative paths to absolute ones */
            int r;
            r = REALLOC_N(t->value, strlen(t->value) + 2);
            ERR_NOMEM(r < 0, aug);
            memmove(t->value + 1, t->value, strlen(t->value) + 1);
            t->value[0] = SEP;
        }

        if (streqv(t->label, "error")) {
            struct tree *del = t;
            t = del->next;
            tree_unlink(del);
        } else {
            t = t->next;
        }
    }

    if (l == NULL) {
        xfm_error(xfm, "missing a child with label 'lens'");
        return -1;
    }
    if (l->value == NULL) {
        xfm_error(xfm, "the 'lens' node does not contain a lens name");
        return -1;
    }
    lens_from_name(aug, l->value);
    ERR_BAIL(aug);

    return 0;
 error:
    xfm_error(xfm, aug->error->details);
    return -1;
}