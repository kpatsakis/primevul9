yang_check_type(struct lys_module *module, struct lys_node *parent, struct yang_type *typ, struct lys_type *type, int tpdftype, struct unres_schema *unres)
{
    struct ly_ctx *ctx = module->ctx;
    int rc, ret = -1;
    unsigned int i, j;
    int8_t req;
    const char *name, *value, *module_name = NULL;
    LY_DATA_TYPE base = 0, base_tmp;
    struct lys_node *siter;
    struct lys_type *dertype;
    struct lys_type_enum *enms_sc = NULL;
    struct lys_type_bit *bits_sc = NULL;
    struct lys_type_bit bit_tmp;
    struct yang_type *yang;

    value = transform_schema2json(module, typ->name);
    if (!value) {
        goto error;
    }

    i = parse_identifier(value);
    if (i < 1) {
        LOGVAL(ctx, LYE_INCHAR, LY_VLOG_NONE, NULL, value[-i], &value[-i]);
        lydict_remove(ctx, value);
        goto error;
    }
    /* module name */
    name = value;
    if (value[i]) {
        module_name = lydict_insert(ctx, value, i);
        name += i;
        if ((name[0] != ':') || (parse_identifier(name + 1) < 1)) {
            LOGVAL(ctx, LYE_INCHAR, LY_VLOG_NONE, NULL, name[0], name);
            lydict_remove(ctx, module_name);
            lydict_remove(ctx, value);
            goto error;
        }
        ++name;
    }

    rc = resolve_superior_type(name, module_name, module, parent, &type->der);
    if (rc == -1) {
        LOGVAL(ctx, LYE_INMOD, LY_VLOG_NONE, NULL, module_name);
        lydict_remove(ctx, module_name);
        lydict_remove(ctx, value);
        goto error;

    /* the type could not be resolved or it was resolved to an unresolved typedef or leafref */
    } else if (rc == EXIT_FAILURE) {
        LOGVAL(ctx, LYE_NORESOLV, LY_VLOG_NONE, NULL, "type", name);
        lydict_remove(ctx, module_name);
        lydict_remove(ctx, value);
        ret = EXIT_FAILURE;
        goto error;
    }
    lydict_remove(ctx, module_name);
    lydict_remove(ctx, value);

    if (type->value_flags & LY_VALUE_UNRESGRP) {
        /* resolved type in grouping, decrease the grouping's nacm number to indicate that one less
         * unresolved item left inside the grouping, LYTYPE_GRP used as a flag for types inside a grouping.  */
        for (siter = parent; siter && (siter->nodetype != LYS_GROUPING); siter = lys_parent(siter));
        if (siter) {
            assert(((struct lys_node_grp *)siter)->unres_count);
            ((struct lys_node_grp *)siter)->unres_count--;
        } else {
            LOGINT(ctx);
            goto error;
        }
        type->value_flags &= ~LY_VALUE_UNRESGRP;
    }

    /* check status */
    if (lyp_check_status(type->parent->flags, type->parent->module, type->parent->name,
                         type->der->flags, type->der->module, type->der->name, parent)) {
        goto error;
    }

    base = typ->base;
    base_tmp = type->base;
    type->base = type->der->type.base;
    if (base == 0) {
        base = type->der->type.base;
    }
    switch (base) {
    case LY_TYPE_STRING:
        if (type->base == LY_TYPE_BINARY) {
            if (type->info.str.pat_count) {
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Binary type could not include pattern statement.");
                goto error;
            }
            type->info.binary.length = type->info.str.length;
            if (type->info.binary.length && lyp_check_length_range(ctx, type->info.binary.length->expr, type)) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, type->info.binary.length->expr, "length");
                goto error;
            }
        } else if (type->base == LY_TYPE_STRING) {
            if (type->info.str.length && lyp_check_length_range(ctx, type->info.str.length->expr, type)) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, type->info.str.length->expr, "length");
                goto error;
            }
        } else {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid restriction in type \"%s\".", type->parent->name);
            goto error;
        }
        break;
    case LY_TYPE_DEC64:
        if (type->base == LY_TYPE_DEC64) {
            /* mandatory sub-statement(s) check */
            if (!type->info.dec64.dig && !type->der->type.der) {
                /* decimal64 type directly derived from built-in type requires fraction-digits */
                LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "fraction-digits", "type");
                goto error;
            }
            if (type->info.dec64.dig && type->der->type.der) {
                /* type is not directly derived from buit-in type and fraction-digits statement is prohibited */
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "fraction-digits");
                goto error;
            }

            /* copy fraction-digits specification from parent type for easier internal use */
            if (type->der->type.der) {
                type->info.dec64.dig = type->der->type.info.dec64.dig;
                type->info.dec64.div = type->der->type.info.dec64.div;
            }
            if (type->info.dec64.range && lyp_check_length_range(ctx, type->info.dec64.range->expr, type)) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, type->info.dec64.range->expr, "range");
                goto error;
            }
        } else if (type->base >= LY_TYPE_INT8 && type->base <=LY_TYPE_UINT64) {
            if (type->info.dec64.dig) {
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Numerical type could not include fraction statement.");
                goto error;
            }
            type->info.num.range = type->info.dec64.range;
            if (type->info.num.range && lyp_check_length_range(ctx, type->info.num.range->expr, type)) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, type->info.num.range->expr, "range");
                goto error;
            }
        } else {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid restriction in type \"%s\".", type->parent->name);
            goto error;
        }
        break;
    case LY_TYPE_ENUM:
        if (type->base != LY_TYPE_ENUM) {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid restriction in type \"%s\".", type->parent->name);
            goto error;
        }
        dertype = &type->der->type;

        if (!dertype->der) {
            if (!type->info.enums.count) {
                /* type is derived directly from buit-in enumeartion type and enum statement is required */
                LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "enum", "type");
                goto error;
            }
        } else {
            for (; !dertype->info.enums.count; dertype = &dertype->der->type);
            if (module->version < 2 && type->info.enums.count) {
                /* type is not directly derived from built-in enumeration type and enum statement is prohibited
                 * in YANG 1.0, since YANG 1.1 enum statements can be used to restrict the base enumeration type */
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "enum");
                goto error;
            }

            /* restricted enumeration type - the name MUST be used in the base type */
            enms_sc = dertype->info.enums.enm;
            for (i = 0; i < type->info.enums.count; i++) {
                for (j = 0; j < dertype->info.enums.count; j++) {
                    if (ly_strequal(enms_sc[j].name, type->info.enums.enm[i].name, 1)) {
                        break;
                    }
                }
                if (j == dertype->info.enums.count) {
                    LOGVAL(ctx, LYE_ENUM_INNAME, LY_VLOG_NONE, NULL, type->info.enums.enm[i].name);
                    goto error;
                }

                if (type->info.enums.enm[i].flags & LYS_AUTOASSIGNED) {
                    /* automatically assign value from base type */
                    type->info.enums.enm[i].value = enms_sc[j].value;
                } else {
                    /* check that the assigned value corresponds to the original
                     * value of the enum in the base type */
                    if (type->info.enums.enm[i].value != enms_sc[j].value) {
                        /* type->info.enums.enm[i].value - assigned value in restricted enum
                         * enms_sc[j].value - value assigned to the corresponding enum (detected above) in base type */
                        LOGVAL(ctx, LYE_ENUM_INVAL, LY_VLOG_NONE, NULL, type->info.enums.enm[i].value,
                               type->info.enums.enm[i].name, enms_sc[j].value);
                        goto error;
                    }
                }
            }
        }
        break;
    case LY_TYPE_BITS:
        if (type->base != LY_TYPE_BITS) {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid restriction in type \"%s\".", type->parent->name);
            goto error;
        }
        dertype = &type->der->type;

        if (!dertype->der) {
            if (!type->info.bits.count) {
                /* type is derived directly from buit-in bits type and bit statement is required */
                LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "bit", "type");
                goto error;
            }
        } else {
            for (; !dertype->info.enums.count; dertype = &dertype->der->type);
            if (module->version < 2 && type->info.bits.count) {
                /* type is not directly derived from buit-in bits type and bit statement is prohibited,
                 * since YANG 1.1 the bit statements can be used to restrict the base bits type */
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "bit");
                goto error;
            }

            bits_sc = dertype->info.bits.bit;
            for (i = 0; i < type->info.bits.count; i++) {
                for (j = 0; j < dertype->info.bits.count; j++) {
                    if (ly_strequal(bits_sc[j].name, type->info.bits.bit[i].name, 1)) {
                        break;
                    }
                }
                if (j == dertype->info.bits.count) {
                    LOGVAL(ctx, LYE_BITS_INNAME, LY_VLOG_NONE, NULL, type->info.bits.bit[i].name);
                    goto error;
                }

                /* restricted bits type */
                if (type->info.bits.bit[i].flags & LYS_AUTOASSIGNED) {
                    /* automatically assign position from base type */
                    type->info.bits.bit[i].pos = bits_sc[j].pos;
                } else {
                    /* check that the assigned position corresponds to the original
                     * position of the bit in the base type */
                    if (type->info.bits.bit[i].pos != bits_sc[j].pos) {
                        /* type->info.bits.bit[i].pos - assigned position in restricted bits
                         * bits_sc[j].pos - position assigned to the corresponding bit (detected above) in base type */
                        LOGVAL(ctx, LYE_BITS_INVAL, LY_VLOG_NONE, NULL, type->info.bits.bit[i].pos,
                               type->info.bits.bit[i].name, bits_sc[j].pos);
                        goto error;
                    }
                }
            }
        }

        for (i = type->info.bits.count; i > 0; i--) {
            j = i - 1;

            /* keep them ordered by position */
            while (j && type->info.bits.bit[j - 1].pos > type->info.bits.bit[j].pos) {
                /* switch them */
                memcpy(&bit_tmp, &type->info.bits.bit[j], sizeof bit_tmp);
                memcpy(&type->info.bits.bit[j], &type->info.bits.bit[j - 1], sizeof bit_tmp);
                memcpy(&type->info.bits.bit[j - 1], &bit_tmp, sizeof bit_tmp);
                j--;
            }
        }
        break;
    case LY_TYPE_LEAFREF:
        if (type->base == LY_TYPE_INST) {
            if (type->info.lref.path) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "path");
                goto error;
            }
            if ((req = type->info.lref.req)) {
                type->info.inst.req = req;
            }
        } else if (type->base == LY_TYPE_LEAFREF) {
            /* require-instance only YANG 1.1 */
            if (type->info.lref.req && (module->version < 2)) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "require-instance");
                goto error;
            }
            /* flag resolving for later use */
            if (!tpdftype) {
                for (siter = parent; siter && siter->nodetype != LYS_GROUPING; siter = lys_parent(siter));
                if (siter) {
                    /* just a flag - do not resolve */
                    tpdftype = 1;
                }
            }

            if (type->info.lref.path) {
                if (type->der->type.der) {
                    LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "path");
                    goto error;
                }
                value = type->info.lref.path;
                /* store in the JSON format */
                type->info.lref.path = transform_schema2json(module, value);
                lydict_remove(ctx, value);
                if (!type->info.lref.path) {
                    goto error;
                }
                /* try to resolve leafref path only when this is instantiated
                 * leaf, so it is not:
                 * - typedef's type,
                 * - in  grouping definition,
                 * - just instantiated in a grouping definition,
                 * because in those cases the nodes referenced in path might not be present
                 * and it is not a bug.  */
                if (!tpdftype && unres_schema_add_node(module, unres, type, UNRES_TYPE_LEAFREF, parent) == -1) {
                    goto error;
                }
            } else if (!type->der->type.der) {
                LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "path", "type");
                goto error;
            } else {
                /* copy leafref definition into the derived type */
                type->info.lref.path = lydict_insert(ctx, type->der->type.info.lref.path, 0);
                /* and resolve the path at the place we are (if not in grouping/typedef) */
                if (!tpdftype && unres_schema_add_node(module, unres, type, UNRES_TYPE_LEAFREF, parent) == -1) {
                    goto error;
                }
            }
        } else {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid restriction in type \"%s\".", type->parent->name);
            goto error;
        }
        break;
    case LY_TYPE_IDENT:
        if (type->base != LY_TYPE_IDENT) {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid restriction in type \"%s\".", type->parent->name);
            goto error;
        }
        if (type->der->type.der) {
            if (type->info.ident.ref) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "base");
                goto error;
            }
        } else {
            if (!type->info.ident.ref) {
                LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "base", "type");
                goto error;
            }
        }
        break;
    case LY_TYPE_UNION:
        if (type->base != LY_TYPE_UNION) {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid restriction in type \"%s\".", type->parent->name);
            goto error;
        }
        if (!type->info.uni.types) {
            if (type->der->type.der) {
                /* this is just a derived type with no additional type specified/required */
                assert(type->der->type.base == LY_TYPE_UNION);
                type->info.uni.has_ptr_type = type->der->type.info.uni.has_ptr_type;
                break;
            }
            LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "type", "(union) type");
            goto error;
        }
        for (i = 0; i < type->info.uni.count; i++) {
            dertype = &type->info.uni.types[i];
            if (dertype->base == LY_TYPE_DER) {
                yang = (struct yang_type *)dertype->der;
                dertype->der = NULL;
                dertype->parent = type->parent;
                if (yang_check_type(module, parent, yang, dertype, tpdftype, unres)) {
                    dertype->der = (struct lys_tpdf *)yang;
                    ret = EXIT_FAILURE;
                    type->base = base_tmp;
                    base = 0;
                    goto error;
                } else {
                    lydict_remove(ctx, yang->name);
                    free(yang);
                }
            }
            if (module->version < 2) {
                if (dertype->base == LY_TYPE_EMPTY) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, "empty", typ->name);
                    goto error;
                } else if (dertype->base == LY_TYPE_LEAFREF) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, "leafref", typ->name);
                    goto error;
                }
            }
            if ((dertype->base == LY_TYPE_INST) || (dertype->base == LY_TYPE_LEAFREF)
                    || ((dertype->base == LY_TYPE_UNION) && dertype->info.uni.has_ptr_type)) {
                type->info.uni.has_ptr_type = 1;
            }
        }
        break;

    default:
        if (base >= LY_TYPE_BINARY && base <= LY_TYPE_UINT64) {
            if (type->base != base) {
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid restriction in type \"%s\".", type->parent->name);
                goto error;
            }
        } else {
            LOGINT(ctx);
            goto error;
        }
    }

    /* if derived type has extension, which need validate data */
    dertype = &type->der->type;
    while (dertype->der) {
        if (dertype->parent->flags & LYS_VALID_EXT) {
            type->parent->flags |= LYS_VALID_EXT;
        }
        dertype = &dertype->der->type;
    }

    return EXIT_SUCCESS;

error:
    if (base) {
        type->base = base_tmp;
    }
    return ret;
}