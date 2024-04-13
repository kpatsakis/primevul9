lyp_get_next_union_type(struct lys_type *type, struct lys_type *prev_type, int *found)
{
    unsigned int i;
    struct lys_type *ret = NULL;

    while (!type->info.uni.count) {
        assert(type->der); /* at least the direct union type has to have type specified */
        type = &type->der->type;
    }

    for (i = 0; i < type->info.uni.count; ++i) {
        if (type->info.uni.types[i].base == LY_TYPE_UNION) {
            ret = lyp_get_next_union_type(&type->info.uni.types[i], prev_type, found);
            if (ret) {
                break;
            }
            continue;
        }

        if (!prev_type || *found) {
            ret = &type->info.uni.types[i];
            break;
        }

        if (&type->info.uni.types[i] == prev_type) {
            *found = 1;
        }
    }

    return ret;
}