verify_type_compatibility_full (VerifyContext *ctx, MonoType *target, MonoType *candidate, gboolean strict)
{
#define IS_ONE_OF3(T, A, B, C) (T == A || T == B || T == C)
#define IS_ONE_OF2(T, A, B) (T == A || T == B)

	MonoType *original_candidate = candidate;
	VERIFIER_DEBUG ( printf ("checking type compatibility %s x %s strict %d\n", mono_type_full_name (target), mono_type_full_name (candidate), strict); );

 	/*only one is byref */
	if (candidate->byref ^ target->byref) {
		/* converting from native int to byref*/
		if (get_stack_type (candidate) == TYPE_NATIVE_INT && target->byref) {
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("using byref native int at 0x%04x", ctx->ip_offset));
			return TRUE;
		}
		return FALSE;
	}
	strict |= target->byref;
	/*From now on we don't care about byref anymore, so it's ok to discard it here*/
	candidate = mono_type_get_underlying_type_any (candidate);

handle_enum:
	switch (target->type) {
	case MONO_TYPE_VOID:
		return candidate->type == MONO_TYPE_VOID;
	case MONO_TYPE_I1:
	case MONO_TYPE_U1:
	case MONO_TYPE_BOOLEAN:
		if (strict)
			return IS_ONE_OF3 (candidate->type, MONO_TYPE_I1, MONO_TYPE_U1, MONO_TYPE_BOOLEAN);
	case MONO_TYPE_I2:
	case MONO_TYPE_U2:
	case MONO_TYPE_CHAR:
		if (strict)
			return IS_ONE_OF3 (candidate->type, MONO_TYPE_I2, MONO_TYPE_U2, MONO_TYPE_CHAR);
	case MONO_TYPE_I4:
	case MONO_TYPE_U4: {
		gboolean is_native_int = IS_ONE_OF2 (candidate->type, MONO_TYPE_I, MONO_TYPE_U);
		gboolean is_int4 = IS_ONE_OF2 (candidate->type, MONO_TYPE_I4, MONO_TYPE_U4);
		if (strict)
			return is_native_int || is_int4;
		return is_native_int || get_stack_type (candidate) == TYPE_I4;
	}

	case MONO_TYPE_I8:
	case MONO_TYPE_U8:
		return IS_ONE_OF2 (candidate->type, MONO_TYPE_I8, MONO_TYPE_U8);

	case MONO_TYPE_R4:
	case MONO_TYPE_R8:
		if (strict)
			return candidate->type == target->type;
		return IS_ONE_OF2 (candidate->type, MONO_TYPE_R4, MONO_TYPE_R8);

	case MONO_TYPE_I:
	case MONO_TYPE_U: {
		gboolean is_native_int = IS_ONE_OF2 (candidate->type, MONO_TYPE_I, MONO_TYPE_U);
		gboolean is_int4 = IS_ONE_OF2 (candidate->type, MONO_TYPE_I4, MONO_TYPE_U4);
		if (strict)
			return is_native_int || is_int4;
		return is_native_int || get_stack_type (candidate) == TYPE_I4;
	}

	case MONO_TYPE_PTR:
		if (candidate->type != MONO_TYPE_PTR)
			return FALSE;
		/* check the underlying type */
		return verify_type_compatibility_full (ctx, target->data.type, candidate->data.type, TRUE);

	case MONO_TYPE_FNPTR: {
		MonoMethodSignature *left, *right;
		if (candidate->type != MONO_TYPE_FNPTR)
			return FALSE;

		left = mono_type_get_signature (target);
		right = mono_type_get_signature (candidate);
		return mono_metadata_signature_equal (left, right) && left->call_convention == right->call_convention;
	}

	case MONO_TYPE_GENERICINST: {
		MonoClass *target_klass;
		MonoClass *candidate_klass;
		if (mono_type_is_enum_type (target)) {
			target = mono_type_get_underlying_type_any (target);
			if (!target)
				return FALSE;
			goto handle_enum;
		}
		/*
		 * VAR / MVAR compatibility must be checked by verify_stack_type_compatibility
		 * to take boxing status into account.
		 */
		if (mono_type_is_generic_argument (original_candidate))
			return FALSE;

		target_klass = mono_class_from_mono_type (target);
		candidate_klass = mono_class_from_mono_type (candidate);
		if (mono_class_is_nullable (target_klass)) {
			if (!mono_class_is_nullable (candidate_klass))
				return FALSE;
			return target_klass == candidate_klass;
		}
		
		return mono_class_is_assignable_from (target_klass, candidate_klass);
	}

	case MONO_TYPE_STRING:
		return candidate->type == MONO_TYPE_STRING;

	case MONO_TYPE_CLASS:
		/*
		 * VAR / MVAR compatibility must be checked by verify_stack_type_compatibility
		 * to take boxing status into account.
		 */
		if (mono_type_is_generic_argument (original_candidate))
			return FALSE;

		if (candidate->type == MONO_TYPE_VALUETYPE)
			return FALSE;

		/* If candidate is an enum it should return true for System.Enum and supertypes.
		 * That's why here we use the original type and not the underlying type.
		 */ 
		return mono_class_is_assignable_from (target->data.klass, mono_class_from_mono_type (original_candidate));

	case MONO_TYPE_OBJECT:
		return MONO_TYPE_IS_REFERENCE (candidate);

	case MONO_TYPE_SZARRAY: {
		MonoClass *left;
		MonoClass *right;
		if (candidate->type != MONO_TYPE_SZARRAY)
			return FALSE;

		left = mono_class_from_mono_type (target)->element_class;
		right = mono_class_from_mono_type (candidate)->element_class;
		return mono_class_is_assignable_from (left, right);
	}

	case MONO_TYPE_ARRAY:
		if (candidate->type != MONO_TYPE_ARRAY)
			return FALSE;
		return is_array_type_compatible (target, candidate);

	case MONO_TYPE_TYPEDBYREF:
		return candidate->type == MONO_TYPE_TYPEDBYREF;

	case MONO_TYPE_VALUETYPE: {
		MonoClass *target_klass;
		MonoClass *candidate_klass;

		if (candidate->type == MONO_TYPE_CLASS)
			return FALSE;

		target_klass = mono_class_from_mono_type (target);
		candidate_klass = mono_class_from_mono_type (candidate);
		if (target_klass == candidate_klass)
			return TRUE;
		if (mono_type_is_enum_type (target)) {
			target = mono_type_get_underlying_type_any (target);
			if (!target)
				return FALSE;
			goto handle_enum;
		}
		return FALSE;
	}

	case MONO_TYPE_VAR:
		if (candidate->type != MONO_TYPE_VAR)
			return FALSE;
		return mono_type_get_generic_param_num (candidate) == mono_type_get_generic_param_num (target);

	case MONO_TYPE_MVAR:
		if (candidate->type != MONO_TYPE_MVAR)
			return FALSE;
		return mono_type_get_generic_param_num (candidate) == mono_type_get_generic_param_num (target);

	default:
		VERIFIER_DEBUG ( printf ("unknown store type %d\n", target->type); );
		g_assert_not_reached ();
		return FALSE;
	}
	return 1;
#undef IS_ONE_OF3
#undef IS_ONE_OF2
}