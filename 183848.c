yang_check_must(struct lys_module *module, struct lys_restr *must, uint size, struct unres_schema *unres)
{
    uint i;

    for (i = 0; i < size; ++i) {
        if (yang_check_ext_instance(module, &must[i].ext, must[i].ext_size, &must[i], unres)) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}