do_invoke_method (VerifyContext *ctx, int method_token, gboolean virtual)
{
	int param_count, i;
	MonoMethodSignature *sig;
	ILStackDesc *value;
	MonoMethod *method;
	gboolean virt_check_this = FALSE;
	gboolean constrained = ctx->prefix_set & PREFIX_CONSTRAINED;

	if (!(method = verifier_load_method (ctx, method_token, virtual ? "callvirt" : "call")))
		return;

	if (virtual) {
		CLEAR_PREFIX (ctx, PREFIX_CONSTRAINED);

		if (method->klass->valuetype) // && !constrained ???
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use callvirtual with valuetype method at 0x%04x", ctx->ip_offset));

		if ((method->flags & METHOD_ATTRIBUTE_STATIC))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use callvirtual with static method at 0x%04x", ctx->ip_offset));

	} else {
		if (method->flags & METHOD_ATTRIBUTE_ABSTRACT) 
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use call with an abstract method at 0x%04x", ctx->ip_offset));
		
		if ((method->flags & METHOD_ATTRIBUTE_VIRTUAL) && !(method->flags & METHOD_ATTRIBUTE_FINAL) && !(method->klass->flags & TYPE_ATTRIBUTE_SEALED)) {
			virt_check_this = TRUE;
			ctx->code [ctx->ip_offset].flags |= IL_CODE_CALL_NONFINAL_VIRTUAL;
		}
	}

	if (!(sig = mono_method_get_signature_full (method, ctx->image, method_token, ctx->generic_context)))
		sig = mono_method_get_signature (method, ctx->image, method_token);

	if (!sig) {
		char *name = mono_type_get_full_name (method->klass);
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Could not resolve signature of %s:%s at 0x%04x", name, method->name, ctx->ip_offset));
		g_free (name);
		return;
	}

	param_count = sig->param_count + sig->hasthis;
	if (!check_underflow (ctx, param_count))
		return;

	for (i = sig->param_count - 1; i >= 0; --i) {
		VERIFIER_DEBUG ( printf ("verifying argument %d\n", i); );
		value = stack_pop (ctx);
		if (!verify_stack_type_compatibility (ctx, sig->params[i], value)) {
			char *stack_name = stack_slot_full_name (value);
			char *sig_name = mono_type_full_name (sig->params [i]);
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Incompatible parameter with function signature: Calling method with signature (%s) but for argument %d there is a (%s) on stack at 0x%04x", sig_name, i, stack_name, ctx->ip_offset));
			g_free (stack_name);
			g_free (sig_name);
		}

		if (stack_slot_is_managed_mutability_pointer (value))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use a readonly pointer as argument of %s at 0x%04x", virtual ? "callvirt" : "call",  ctx->ip_offset));

		if ((ctx->prefix_set & PREFIX_TAIL) && stack_slot_is_managed_pointer (value)) {
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Cannot  pass a byref argument to a tail %s at 0x%04x", virtual ? "callvirt" : "call",  ctx->ip_offset));
			return;
		}
	}

	if (sig->hasthis) {
		MonoType *type = &method->klass->byval_arg;
		ILStackDesc copy;

		if (mono_method_is_constructor (method) && !method->klass->valuetype) {
			if (!mono_method_is_constructor (ctx->method))
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot call a constructor outside one at 0x%04x", ctx->ip_offset));
			if (method->klass != ctx->method->klass->parent && method->klass != ctx->method->klass)
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot call a constructor to a type diferent that this or super at 0x%04x", ctx->ip_offset));

			ctx->super_ctor_called = TRUE;
			value = stack_pop_safe (ctx);
			if ((value->stype & THIS_POINTER_MASK) != THIS_POINTER_MASK)
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid 'this ptr' argument for constructor at 0x%04x", ctx->ip_offset));
		} else {
			value = stack_pop (ctx);
		}
			
		copy_stack_value (&copy, value);
		//TODO we should extract this to a 'drop_byref_argument' and use everywhere
		//Other parts of the code suffer from the same issue of 
		copy.type = mono_type_get_type_byval (copy.type);
		copy.stype &= ~POINTER_MASK;

		if (virt_check_this && !stack_slot_is_this_pointer (value) && !(method->klass->valuetype || stack_slot_is_boxed_value (value)))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot call a non-final virtual method from an objet diferent thant the this pointer at 0x%04x", ctx->ip_offset));

		if (constrained && virtual) {
			if (!stack_slot_is_managed_pointer (value))
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Object is not a managed pointer for a constrained call at 0x%04x", ctx->ip_offset));
			if (!mono_metadata_type_equal_full (mono_type_get_type_byval (value->type), ctx->constrained_type, TRUE))
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Object not compatible with constrained type at 0x%04x", ctx->ip_offset));
			copy.stype |= BOXED_MASK;
		} else {
			if (stack_slot_is_managed_pointer (value) && !mono_class_from_mono_type (value->type)->valuetype)
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot call a reference type using a managed pointer to the this arg at 0x%04x", ctx->ip_offset));
	
			if (!virtual && mono_class_from_mono_type (value->type)->valuetype && !method->klass->valuetype && !stack_slot_is_boxed_value (value))
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot call a valuetype baseclass at 0x%04x", ctx->ip_offset));
	
			if (virtual && mono_class_from_mono_type (value->type)->valuetype && !stack_slot_is_boxed_value (value))
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use a valuetype with callvirt at 0x%04x", ctx->ip_offset));
	
			if (method->klass->valuetype && (stack_slot_is_boxed_value (value) || !stack_slot_is_managed_pointer (value)))
				CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use a boxed or literal valuetype to call a valuetype method at 0x%04x", ctx->ip_offset));
		}
		if (!verify_stack_type_compatibility (ctx, type, &copy)) {
			char *expected = mono_type_full_name (type);
			char *effective = stack_slot_full_name (&copy);
			char *method_name = mono_method_full_name (method, TRUE);
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Incompatible this argument on stack with method signature expected '%s' but got '%s' for a call to '%s' at 0x%04x",
					expected, effective, method_name, ctx->ip_offset));
			g_free (method_name);
			g_free (effective);
			g_free (expected);
		}

		if (!IS_SKIP_VISIBILITY (ctx) && !mono_method_can_access_method_full (ctx->method, method, mono_class_from_mono_type (value->type))) {
			char *name = mono_method_full_name (method, TRUE);
			CODE_NOT_VERIFIABLE2 (ctx, g_strdup_printf ("Method %s is not accessible at 0x%04x", name, ctx->ip_offset), MONO_EXCEPTION_METHOD_ACCESS);
			g_free (name);
		}

	} else if (!IS_SKIP_VISIBILITY (ctx) && !mono_method_can_access_method_full (ctx->method, method, NULL)) {
		char *name = mono_method_full_name (method, TRUE);
		CODE_NOT_VERIFIABLE2 (ctx, g_strdup_printf ("Method %s is not accessible at 0x%04x", name, ctx->ip_offset), MONO_EXCEPTION_METHOD_ACCESS);
		g_free (name);
	}

	if (sig->ret->type != MONO_TYPE_VOID) {
		if (check_overflow (ctx)) {
			value = stack_push (ctx);
			set_stack_value (ctx, value, sig->ret, FALSE);
			if ((ctx->prefix_set & PREFIX_READONLY) && method->klass->rank && !strcmp (method->name, "Address")) {
				ctx->prefix_set &= ~PREFIX_READONLY;
				value->stype |= CMMP_MASK;
			}
		}
	}

	if ((ctx->prefix_set & PREFIX_TAIL)) {
		if (!mono_delegate_ret_equal (mono_method_signature (ctx->method)->ret, sig->ret))
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Tail call with incompatible return type at 0x%04x", ctx->ip_offset));
		if (ctx->header->code [ctx->ip_offset + 5] != CEE_RET)
			CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Tail call not followed by ret at 0x%04x", ctx->ip_offset));
	}

}