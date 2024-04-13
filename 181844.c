lyp_sort_revisions(struct lys_module *module)
{
    uint8_t i, r;
    struct lys_revision rev;

    for (i = 1, r = 0; i < module->rev_size; i++) {
        if (strcmp(module->rev[i].date, module->rev[r].date) > 0) {
            r = i;
        }
    }

    if (r) {
        /* the newest revision is not on position 0, switch them */
        memcpy(&rev, &module->rev[0], sizeof rev);
        memcpy(&module->rev[0], &module->rev[r], sizeof rev);
        memcpy(&module->rev[r], &rev, sizeof rev);
    }
}