yang_fill_type(struct lys_module *module, struct lys_type *type, struct yang_type *stype,
               void *parent, struct unres_schema *unres)
{
    unsigned int i, j;

    type->parent = parent;
    if (yang_check_ext_instance(module, &type->ext, type->ext_size, type, unres)) {
        return EXIT_FAILURE;
    }
    for (j = 0; j < type->ext_size; ++j) {
        if (type->ext[j]->flags & LYEXT_OPT_VALID) {
            type->parent->flags |= LYS_VALID_EXT;
            break;
        }
    }

    switch (stype->base) {
    case LY_TYPE_ENUM:
        for (i = 0; i < type->info.enums.count; ++i) {
            if (yang_check_iffeatures(module, &type->info.enums.enm[i], parent, ENUM_KEYWORD, unres)) {
                return EXIT_FAILURE;
            }
            if (yang_check_ext_instance(module, &type->info.enums.enm[i].ext, type->info.enums.enm[i].ext_size,
                                        &type->info.enums.enm[i], unres)) {
                return EXIT_FAILURE;
            }
            for (j = 0; j < type->info.enums.enm[i].ext_size; ++j) {
                if (type->info.enums.enm[i].ext[j]->flags & LYEXT_OPT_VALID) {
                    type->parent->flags |= LYS_VALID_EXT;
                    break;
                }
            }
        }
        break;
    case LY_TYPE_BITS:
        for (i = 0; i < type->info.bits.count; ++i) {
            if (yang_check_iffeatures(module, &type->info.bits.bit[i], parent, BIT_KEYWORD, unres)) {
                return EXIT_FAILURE;
            }
            if (yang_check_ext_instance(module, &type->info.bits.bit[i].ext, type->info.bits.bit[i].ext_size,
                                        &type->info.bits.bit[i], unres)) {
                return EXIT_FAILURE;
            }
            for (j = 0; j < type->info.bits.bit[i].ext_size; ++j) {
                if (type->info.bits.bit[i].ext[j]->flags & LYEXT_OPT_VALID) {
                    type->parent->flags |= LYS_VALID_EXT;
                    break;
                }
            }
        }
        break;
    case LY_TYPE_IDENT:
        if (yang_check_identityref(module, type, unres)) {
            return EXIT_FAILURE;
        }
        break;
    case LY_TYPE_STRING:
        if (type->info.str.length) {
            if (yang_check_ext_instance(module, &type->info.str.length->ext,
                                        type->info.str.length->ext_size, type->info.str.length, unres)) {
                return EXIT_FAILURE;
            }
            for (j = 0; j < type->info.str.length->ext_size; ++j) {
                if (type->info.str.length->ext[j]->flags & LYEXT_OPT_VALID) {
                    type->parent->flags |= LYS_VALID_EXT;
                    break;
                }
            }
        }

        for (i = 0; i < type->info.str.pat_count; ++i) {
            if (yang_check_ext_instance(module, &type->info.str.patterns[i].ext, type->info.str.patterns[i].ext_size,
                                        &type->info.str.patterns[i], unres)) {
                return EXIT_FAILURE;
            }
            for (j = 0; j < type->info.str.patterns[i].ext_size; ++j) {
                if (type->info.str.patterns[i].ext[j]->flags & LYEXT_OPT_VALID) {
                    type->parent->flags |= LYS_VALID_EXT;
                    break;
                }
            }
        }
        break;
    case LY_TYPE_DEC64:
        if (type->info.dec64.range) {
            if (yang_check_ext_instance(module, &type->info.dec64.range->ext,
                                        type->info.dec64.range->ext_size, type->info.dec64.range, unres)) {
                return EXIT_FAILURE;
            }
            for (j = 0; j < type->info.dec64.range->ext_size; ++j) {
                if (type->info.dec64.range->ext[j]->flags & LYEXT_OPT_VALID) {
                    type->parent->flags |= LYS_VALID_EXT;
                    break;
                }
            }
        }
        break;
    case LY_TYPE_UNION:
        for (i = 0; i < type->info.uni.count; ++i) {
            if (yang_fill_type(module, &type->info.uni.types[i], (struct yang_type *)type->info.uni.types[i].der,
                               parent, unres)) {
                return EXIT_FAILURE;
            }
        }
        break;
    default:
        /* nothing checks */
        break;
    }
    return EXIT_SUCCESS;
}