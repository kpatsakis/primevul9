yang_ext_instance(void *node, enum yytokentype type, int is_ext_instance)
{
    struct lys_ext_instance ***ext, **tmp, *instance = NULL;
    LYEXT_PAR parent_type;
    uint8_t *size;

    switch (type) {
    case MODULE_KEYWORD:
    case SUBMODULE_KEYWORD:
        ext = &((struct lys_module *)node)->ext;
        size = &((struct lys_module *)node)->ext_size;
        parent_type = LYEXT_PAR_MODULE;
        break;
    case BELONGS_TO_KEYWORD:
        if (is_ext_instance) {
            ext = &((struct lys_ext_instance *)node)->ext;
            size = &((struct lys_ext_instance *)node)->ext_size;
            parent_type = LYEXT_PAR_EXTINST;
        } else {
            ext = &((struct lys_module *)node)->ext;
            size = &((struct lys_module *)node)->ext_size;
            parent_type = LYEXT_PAR_MODULE;
        }
        break;
    case IMPORT_KEYWORD:
        ext = &((struct lys_import *)node)->ext;
        size = &((struct lys_import *)node)->ext_size;
        parent_type = LYEXT_PAR_IMPORT;
        break;
    case INCLUDE_KEYWORD:
        ext = &((struct lys_include *)node)->ext;
        size = &((struct lys_include *)node)->ext_size;
        parent_type = LYEXT_PAR_INCLUDE;
        break;
    case REVISION_KEYWORD:
        ext = &((struct lys_revision *)node)->ext;
        size = &((struct lys_revision *)node)->ext_size;
        parent_type = LYEXT_PAR_REVISION;
        break;
    case GROUPING_KEYWORD:
    case CONTAINER_KEYWORD:
    case LEAF_KEYWORD:
    case LEAF_LIST_KEYWORD:
    case LIST_KEYWORD:
    case CHOICE_KEYWORD:
    case CASE_KEYWORD:
    case ANYXML_KEYWORD:
    case ANYDATA_KEYWORD:
    case USES_KEYWORD:
    case AUGMENT_KEYWORD:
    case ACTION_KEYWORD:
    case RPC_KEYWORD:
    case INPUT_KEYWORD:
    case OUTPUT_KEYWORD:
    case NOTIFICATION_KEYWORD:
        ext = &((struct lys_node *)node)->ext;
        size = &((struct lys_node *)node)->ext_size;
        parent_type = LYEXT_PAR_NODE;
        break;
    case ARGUMENT_KEYWORD:
        if (is_ext_instance) {
            ext = &((struct lys_ext_instance *)node)->ext;
            size = &((struct lys_ext_instance *)node)->ext_size;
            parent_type = LYEXT_PAR_EXTINST;
        } else {
            ext = &((struct lys_ext *)node)->ext;
            size = &((struct lys_ext *)node)->ext_size;
            parent_type = LYEXT_PAR_EXT;
        }
        break;
    case EXTENSION_KEYWORD:
        ext = &((struct lys_ext *)node)->ext;
        size = &((struct lys_ext *)node)->ext_size;
        parent_type = LYEXT_PAR_EXT;
        break;
    case FEATURE_KEYWORD:
        ext = &((struct lys_feature *)node)->ext;
        size = &((struct lys_feature *)node)->ext_size;
        parent_type = LYEXT_PAR_FEATURE;
        break;
    case IDENTITY_KEYWORD:
        ext = &((struct lys_ident *)node)->ext;
        size = &((struct lys_ident *)node)->ext_size;
        parent_type = LYEXT_PAR_IDENT;
        break;
    case IF_FEATURE_KEYWORD:
        ext = &((struct lys_iffeature *)node)->ext;
        size = &((struct lys_iffeature *)node)->ext_size;
        parent_type = LYEXT_PAR_IFFEATURE;
        break;
    case TYPEDEF_KEYWORD:
        ext = &((struct lys_tpdf *)node)->ext;
        size = &((struct lys_tpdf *)node)->ext_size;
        parent_type = LYEXT_PAR_TPDF;
        break;
    case TYPE_KEYWORD:
        ext = &((struct yang_type *)node)->type->ext;
        size = &((struct yang_type *)node)->type->ext_size;
        parent_type = LYEXT_PAR_TYPE;
        break;
    case LENGTH_KEYWORD:
    case PATTERN_KEYWORD:
    case RANGE_KEYWORD:
    case MUST_KEYWORD:
        ext = &((struct lys_restr *)node)->ext;
        size = &((struct lys_restr *)node)->ext_size;
        parent_type = LYEXT_PAR_RESTR;
        break;
    case WHEN_KEYWORD:
        ext = &((struct lys_when *)node)->ext;
        size = &((struct lys_when *)node)->ext_size;
        parent_type = LYEXT_PAR_RESTR;
        break;
    case ENUM_KEYWORD:
        ext = &((struct lys_type_enum *)node)->ext;
        size = &((struct lys_type_enum *)node)->ext_size;
        parent_type = LYEXT_PAR_TYPE_ENUM;
        break;
    case BIT_KEYWORD:
        ext = &((struct lys_type_bit *)node)->ext;
        size = &((struct lys_type_bit *)node)->ext_size;
        parent_type = LYEXT_PAR_TYPE_BIT;
        break;
    case REFINE_KEYWORD:
        ext = &((struct lys_type_bit *)node)->ext;
        size = &((struct lys_type_bit *)node)->ext_size;
        parent_type = LYEXT_PAR_REFINE;
        break;
    case DEVIATION_KEYWORD:
        ext = &((struct lys_deviation *)node)->ext;
        size = &((struct lys_deviation *)node)->ext_size;
        parent_type = LYEXT_PAR_DEVIATION;
        break;
    case NOT_SUPPORTED_KEYWORD:
    case ADD_KEYWORD:
    case DELETE_KEYWORD:
    case REPLACE_KEYWORD:
        ext = &((struct lys_deviate *)node)->ext;
        size = &((struct lys_deviate *)node)->ext_size;
        parent_type = LYEXT_PAR_DEVIATE;
        break;
    case EXTENSION_INSTANCE:
        ext = &((struct lys_ext_instance *)node)->ext;
        size = &((struct lys_ext_instance *)node)->ext_size;
        parent_type = LYEXT_PAR_EXTINST;
        break;
    default:
        LOGINT(NULL);
        return NULL;
    }

    instance = calloc(1, sizeof *instance);
    if (!instance) {
        goto error;
    }
    instance->parent_type = parent_type;
    tmp = realloc(*ext, (*size + 1) * sizeof *tmp);
    if (!tmp) {
        goto error;
    }
    tmp[*size] = instance;
    *ext = tmp;
    (*size)++;
    return instance;

error:
    LOGMEM(NULL);
    free(instance);
    return NULL;
}