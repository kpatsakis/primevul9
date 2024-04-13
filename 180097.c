ves_icall_ModuleBuilder_create_modified_type (MonoReflectionTypeBuilder *tb, MonoString *smodifiers)
{
	MonoClass *klass;
	int isbyref = 0, rank;
	char *str = mono_string_to_utf8 (smodifiers);
	char *p;

	MONO_ARCH_SAVE_REGS;

	klass = mono_class_from_mono_type (tb->type.type);
	p = str;
	/* logic taken from mono_reflection_parse_type(): keep in sync */
	while (*p) {
		switch (*p) {
		case '&':
			if (isbyref) { /* only one level allowed by the spec */
				g_free (str);
				return NULL;
			}
			isbyref = 1;
			p++;
			g_free (str);
			return mono_type_get_object (mono_object_domain (tb), &klass->this_arg);
			break;
		case '*':
			klass = mono_ptr_class_get (&klass->byval_arg);
			mono_class_init (klass);
			p++;
			break;
		case '[':
			rank = 1;
			p++;
			while (*p) {
				if (*p == ']')
					break;
				if (*p == ',')
					rank++;
				else if (*p != '*') { /* '*' means unknown lower bound */
					g_free (str);
					return NULL;
				}
				++p;
			}
			if (*p != ']') {
				g_free (str);
				return NULL;
			}
			p++;
			klass = mono_array_class_get (klass, rank);
			mono_class_init (klass);
			break;
		default:
			break;
		}
	}
	g_free (str);
	return mono_type_get_object (mono_object_domain (tb), &klass->byval_arg);
}