yang_fill_include(struct lys_module *trg, char *value, struct lys_include *inc,
                  struct unres_schema *unres)
{
    const char *str;
    int rc;
    int ret = 0;

    str = lydict_insert_zc(trg->ctx, value);
    rc = lyp_check_include(trg, str, inc, unres);
    if (!rc) {
        /* success, copy the filled data into the final array */
        memcpy(&trg->inc[trg->inc_size], inc, sizeof *inc);
        if (yang_check_ext_instance(trg, &trg->inc[trg->inc_size].ext, trg->inc[trg->inc_size].ext_size,
                                    &trg->inc[trg->inc_size], unres)) {
            ret = -1;
        }
        trg->inc_size++;
    } else if (rc == -1) {
        lys_extension_instances_free(trg->ctx, inc->ext, inc->ext_size, NULL);
        ret = -1;
    }

    lydict_remove(trg->ctx, str);
    return ret;
}