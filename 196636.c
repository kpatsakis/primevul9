GSList*
mono_method_verify (MonoMethod *method, int level)
{
	MonoError error;
	const unsigned char *ip, *code_start;
	const unsigned char *end;
	MonoSimpleBasicBlock *bb = NULL, *original_bb = NULL;

	int i, n, need_merge = 0, start = 0;
	guint token, ip_offset = 0, prefix = 0;
	MonoGenericContext *generic_context = NULL;
	MonoImage *image;
	VerifyContext ctx;
	GSList *tmp;
	VERIFIER_DEBUG ( printf ("Verify IL for method %s %s %s\n",  method->klass->name_space,  method->klass->name, method->name); );

	if (method->iflags & (METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL | METHOD_IMPL_ATTRIBUTE_RUNTIME) ||
			(method->flags & (METHOD_ATTRIBUTE_PINVOKE_IMPL | METHOD_ATTRIBUTE_ABSTRACT))) {
		return NULL;
	}

	memset (&ctx, 0, sizeof (VerifyContext));

	//FIXME use mono_method_get_signature_full
	ctx.signature = mono_method_signature (method);
	if (!ctx.signature) {
		ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Could not decode method signature"));
		return ctx.list;
	}
	ctx.header = mono_method_get_header (method);
	if (!ctx.header) {
		ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Could not decode method header"));
		return ctx.list;
	}
	ctx.method = method;
	code_start = ip = ctx.header->code;
	end = ip + ctx.header->code_size;
	ctx.image = image = method->klass->image;


	ctx.max_args = ctx.signature->param_count + ctx.signature->hasthis;
	ctx.max_stack = ctx.header->max_stack;
	ctx.verifiable = ctx.valid = 1;
	ctx.level = level;

	ctx.code = g_new (ILCodeDesc, ctx.header->code_size);
	ctx.code_size = ctx.header->code_size;

	memset(ctx.code, 0, sizeof (ILCodeDesc) * ctx.header->code_size);


	ctx.num_locals = ctx.header->num_locals;
	ctx.locals = g_memdup (ctx.header->locals, sizeof (MonoType*) * ctx.header->num_locals);

	if (ctx.num_locals > 0 && !ctx.header->init_locals)
		CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("Method with locals variable but without init locals set"));

	ctx.params = g_new (MonoType*, ctx.max_args);
	if (ctx.signature->hasthis)
		ctx.params [0] = method->klass->valuetype ? &method->klass->this_arg : &method->klass->byval_arg;
	memcpy (ctx.params + ctx.signature->hasthis, ctx.signature->params, sizeof (MonoType *) * ctx.signature->param_count);

	if (ctx.signature->is_inflated)
		ctx.generic_context = generic_context = mono_method_get_context (method);

	if (!generic_context && (method->klass->generic_container || method->is_generic)) {
		if (method->is_generic)
			ctx.generic_context = generic_context = &(mono_method_get_generic_container (method)->context);
		else
			ctx.generic_context = generic_context = &method->klass->generic_container->context;
	}

	for (i = 0; i < ctx.num_locals; ++i) {
		MonoType *uninflated = ctx.locals [i];
		ctx.locals [i] = mono_class_inflate_generic_type_checked (ctx.locals [i], ctx.generic_context, &error);
		if (!mono_error_ok (&error)) {
			char *name = mono_type_full_name (ctx.locals [i] ? ctx.locals [i] : uninflated);
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid local %d of type %s", i, name));
			g_free (name);
			mono_error_cleanup (&error);
			/* we must not free (in cleanup) what was not yet allocated (but only copied) */
			ctx.num_locals = i;
			ctx.max_args = 0;
			goto cleanup;
		}
	}
	for (i = 0; i < ctx.max_args; ++i) {
		MonoType *uninflated = ctx.params [i];
		ctx.params [i] = mono_class_inflate_generic_type_checked (ctx.params [i], ctx.generic_context, &error);
		if (!mono_error_ok (&error)) {
			char *name = mono_type_full_name (ctx.params [i] ? ctx.params [i] : uninflated);
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid parameter %d of type %s", i, name));
			g_free (name);
			mono_error_cleanup (&error);
			/* we must not free (in cleanup) what was not yet allocated (but only copied) */
			ctx.max_args = i;
			goto cleanup;
		}
	}
	stack_init (&ctx, &ctx.eval);

	for (i = 0; i < ctx.num_locals; ++i) {
		if (!mono_type_is_valid_in_context (&ctx, ctx.locals [i]))
			break;
		if (get_stack_type (ctx.locals [i]) == TYPE_INV) {
			char *name = mono_type_full_name (ctx.locals [i]);
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid local %i of type %s", i, name));
			g_free (name);
			break;
		}
		
	}

	for (i = 0; i < ctx.max_args; ++i) {
		if (!mono_type_is_valid_in_context (&ctx, ctx.params [i]))
			break;

		if (get_stack_type (ctx.params [i]) == TYPE_INV) {
			char *name = mono_type_full_name (ctx.params [i]);
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid parameter %i of type %s", i, name));
			g_free (name);
			break;
		}
	}

	if (!ctx.valid)
		goto cleanup;

	for (i = 0; i < ctx.header->num_clauses && ctx.valid; ++i) {
		MonoExceptionClause *clause = ctx.header->clauses + i;
		VERIFIER_DEBUG (printf ("clause try %x len %x filter at %x handler at %x len %x\n", clause->try_offset, clause->try_len, clause->data.filter_offset, clause->handler_offset, clause->handler_len); );

		if (clause->try_offset > ctx.code_size || ADD_IS_GREATER_OR_OVF (clause->try_offset, clause->try_len, ctx.code_size))
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("try clause out of bounds at 0x%04x", clause->try_offset));

		if (clause->try_len <= 0)
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("try clause len <= 0 at 0x%04x", clause->try_offset));

		if (clause->handler_offset > ctx.code_size || ADD_IS_GREATER_OR_OVF (clause->handler_offset, clause->handler_len, ctx.code_size))
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("handler clause out of bounds at 0x%04x", clause->try_offset));

		if (clause->handler_len <= 0)
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("handler clause len <= 0 at 0x%04x", clause->try_offset));

		if (clause->try_offset < clause->handler_offset && ADD_IS_GREATER_OR_OVF (clause->try_offset, clause->try_len, HANDLER_START (clause)))
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("try block (at 0x%04x) includes handler block (at 0x%04x)", clause->try_offset, clause->handler_offset));

		if (clause->flags == MONO_EXCEPTION_CLAUSE_FILTER) {
			if (clause->data.filter_offset > ctx.code_size)
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("filter clause out of bounds at 0x%04x", clause->try_offset));

			if (clause->data.filter_offset >= clause->handler_offset)
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("filter clause must come before the handler clause at 0x%04x", clause->data.filter_offset));
		}

		for (n = i + 1; n < ctx.header->num_clauses && ctx.valid; ++n)
			verify_clause_relationship (&ctx, clause, ctx.header->clauses + n);

		if (!ctx.valid)
			break;

		ctx.code [clause->try_offset].flags |= IL_CODE_FLAG_WAS_TARGET;
		if (clause->try_offset + clause->try_len < ctx.code_size)
			ctx.code [clause->try_offset + clause->try_len].flags |= IL_CODE_FLAG_WAS_TARGET;
		if (clause->handler_offset + clause->handler_len < ctx.code_size)
			ctx.code [clause->handler_offset + clause->handler_len].flags |= IL_CODE_FLAG_WAS_TARGET;

		if (clause->flags == MONO_EXCEPTION_CLAUSE_NONE) {
			if (!clause->data.catch_class) {
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Catch clause %d with invalid type", i));
				break;
			}
		
			init_stack_with_value_at_exception_boundary (&ctx, ctx.code + clause->handler_offset, clause->data.catch_class);
		}
		else if (clause->flags == MONO_EXCEPTION_CLAUSE_FILTER) {
			init_stack_with_value_at_exception_boundary (&ctx, ctx.code + clause->data.filter_offset, mono_defaults.exception_class);
			init_stack_with_value_at_exception_boundary (&ctx, ctx.code + clause->handler_offset, mono_defaults.exception_class);	
		}
	}

	original_bb = bb = mono_basic_block_split (method, &error);
	if (!mono_error_ok (&error)) {
		ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid branch target: %s", mono_error_get_message (&error)));
		mono_error_cleanup (&error);
		goto cleanup;
	}
	g_assert (bb);

	while (ip < end && ctx.valid) {
		int op_size;
		ip_offset = ip - code_start;
		{
			const unsigned char *ip_copy = ip;
			int op;

			if (ip_offset > bb->end) {
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Branch or EH block at [0x%04x] targets middle instruction at 0x%04x", bb->end, ip_offset));
				goto cleanup;
			}

			if (ip_offset == bb->end)
				bb = bb->next;
	
			op_size = mono_opcode_value_and_size (&ip_copy, end, &op);
			if (op_size == -1) {
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid instruction %x at 0x%04x", *ip, ip_offset));
				goto cleanup;
			}

			if (ADD_IS_GREATER_OR_OVF (ip_offset, op_size, bb->end)) {
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Branch or EH block targets middle of instruction at 0x%04x", ip_offset));
				goto cleanup;
			}

			/*Last Instruction*/
			if (ip_offset + op_size == bb->end && mono_opcode_is_prefix (op)) {
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Branch or EH block targets between prefix '%s' and instruction at 0x%04x", mono_opcode_name (op), ip_offset));
				goto cleanup;
			}
		}

		ctx.ip_offset = ip_offset = ip - code_start;

		/*We need to check against fallthrou in and out of protected blocks.
		 * For fallout we check the once a protected block ends, if the start flag is not set.
		 * Likewise for fallthru in, we check if ip is the start of a protected block and start is not set
		 * TODO convert these checks to be done using flags and not this loop
		 */
		for (i = 0; i < ctx.header->num_clauses && ctx.valid; ++i) {
			MonoExceptionClause *clause = ctx.header->clauses + i;

			if ((clause->try_offset + clause->try_len == ip_offset) && start == 0) {
				CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("fallthru off try block at 0x%04x", ip_offset));
				start = 1;
			}

			if ((clause->handler_offset + clause->handler_len == ip_offset) && start == 0) {
				if (clause->flags == MONO_EXCEPTION_CLAUSE_FILTER)
					ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("fallout of handler block at 0x%04x", ip_offset));
				else
					CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("fallout of handler block at 0x%04x", ip_offset));
				start = 1;
			}

			if (clause->flags == MONO_EXCEPTION_CLAUSE_FILTER && clause->handler_offset == ip_offset && start == 0) {
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("fallout of filter block at 0x%04x", ip_offset));
				start = 1;
			}

			if (clause->handler_offset == ip_offset && start == 0) {
				CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("fallthru handler block at 0x%04x", ip_offset));
				start = 1;
			}

			if (clause->try_offset == ip_offset && ctx.eval.size > 0 && start == 0) {
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Try to enter try block with a non-empty stack at 0x%04x", ip_offset));
				start = 1;
			}
		}

		/*This must be done after fallthru detection otherwise it won't happen.*/
		if (bb->dead) {
			/*FIXME remove this once we move all bad branch checking code to use BB only*/
			ctx.code [ip_offset].flags |= IL_CODE_FLAG_SEEN;
			ip += op_size;
			continue;
		}

		if (!ctx.valid)
			break;

		if (need_merge) {
			VERIFIER_DEBUG ( printf ("extra merge needed! 0x%04x \n", ctx.target); );
			merge_stacks (&ctx, &ctx.eval, &ctx.code [ctx.target], FALSE, TRUE);
			need_merge = 0;	
		}
		merge_stacks (&ctx, &ctx.eval, &ctx.code[ip_offset], start, FALSE);
		start = 0;

		/*TODO we can fast detect a forward branch or exception block targeting code after prefix, we should fail fast*/
#ifdef MONO_VERIFIER_DEBUG
		{
			char *discode;
			discode = mono_disasm_code_one (NULL, method, ip, NULL);
			discode [strlen (discode) - 1] = 0; /* no \n */
			g_print ("[%d] %-29s (%d)\n",  ip_offset, discode, ctx.eval.size);
			g_free (discode);
		}
		dump_stack_state (&ctx.code [ip_offset]);
		dump_stack_state (&ctx.eval);
#endif

		switch (*ip) {
		case CEE_NOP:
		case CEE_BREAK:
			++ip;
			break;

		case CEE_LDARG_0:
		case CEE_LDARG_1:
		case CEE_LDARG_2:
		case CEE_LDARG_3:
			push_arg (&ctx, *ip - CEE_LDARG_0, FALSE);
			++ip;
			break;

		case CEE_LDARG_S:
		case CEE_LDARGA_S:
			code_bounds_check (2);
			push_arg (&ctx, ip [1],  *ip == CEE_LDARGA_S);
			ip += 2;
			break;

		case CEE_ADD_OVF_UN:
			do_binop (&ctx, *ip, add_ovf_un_table);
			++ip;
			break;

		case CEE_SUB_OVF_UN:
			do_binop (&ctx, *ip, sub_ovf_un_table);
			++ip;
			break;

		case CEE_ADD_OVF:
		case CEE_SUB_OVF:
		case CEE_MUL_OVF:
		case CEE_MUL_OVF_UN:
			do_binop (&ctx, *ip, bin_ovf_table);
			++ip;
			break;

		case CEE_ADD:
			do_binop (&ctx, *ip, add_table);
			++ip;
			break;

		case CEE_SUB:
			do_binop (&ctx, *ip, sub_table);
			++ip;
			break;

		case CEE_MUL:
		case CEE_DIV:
		case CEE_REM:
			do_binop (&ctx, *ip, bin_op_table);
			++ip;
			break;

		case CEE_AND:
		case CEE_DIV_UN:
		case CEE_OR:
		case CEE_REM_UN:
		case CEE_XOR:
			do_binop (&ctx, *ip, int_bin_op_table);
			++ip;
			break;

		case CEE_SHL:
		case CEE_SHR:
		case CEE_SHR_UN:
			do_binop (&ctx, *ip, shift_op_table);
			++ip;
			break;

		case CEE_POP:
			if (!check_underflow (&ctx, 1))
				break;
			stack_pop_safe (&ctx);
			++ip;
			break;

		case CEE_RET:
			do_ret (&ctx);
			++ip;
			start = 1;
			break;

		case CEE_LDLOC_0:
		case CEE_LDLOC_1:
		case CEE_LDLOC_2:
		case CEE_LDLOC_3:
			/*TODO support definite assignment verification? */
			push_local (&ctx, *ip - CEE_LDLOC_0, FALSE);
			++ip;
			break;

		case CEE_STLOC_0:
		case CEE_STLOC_1:
		case CEE_STLOC_2:
		case CEE_STLOC_3:
			store_local (&ctx, *ip - CEE_STLOC_0);
			++ip;
			break;

		case CEE_STLOC_S:
			code_bounds_check (2);
			store_local (&ctx, ip [1]);
			ip += 2;
			break;

		case CEE_STARG_S:
			code_bounds_check (2);
			store_arg (&ctx, ip [1]);
			ip += 2;
			break;

		case CEE_LDC_I4_M1:
		case CEE_LDC_I4_0:
		case CEE_LDC_I4_1:
		case CEE_LDC_I4_2:
		case CEE_LDC_I4_3:
		case CEE_LDC_I4_4:
		case CEE_LDC_I4_5:
		case CEE_LDC_I4_6:
		case CEE_LDC_I4_7:
		case CEE_LDC_I4_8:
			if (check_overflow (&ctx))
				stack_push_val (&ctx, TYPE_I4, &mono_defaults.int32_class->byval_arg);
			++ip;
			break;

		case CEE_LDC_I4_S:
			code_bounds_check (2);
			if (check_overflow (&ctx))
				stack_push_val (&ctx, TYPE_I4, &mono_defaults.int32_class->byval_arg);
			ip += 2;
			break;

		case CEE_LDC_I4:
			code_bounds_check (5);
			if (check_overflow (&ctx))
				stack_push_val (&ctx,TYPE_I4, &mono_defaults.int32_class->byval_arg);
			ip += 5;
			break;

		case CEE_LDC_I8:
			code_bounds_check (9);
			if (check_overflow (&ctx))
				stack_push_val (&ctx,TYPE_I8, &mono_defaults.int64_class->byval_arg);
			ip += 9;
			break;

		case CEE_LDC_R4:
			code_bounds_check (5);
			if (check_overflow (&ctx))
				stack_push_val (&ctx, TYPE_R8, &mono_defaults.double_class->byval_arg);
			ip += 5;
			break;

		case CEE_LDC_R8:
			code_bounds_check (9);
			if (check_overflow (&ctx))
				stack_push_val (&ctx, TYPE_R8, &mono_defaults.double_class->byval_arg);
			ip += 9;
			break;

		case CEE_LDNULL:
			if (check_overflow (&ctx))
				stack_push_val (&ctx, TYPE_COMPLEX | NULL_LITERAL_MASK, &mono_defaults.object_class->byval_arg);
			++ip;
			break;

		case CEE_BEQ_S:
		case CEE_BNE_UN_S:
			code_bounds_check (2);
			do_branch_op (&ctx, (signed char)ip [1] + 2, cmp_br_eq_op);
			ip += 2;
			need_merge = 1;
			break;

		case CEE_BGE_S:
		case CEE_BGT_S:
		case CEE_BLE_S:
		case CEE_BLT_S:
		case CEE_BGE_UN_S:
		case CEE_BGT_UN_S:
		case CEE_BLE_UN_S:
		case CEE_BLT_UN_S:
			code_bounds_check (2);
			do_branch_op (&ctx, (signed char)ip [1] + 2, cmp_br_op);
			ip += 2;
			need_merge = 1;
			break;

		case CEE_BEQ:
		case CEE_BNE_UN:
			code_bounds_check (5);
			do_branch_op (&ctx, (gint32)read32 (ip + 1) + 5, cmp_br_eq_op);
			ip += 5;
			need_merge = 1;
			break;

		case CEE_BGE:
		case CEE_BGT:
		case CEE_BLE:
		case CEE_BLT:
		case CEE_BGE_UN:
		case CEE_BGT_UN:
		case CEE_BLE_UN:
		case CEE_BLT_UN:
			code_bounds_check (5);
			do_branch_op (&ctx, (gint32)read32 (ip + 1) + 5, cmp_br_op);
			ip += 5;
			need_merge = 1;
			break;

		case CEE_LDLOC_S:
		case CEE_LDLOCA_S:
			code_bounds_check (2);
			push_local (&ctx, ip[1], *ip == CEE_LDLOCA_S);
			ip += 2;
			break;

		case CEE_UNUSED99:
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Use of the `unused' opcode"));
			++ip;
			break; 

		case CEE_DUP: {
			ILStackDesc * top;
			if (!check_underflow (&ctx, 1))
				break;
			if (!check_overflow (&ctx))
				break;
			top = stack_pop_safe (&ctx);
			copy_stack_value (stack_push (&ctx), top); 
			copy_stack_value (stack_push (&ctx), top);
			++ip;
			break;
		}

		case CEE_JMP:
			code_bounds_check (5);
			if (ctx.eval.size)
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Eval stack must be empty in jmp at 0x%04x", ip_offset));
			token = read32 (ip + 1);
			if (in_any_block (ctx.header, ip_offset))
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("jmp cannot escape exception blocks at 0x%04x", ip_offset));

			CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("Intruction jmp is not verifiable at 0x%04x", ctx.ip_offset));
			/*
			 * FIXME: check signature, retval, arguments etc.
			 */
			ip += 5;
			break;
		case CEE_CALL:
		case CEE_CALLVIRT:
			code_bounds_check (5);
			do_invoke_method (&ctx, read32 (ip + 1), *ip == CEE_CALLVIRT);
			ip += 5;
			break;

		case CEE_CALLI:
			code_bounds_check (5);
			token = read32 (ip + 1);
			/*
			 * FIXME: check signature, retval, arguments etc.
			 * FIXME: check requirements for tail call
			 */
			CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("Intruction calli is not verifiable at 0x%04x", ctx.ip_offset));
			ip += 5;
			break;
		case CEE_BR_S:
			code_bounds_check (2);
			do_static_branch (&ctx, (signed char)ip [1] + 2);
			need_merge = 1;
			ip += 2;
			start = 1;
			break;

		case CEE_BRFALSE_S:
		case CEE_BRTRUE_S:
			code_bounds_check (2);
			do_boolean_branch_op (&ctx, (signed char)ip [1] + 2);
			ip += 2;
			need_merge = 1;
			break;

		case CEE_BR:
			code_bounds_check (5);
			do_static_branch (&ctx, (gint32)read32 (ip + 1) + 5);
			need_merge = 1;
			ip += 5;
			start = 1;
			break;

		case CEE_BRFALSE:
		case CEE_BRTRUE:
			code_bounds_check (5);
			do_boolean_branch_op (&ctx, (gint32)read32 (ip + 1) + 5);
			ip += 5;
			need_merge = 1;
			break;

		case CEE_SWITCH: {
			guint32 entries;
			code_bounds_check (5);
			entries = read32 (ip + 1);

			if (entries > 0xFFFFFFFFU / sizeof (guint32))
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Too many switch entries %x at 0x%04x", entries, ctx.ip_offset));

			ip += 5;
			code_bounds_check (sizeof (guint32) * entries);
			
			do_switch (&ctx, entries, ip);
			ip += sizeof (guint32) * entries;
			break;
		}
		case CEE_LDIND_I1:
		case CEE_LDIND_U1:
		case CEE_LDIND_I2:
		case CEE_LDIND_U2:
		case CEE_LDIND_I4:
		case CEE_LDIND_U4:
		case CEE_LDIND_I8:
		case CEE_LDIND_I:
		case CEE_LDIND_R4:
		case CEE_LDIND_R8:
		case CEE_LDIND_REF:
			do_load_indirect (&ctx, *ip);
			++ip;
			break;
			
		case CEE_STIND_REF:
		case CEE_STIND_I1:
		case CEE_STIND_I2:
		case CEE_STIND_I4:
		case CEE_STIND_I8:
		case CEE_STIND_R4:
		case CEE_STIND_R8:
		case CEE_STIND_I:
			do_store_indirect (&ctx, *ip);
			++ip;
			break;

		case CEE_NOT:
		case CEE_NEG:
			do_unary_math_op (&ctx, *ip);
			++ip;
			break;

		case CEE_CONV_I1:
		case CEE_CONV_I2:
		case CEE_CONV_I4:
		case CEE_CONV_U1:
		case CEE_CONV_U2:
		case CEE_CONV_U4:
			do_conversion (&ctx, TYPE_I4);
			++ip;
			break;			

		case CEE_CONV_I8:
		case CEE_CONV_U8:
			do_conversion (&ctx, TYPE_I8);
			++ip;
			break;			

		case CEE_CONV_R4:
		case CEE_CONV_R8:
		case CEE_CONV_R_UN:
			do_conversion (&ctx, TYPE_R8);
			++ip;
			break;			

		case CEE_CONV_I:
		case CEE_CONV_U:
			do_conversion (&ctx, TYPE_NATIVE_INT);
			++ip;
			break;

		case CEE_CPOBJ:
			code_bounds_check (5);
			do_cpobj (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_LDOBJ:
			code_bounds_check (5);
			do_ldobj_value (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_LDSTR:
			code_bounds_check (5);
			do_ldstr (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_NEWOBJ:
			code_bounds_check (5);
			do_newobj (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_CASTCLASS:
		case CEE_ISINST:
			code_bounds_check (5);
			do_cast (&ctx, read32 (ip + 1), *ip == CEE_CASTCLASS ? "castclass" : "isinst");
			ip += 5;
			break;

		case CEE_UNUSED58:
		case CEE_UNUSED1:
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Use of the `unused' opcode"));
			++ip;
			break;

		case CEE_UNBOX:
			code_bounds_check (5);
			do_unbox_value (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_THROW:
			do_throw (&ctx);
			start = 1;
			++ip;
			break;

		case CEE_LDFLD:
		case CEE_LDFLDA:
			code_bounds_check (5);
			do_push_field (&ctx, read32 (ip + 1), *ip == CEE_LDFLDA);
			ip += 5;
			break;

		case CEE_LDSFLD:
		case CEE_LDSFLDA:
			code_bounds_check (5);
			do_push_static_field (&ctx, read32 (ip + 1), *ip == CEE_LDSFLDA);
			ip += 5;
			break;

		case CEE_STFLD:
			code_bounds_check (5);
			do_store_field (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_STSFLD:
			code_bounds_check (5);
			do_store_static_field (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_STOBJ:
			code_bounds_check (5);
			do_stobj (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_CONV_OVF_I1_UN:
		case CEE_CONV_OVF_I2_UN:
		case CEE_CONV_OVF_I4_UN:
		case CEE_CONV_OVF_U1_UN:
		case CEE_CONV_OVF_U2_UN:
		case CEE_CONV_OVF_U4_UN:
			do_conversion (&ctx, TYPE_I4);
			++ip;
			break;			

		case CEE_CONV_OVF_I8_UN:
		case CEE_CONV_OVF_U8_UN:
			do_conversion (&ctx, TYPE_I8);
			++ip;
			break;			

		case CEE_CONV_OVF_I_UN:
		case CEE_CONV_OVF_U_UN:
			do_conversion (&ctx, TYPE_NATIVE_INT);
			++ip;
			break;

		case CEE_BOX:
			code_bounds_check (5);
			do_box_value (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_NEWARR:
			code_bounds_check (5);
			do_newarr (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_LDLEN:
			do_ldlen (&ctx);
			++ip;
			break;

		case CEE_LDELEMA:
			code_bounds_check (5);
			do_ldelema (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_LDELEM_I1:
		case CEE_LDELEM_U1:
		case CEE_LDELEM_I2:
		case CEE_LDELEM_U2:
		case CEE_LDELEM_I4:
		case CEE_LDELEM_U4:
		case CEE_LDELEM_I8:
		case CEE_LDELEM_I:
		case CEE_LDELEM_R4:
		case CEE_LDELEM_R8:
		case CEE_LDELEM_REF:
			do_ldelem (&ctx, *ip, 0);
			++ip;
			break;

		case CEE_STELEM_I:
		case CEE_STELEM_I1:
		case CEE_STELEM_I2:
		case CEE_STELEM_I4:
		case CEE_STELEM_I8:
		case CEE_STELEM_R4:
		case CEE_STELEM_R8:
		case CEE_STELEM_REF:
			do_stelem (&ctx, *ip, 0);
			++ip;
			break;

		case CEE_LDELEM:
			code_bounds_check (5);
			do_ldelem (&ctx, *ip, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_STELEM:
			code_bounds_check (5);
			do_stelem (&ctx, *ip, read32 (ip + 1));
			ip += 5;
			break;
			
		case CEE_UNBOX_ANY:
			code_bounds_check (5);
			do_unbox_any (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_CONV_OVF_I1:
		case CEE_CONV_OVF_U1:
		case CEE_CONV_OVF_I2:
		case CEE_CONV_OVF_U2:
		case CEE_CONV_OVF_I4:
		case CEE_CONV_OVF_U4:
			do_conversion (&ctx, TYPE_I4);
			++ip;
			break;

		case CEE_CONV_OVF_I8:
		case CEE_CONV_OVF_U8:
			do_conversion (&ctx, TYPE_I8);
			++ip;
			break;

		case CEE_CONV_OVF_I:
		case CEE_CONV_OVF_U:
			do_conversion (&ctx, TYPE_NATIVE_INT);
			++ip;
			break;

		case CEE_REFANYVAL:
			code_bounds_check (5);
			do_refanyval (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_CKFINITE:
			do_ckfinite (&ctx);
			++ip;
			break;

		case CEE_MKREFANY:
			code_bounds_check (5);
			do_mkrefany (&ctx,  read32 (ip + 1));
			ip += 5;
			break;

		case CEE_LDTOKEN:
			code_bounds_check (5);
			do_load_token (&ctx, read32 (ip + 1));
			ip += 5;
			break;

		case CEE_ENDFINALLY:
			if (!is_correct_endfinally (ctx.header, ip_offset))
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("endfinally must be used inside a finally/fault handler at 0x%04x", ctx.ip_offset));
			ctx.eval.size = 0;
			start = 1;
			++ip;
			break;

		case CEE_LEAVE:
			code_bounds_check (5);
			do_leave (&ctx, read32 (ip + 1) + 5);
			ip += 5;
			start = 1;
			need_merge = 1;
			break;

		case CEE_LEAVE_S:
			code_bounds_check (2);
			do_leave (&ctx, (signed char)ip [1] + 2);
			ip += 2;
			start = 1;
			need_merge = 1;
			break;

		case CEE_PREFIX1:
			code_bounds_check (2);
			++ip;
			switch (*ip) {
			case CEE_STLOC:
				code_bounds_check (3);
				store_local (&ctx, read16 (ip + 1));
				ip += 3;
				break;

			case CEE_CEQ:
				do_cmp_op (&ctx, cmp_br_eq_op, *ip);
				++ip;
				break;

			case CEE_CGT:
			case CEE_CGT_UN:
			case CEE_CLT:
			case CEE_CLT_UN:
				do_cmp_op (&ctx, cmp_br_op, *ip);
				++ip;
				break;

			case CEE_STARG:
				code_bounds_check (3);
				store_arg (&ctx, read16 (ip + 1) );
				ip += 3;
				break;


			case CEE_ARGLIST:
				if (!check_overflow (&ctx))
					break;
				if (ctx.signature->call_convention != MONO_CALL_VARARG)
					ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Cannot use arglist on method without VARGARG calling convention at 0x%04x", ctx.ip_offset));
				set_stack_value (&ctx, stack_push (&ctx), &mono_defaults.argumenthandle_class->byval_arg, FALSE);
				++ip;
				break;
	
			case CEE_LDFTN:
				code_bounds_check (5);
				do_load_function_ptr (&ctx, read32 (ip + 1), FALSE);
				ip += 5;
				break;

			case CEE_LDVIRTFTN:
				code_bounds_check (5);
				do_load_function_ptr (&ctx, read32 (ip + 1), TRUE);
				ip += 5;
				break;

			case CEE_LDARG:
			case CEE_LDARGA:
				code_bounds_check (3);
				push_arg (&ctx, read16 (ip + 1),  *ip == CEE_LDARGA);
				ip += 3;
				break;

			case CEE_LDLOC:
			case CEE_LDLOCA:
				code_bounds_check (3);
				push_local (&ctx, read16 (ip + 1), *ip == CEE_LDLOCA);
				ip += 3;
				break;

			case CEE_LOCALLOC:
				do_localloc (&ctx);
				++ip;
				break;

			case CEE_UNUSED56:
			case CEE_UNUSED57:
			case CEE_UNUSED70:
			case CEE_UNUSED:
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Use of the `unused' opcode"));
				++ip;
				break;
			case CEE_ENDFILTER:
				do_endfilter (&ctx);
				start = 1;
				++ip;
				break;
			case CEE_UNALIGNED_:
				code_bounds_check (2);
				prefix |= PREFIX_UNALIGNED;
				ip += 2;
				break;
			case CEE_VOLATILE_:
				prefix |= PREFIX_VOLATILE;
				++ip;
				break;
			case CEE_TAIL_:
				prefix |= PREFIX_TAIL;
				++ip;
				if (ip < end && (*ip != CEE_CALL && *ip != CEE_CALLI && *ip != CEE_CALLVIRT))
					ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("tail prefix must be used only with call opcodes at 0x%04x", ip_offset));
				break;

			case CEE_INITOBJ:
				code_bounds_check (5);
				do_initobj (&ctx, read32 (ip + 1));
				ip += 5;
				break;

			case CEE_CONSTRAINED_:
				code_bounds_check (5);
				ctx.constrained_type = get_boxable_mono_type (&ctx, read32 (ip + 1), "constrained.");
				prefix |= PREFIX_CONSTRAINED;
				ip += 5;
				break;
	
			case CEE_READONLY_:
				prefix |= PREFIX_READONLY;
				ip++;
				break;

			case CEE_CPBLK:
				CLEAR_PREFIX (&ctx, PREFIX_UNALIGNED | PREFIX_VOLATILE);
				if (!check_underflow (&ctx, 3))
					break;
				CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("Instruction cpblk is not verifiable at 0x%04x", ctx.ip_offset));
				ip++;
				break;
				
			case CEE_INITBLK:
				CLEAR_PREFIX (&ctx, PREFIX_UNALIGNED | PREFIX_VOLATILE);
				if (!check_underflow (&ctx, 3))
					break;
				CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("Instruction initblk is not verifiable at 0x%04x", ctx.ip_offset));
				ip++;
				break;
				
			case CEE_NO_:
				ip += 2;
				break;
			case CEE_RETHROW:
				if (!is_correct_rethrow (ctx.header, ip_offset))
					ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("rethrow must be used inside a catch handler at 0x%04x", ctx.ip_offset));
				ctx.eval.size = 0;
				start = 1;
				++ip;
				break;

			case CEE_SIZEOF:
				code_bounds_check (5);
				do_sizeof (&ctx, read32 (ip + 1));
				ip += 5;
				break;

			case CEE_REFANYTYPE:
				do_refanytype (&ctx);
				++ip;
				break;

			default:
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid instruction FE %x at 0x%04x", *ip, ctx.ip_offset));
				++ip;
			}
			break;

		default:
			ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid instruction %x at 0x%04x", *ip, ctx.ip_offset));
			++ip;
		}

		/*TODO we can fast detect a forward branch or exception block targeting code after prefix, we should fail fast*/
		if (prefix) {
			if (!ctx.prefix_set) //first prefix
				ctx.code [ctx.ip_offset].flags |= IL_CODE_FLAG_SEEN;
			ctx.prefix_set |= prefix;
			ctx.has_flags = TRUE;
			prefix = 0;
		} else {
			if (!ctx.has_flags)
				ctx.code [ctx.ip_offset].flags |= IL_CODE_FLAG_SEEN;

			if (ctx.prefix_set & PREFIX_CONSTRAINED)
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid instruction after constrained prefix at 0x%04x", ctx.ip_offset));
			if (ctx.prefix_set & PREFIX_READONLY)
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid instruction after readonly prefix at 0x%04x", ctx.ip_offset));
			if (ctx.prefix_set & PREFIX_VOLATILE)
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid instruction after volatile prefix at 0x%04x", ctx.ip_offset));
			if (ctx.prefix_set & PREFIX_UNALIGNED)
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Invalid instruction after unaligned prefix at 0x%04x", ctx.ip_offset));
			ctx.prefix_set = prefix = 0;
			ctx.has_flags = FALSE;
		}
	}
	/*
	 * if ip != end we overflowed: mark as error.
	 */
	if ((ip != end || !start) && ctx.verifiable && !ctx.list) {
		ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Run ahead of method code at 0x%04x", ip_offset));
	}

	/*We should guard against the last decoded opcode, otherwise we might add errors that doesn't make sense.*/
	for (i = 0; i < ctx.code_size && i < ip_offset; ++i) {
		if (ctx.code [i].flags & IL_CODE_FLAG_WAS_TARGET) {
			if (!(ctx.code [i].flags & IL_CODE_FLAG_SEEN))
				ADD_VERIFY_ERROR (&ctx, g_strdup_printf ("Branch or exception block target middle of intruction at 0x%04x", i));

			if (ctx.code [i].flags & IL_CODE_DELEGATE_SEQUENCE)
				CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("Branch to delegate code sequence at 0x%04x", i));
		}
		if ((ctx.code [i].flags & IL_CODE_LDFTN_DELEGATE_NONFINAL_VIRTUAL) && ctx.has_this_store)
			CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("Invalid ldftn with virtual function in method with stdarg 0 at  0x%04x", i));

		if ((ctx.code [i].flags & IL_CODE_CALL_NONFINAL_VIRTUAL) && ctx.has_this_store)
			CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("Invalid call to a non-final virtual function in method with stdarg.0 or ldarga.0 at  0x%04x", i));
	}

	if (mono_method_is_constructor (ctx.method) && !ctx.super_ctor_called && !ctx.method->klass->valuetype && ctx.method->klass != mono_defaults.object_class)
		CODE_NOT_VERIFIABLE (&ctx, g_strdup_printf ("Constructor not calling super\n"));

cleanup:
	if (ctx.code) {
		for (i = 0; i < ctx.header->code_size; ++i) {
			if (ctx.code [i].stack)
				g_free (ctx.code [i].stack);
		}
	}

	for (tmp = ctx.funptrs; tmp; tmp = tmp->next)
		g_free (tmp->data);
	g_slist_free (ctx.funptrs);

	for (tmp = ctx.exception_types; tmp; tmp = tmp->next)
		mono_metadata_free_type (tmp->data);
	g_slist_free (ctx.exception_types);

	for (i = 0; i < ctx.num_locals; ++i) {
		if (ctx.locals [i])
			mono_metadata_free_type (ctx.locals [i]);
	}
	for (i = 0; i < ctx.max_args; ++i) {
		if (ctx.params [i])
			mono_metadata_free_type (ctx.params [i]);
	}

	if (ctx.eval.stack)
		g_free (ctx.eval.stack);
	if (ctx.code)
		g_free (ctx.code);
	g_free (ctx.locals);
	g_free (ctx.params);
	mono_basic_block_free (original_bb);

	return ctx.list;