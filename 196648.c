verify_field_table (MonoImage *image, GSList *list, int level)
{
	MonoTableInfo *t = &image->tables [MONO_TABLE_FIELD];
	guint32 cols [MONO_FIELD_SIZE];
	const char *p;
	guint32 i, flags;

	for (i = 0; i < t->rows; ++i) {
		mono_metadata_decode_row (t, i, cols, MONO_FIELD_SIZE);
		/*
		 * Check this field has only one owner and that the owner is not 
		 * an interface (done in verify_typedef_table() )
		 */
		flags = cols [MONO_FIELD_FLAGS];
		switch (flags & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK) {
		case FIELD_ATTRIBUTE_COMPILER_CONTROLLED:
		case FIELD_ATTRIBUTE_PRIVATE:
		case FIELD_ATTRIBUTE_FAM_AND_ASSEM:
		case FIELD_ATTRIBUTE_ASSEMBLY:
		case FIELD_ATTRIBUTE_FAMILY:
		case FIELD_ATTRIBUTE_FAM_OR_ASSEM:
		case FIELD_ATTRIBUTE_PUBLIC:
			break;
		default:
			if (level & MONO_VERIFY_ERROR)
				ADD_ERROR (list, g_strdup_printf ("Invalid access mask in Field row %d", i + 1));
			break;
		}
		if (level & MONO_VERIFY_ERROR) {
			if ((flags & FIELD_ATTRIBUTE_LITERAL) && (flags & FIELD_ATTRIBUTE_INIT_ONLY))
				ADD_ERROR (list, g_strdup_printf ("Literal and InitOnly cannot be both set in Field row %d", i + 1));
			if ((flags & FIELD_ATTRIBUTE_LITERAL) && !(flags & FIELD_ATTRIBUTE_STATIC))
				ADD_ERROR (list, g_strdup_printf ("Literal needs also Static set in Field row %d", i + 1));
			if ((flags & FIELD_ATTRIBUTE_RT_SPECIAL_NAME) && !(flags & FIELD_ATTRIBUTE_SPECIAL_NAME))
				ADD_ERROR (list, g_strdup_printf ("RTSpecialName needs also SpecialName set in Field row %d", i + 1));
			/*
			 * FIXME: check there is only one owner in the respective table.
			 * if (flags & FIELD_ATTRIBUTE_HAS_FIELD_MARSHAL)
			 * if (flags & FIELD_ATTRIBUTE_HAS_DEFAULT)
			 * if (flags & FIELD_ATTRIBUTE_HAS_FIELD_RVA)
			 */
		}
		if (!(p = is_valid_string (image, cols [MONO_FIELD_NAME], TRUE))) {
			if (level & MONO_VERIFY_ERROR)
				ADD_ERROR (list, g_strdup_printf ("Invalid name in Field row %d", i + 1));
		} else {
			if (level & MONO_VERIFY_CLS) {
				if (!is_valid_cls_ident (p))
					ADD_WARN (list, MONO_VERIFY_CLS, g_strdup_printf ("Invalid CLS name '%s` in Field row %d", p, i + 1));
			}
		}
		/*
		 * check signature.
		 * if owner is module needs to be static, access mask needs to be compilercontrolled,
		 * public or private (not allowed in cls mode).
		 * if owner is an enum ...
		 */


	}
	return list;
}