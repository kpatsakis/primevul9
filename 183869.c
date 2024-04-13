yang_check_identities(struct lys_module *module, struct unres_schema *unres)
{
    uint32_t i, size, base_size;
    uint8_t j;

    size = module->ident_size;
    module->ident_size = 0;
    for (i = 0; i < size; ++i) {
        base_size = module->ident[i].base_size;
        module->ident[i].base_size = 0;
        for (j = 0; j < base_size; ++j) {
            if (yang_read_base(module, &module->ident[i], (char *)module->ident[i].base[j], unres)) {
                ++j;
                module->ident_size = size;
                goto error;
            }
        }
        module->ident_size++;
        if (yang_check_iffeatures(module, NULL, &module->ident[i], IDENTITY_KEYWORD, unres)) {
            goto error;
        }
        if (yang_check_ext_instance(module, &module->ident[i].ext, module->ident[i].ext_size, &module->ident[i], unres)) {
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    for (; j< module->ident[i].base_size; ++j) {
        free(module->ident[i].base[j]);
    }
    yang_free_ident_base(module->ident, i + 1, size);
    return EXIT_FAILURE;
}