yang_read_ext(struct lys_module *module, void *actual, char *ext_name, char *ext_arg,
              enum yytokentype actual_type, enum yytokentype backup_type, int is_ext_instance)
{
    struct lys_ext_instance *instance;
    LY_STMT stmt = LY_STMT_UNKNOWN;
    LYEXT_SUBSTMT insubstmt;
    uint8_t insubstmt_index = 0;

    if (backup_type != NODE) {
        switch (actual_type) {
        case YANG_VERSION_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_VERSION;
            stmt = LY_STMT_VERSION;
            break;
        case NAMESPACE_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_NAMESPACE;
            stmt = LY_STMT_NAMESPACE;
            break;
        case PREFIX_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_PREFIX;
            stmt = LY_STMT_PREFIX;
            break;
        case REVISION_DATE_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_REVISIONDATE;
            stmt = LY_STMT_REVISIONDATE;
            break;
        case DESCRIPTION_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_DESCRIPTION;
            stmt = LY_STMT_DESCRIPTION;
            break;
        case REFERENCE_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_REFERENCE;
            stmt = LY_STMT_REFERENCE;
            break;
        case CONTACT_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_CONTACT;
            stmt = LY_STMT_CONTACT;
            break;
        case ORGANIZATION_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_ORGANIZATION;
            stmt = LY_STMT_ORGANIZATION;
            break;
        case YIN_ELEMENT_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_YINELEM;
            stmt = LY_STMT_YINELEM;
            break;
        case STATUS_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_STATUS;
            stmt = LY_STMT_STATUS;
            break;
        case BASE_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_BASE;
            stmt = LY_STMT_BASE;
            if (backup_type == IDENTITY_KEYWORD) {
                insubstmt_index = ((struct lys_ident *)actual)->base_size;
            } else if (backup_type == TYPE_KEYWORD) {
                insubstmt_index = ((struct yang_type *)actual)->type->info.ident.count;
            }
            break;
        case DEFAULT_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_DEFAULT;
            stmt = LY_STMT_DEFAULT;
            switch (backup_type) {
            case LEAF_LIST_KEYWORD:
                insubstmt_index = ((struct lys_node_leaflist *)actual)->dflt_size;
                break;
            case REFINE_KEYWORD:
                insubstmt_index = ((struct lys_refine *)actual)->dflt_size;
                break;
            case ADD_KEYWORD:
                insubstmt_index = ((struct lys_deviate *)actual)->dflt_size;
                break;
            default:
                /* nothing changes */
                break;
            }
            break;
        case UNITS_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_UNITS;
            stmt = LY_STMT_UNITS;
            break;
        case REQUIRE_INSTANCE_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_REQINSTANCE;
            stmt = LY_STMT_REQINSTANCE;
            break;
        case PATH_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_PATH;
            stmt = LY_STMT_PATH;
            break;
        case ERROR_MESSAGE_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_ERRMSG;
            stmt = LY_STMT_ERRMSG;
            break;
        case ERROR_APP_TAG_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_ERRTAG;
            stmt = LY_STMT_ERRTAG;
            break;
        case MODIFIER_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_MODIFIER;
            stmt = LY_STMT_MODIFIER;
            break;
        case FRACTION_DIGITS_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_DIGITS;
            stmt = LY_STMT_DIGITS;
            break;
        case VALUE_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_VALUE;
            stmt = LY_STMT_VALUE;
            break;
        case POSITION_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_POSITION;
            stmt = LY_STMT_POSITION;
            break;
        case PRESENCE_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_PRESENCE;
            stmt = LY_STMT_PRESENCE;
            break;
        case CONFIG_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_CONFIG;
            stmt = LY_STMT_CONFIG;
            break;
        case MANDATORY_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_MANDATORY;
            stmt = LY_STMT_MANDATORY;
            break;
        case MIN_ELEMENTS_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_MIN;
            stmt = LY_STMT_MIN;
            break;
        case MAX_ELEMENTS_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_MAX;
            stmt = LY_STMT_MAX;
            break;
        case ORDERED_BY_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_ORDEREDBY;
            stmt = LY_STMT_ORDEREDBY;
            break;
        case KEY_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_KEY;
            stmt = LY_STMT_KEY;
            break;
        case UNIQUE_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_UNIQUE;
            stmt = LY_STMT_UNIQUE;
            switch (backup_type) {
            case LIST_KEYWORD:
                insubstmt_index = ((struct lys_node_list *)actual)->unique_size;
                break;
            case ADD_KEYWORD:
            case DELETE_KEYWORD:
            case REPLACE_KEYWORD:
                insubstmt_index = ((struct lys_deviate *)actual)->unique_size;
                break;
            default:
                /* nothing changes */
                break;
            }
            break;
        default:
            LOGINT(module->ctx);
            return NULL;
        }

        instance = yang_ext_instance(actual, backup_type, is_ext_instance);
    } else {
        switch (actual_type) {
        case ARGUMENT_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_ARGUMENT;
            stmt = LY_STMT_ARGUMENT;
            break;
        case BELONGS_TO_KEYWORD:
            insubstmt = LYEXT_SUBSTMT_BELONGSTO;
            stmt = LY_STMT_BELONGSTO;
            break;
        default:
            insubstmt = LYEXT_SUBSTMT_SELF;
            break;
        }

        instance = yang_ext_instance(actual, actual_type, is_ext_instance);
    }

    if (!instance) {
        return NULL;
    }
    instance->insubstmt = insubstmt;
    instance->insubstmt_index = insubstmt_index;
    instance->flags |= LYEXT_OPT_YANG;
    instance->def = (struct lys_ext *)ext_name;    /* hack for UNRES */
    instance->arg_value = lydict_insert_zc(module->ctx, ext_arg);
    if (is_ext_instance && stmt != LY_STMT_UNKNOWN && instance->parent_type == LYEXT_PAR_EXTINST) {
        instance->insubstmt_index = yang_fill_ext_substm_index(actual, stmt, backup_type);
    }
    return instance;
}