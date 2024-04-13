yang_check_iffeatures(struct lys_module *module, void *ptr, void *parent, enum yytokentype type, struct unres_schema *unres)
{
    struct lys_iffeature *iffeature;
    uint8_t *ptr_size, size, i;
    char *s;
    int parent_is_feature = 0;

    switch (type) {
    case FEATURE_KEYWORD:
        iffeature = ((struct lys_feature *)parent)->iffeature;
        size = ((struct lys_feature *)parent)->iffeature_size;
        ptr_size = &((struct lys_feature *)parent)->iffeature_size;
        parent_is_feature = 1;
        break;
    case IDENTITY_KEYWORD:
        iffeature = ((struct lys_ident *)parent)->iffeature;
        size = ((struct lys_ident *)parent)->iffeature_size;
        ptr_size = &((struct lys_ident *)parent)->iffeature_size;
        break;
    case ENUM_KEYWORD:
        iffeature = ((struct lys_type_enum *)ptr)->iffeature;
        size = ((struct lys_type_enum *)ptr)->iffeature_size;
        ptr_size = &((struct lys_type_enum *)ptr)->iffeature_size;
        break;
    case BIT_KEYWORD:
        iffeature = ((struct lys_type_bit *)ptr)->iffeature;
        size = ((struct lys_type_bit *)ptr)->iffeature_size;
        ptr_size = &((struct lys_type_bit *)ptr)->iffeature_size;
        break;
    case REFINE_KEYWORD:
        iffeature = ((struct lys_refine *)ptr)->iffeature;
        size = ((struct lys_refine *)ptr)->iffeature_size;
        ptr_size = &((struct lys_refine *)ptr)->iffeature_size;
        break;
    default:
        iffeature = ((struct lys_node *)parent)->iffeature;
        size = ((struct lys_node *)parent)->iffeature_size;
        ptr_size = &((struct lys_node *)parent)->iffeature_size;
        break;
    }

    *ptr_size = 0;
    for (i = 0; i < size; ++i) {
        s = (char *)iffeature[i].features;
        iffeature[i].features = NULL;
        if (yang_fill_iffeature(module, &iffeature[i], parent, s, unres, parent_is_feature)) {
            *ptr_size = size;
            return EXIT_FAILURE;
        }
        if (yang_check_ext_instance(module, &iffeature[i].ext, iffeature[i].ext_size, &iffeature[i], unres)) {
            *ptr_size = size;
            return EXIT_FAILURE;
        }
        (*ptr_size)++;
    }

    return EXIT_SUCCESS;
}