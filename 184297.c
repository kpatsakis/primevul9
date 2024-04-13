onig_compile(regex_t* reg, const UChar* pattern, const UChar* pattern_end,
             OnigErrorInfo* einfo)
{
  int r;
  Node*  root;
  ScanEnv  scan_env;
#ifdef USE_CALL
  UnsetAddrList  uslist = {0};
#endif

  root = 0;
  if (IS_NOT_NULL(einfo)) {
    einfo->enc = reg->enc;
    einfo->par = (UChar* )NULL;
  }

#ifdef ONIG_DEBUG
  fprintf(DBGFP, "\nPATTERN: /");
  print_enc_string(DBGFP, reg->enc, pattern, pattern_end);
#endif

  if (reg->ops_alloc == 0) {
    r = ops_init(reg, OPS_INIT_SIZE);
    if (r != 0) goto end;
  }
  else
    reg->ops_used = 0;

  r = onig_parse_tree(&root, pattern, pattern_end, reg, &scan_env);
  if (r != 0) goto err;

  r = reduce_string_list(root, reg->enc);
  if (r != 0) goto err;

  /* mixed use named group and no-named group */
  if (scan_env.num_named > 0 &&
      IS_SYNTAX_BV(scan_env.syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
      ! OPTON_CAPTURE_GROUP(reg->options)) {
    if (scan_env.num_named != scan_env.num_mem)
      r = disable_noname_group_capture(&root, reg, &scan_env);
    else
      r = numbered_ref_check(root);

    if (r != 0) goto err;
  }

  r = check_backrefs(root, &scan_env);
  if (r != 0) goto err;

#ifdef USE_CALL
  if (scan_env.num_call > 0) {
    r = unset_addr_list_init(&uslist, scan_env.num_call);
    if (r != 0) goto err;
    scan_env.unset_addr_list = &uslist;
    r = tune_call(root, &scan_env, 0);
    if (r != 0) goto err_unset;
    r = tune_call2(root);
    if (r != 0) goto err_unset;
    r = recursive_call_check_trav(root, &scan_env, 0);
    if (r  < 0) goto err_unset;
    r = infinite_recursive_call_check_trav(root, &scan_env);
    if (r != 0) goto err_unset;

    tune_called_state(root, 0);
  }

  reg->num_call = scan_env.num_call;
#endif

#ifdef ONIG_DEBUG_PARSE
  fprintf(DBGFP, "MAX PARSE DEPTH: %d\n", scan_env.max_parse_depth);
  fprintf(DBGFP, "TREE (parsed)\n");
  print_tree(DBGFP, root);
  fprintf(DBGFP, "\n");
#endif

  r = tune_tree(root, reg, 0, &scan_env);
  if (r != 0) goto err_unset;

  if (scan_env.backref_num != 0) {
    set_parent_node_trav(root, NULL_NODE);
    r = set_empty_repeat_node_trav(root, NULL_NODE, &scan_env);
    if (r != 0) goto err_unset;
    set_empty_status_check_trav(root, &scan_env);
  }

#ifdef ONIG_DEBUG_PARSE
  fprintf(DBGFP, "TREE (after tune)\n");
  print_tree(DBGFP, root);
  fprintf(DBGFP, "\n");
#endif

  reg->capture_history = scan_env.cap_history;
  reg->push_mem_start  = scan_env.backtrack_mem | scan_env.cap_history;

#ifdef USE_CALLOUT
  if (IS_NOT_NULL(reg->extp) && reg->extp->callout_num != 0) {
    reg->push_mem_end = reg->push_mem_start;
  }
  else {
    if (MEM_STATUS_IS_ALL_ON(reg->push_mem_start))
      reg->push_mem_end = scan_env.backrefed_mem | scan_env.cap_history;
    else
      reg->push_mem_end = reg->push_mem_start &
                        (scan_env.backrefed_mem | scan_env.cap_history);
  }
#else
  if (MEM_STATUS_IS_ALL_ON(reg->push_mem_start))
    reg->push_mem_end = scan_env.backrefed_mem | scan_env.cap_history;
  else
    reg->push_mem_end = reg->push_mem_start &
                      (scan_env.backrefed_mem | scan_env.cap_history);
#endif

  clear_optimize_info(reg);
#ifndef ONIG_DONT_OPTIMIZE
  r = set_optimize_info_from_tree(root, reg, &scan_env);
  if (r != 0) goto err_unset;
#endif

  if (IS_NOT_NULL(scan_env.mem_env_dynamic)) {
    xfree(scan_env.mem_env_dynamic);
    scan_env.mem_env_dynamic = (MemEnv* )NULL;
  }

  r = compile_tree(root, reg, &scan_env);
  if (r == 0) {
    if (scan_env.keep_num > 0) {
      r = add_op(reg, OP_UPDATE_VAR);
      if (r != 0) goto err;

      COP(reg)->update_var.type = UPDATE_VAR_KEEP_FROM_STACK_LAST;
      COP(reg)->update_var.id   = 0; /* not used */
      COP(reg)->update_var.clear = FALSE;
    }

    r = add_op(reg, OP_END);
    if (r != 0) goto err;

#ifdef USE_CALL
    if (scan_env.num_call > 0) {
      r = fix_unset_addr_list(&uslist, reg);
      unset_addr_list_end(&uslist);
      if (r != 0) goto err;
    }
#endif

    set_addr_in_repeat_range(reg);

    if ((reg->push_mem_end != 0)
#ifdef USE_REPEAT_AND_EMPTY_CHECK_LOCAL_VAR
        || (reg->num_repeat      != 0)
        || (reg->num_empty_check != 0)
#endif
#ifdef USE_CALLOUT
        || (IS_NOT_NULL(reg->extp) && reg->extp->callout_num != 0)
#endif
#ifdef USE_CALL
        || scan_env.num_call > 0
#endif
        )
      reg->stack_pop_level = STACK_POP_LEVEL_ALL;
    else {
      if (reg->push_mem_start != 0)
        reg->stack_pop_level = STACK_POP_LEVEL_MEM_START;
      else
        reg->stack_pop_level = STACK_POP_LEVEL_FREE;
    }

    r = ops_make_string_pool(reg);
    if (r != 0) goto err;
  }
#ifdef USE_CALL
  else if (scan_env.num_call > 0) {
    unset_addr_list_end(&uslist);
  }
#endif
  onig_node_free(root);

#ifdef ONIG_DEBUG_COMPILE
  onig_print_names(DBGFP, reg);
  onig_print_compiled_byte_code_list(DBGFP, reg);
#endif

#ifdef USE_DIRECT_THREADED_CODE
  /* opcode -> opaddr */
  onig_init_for_match_at(reg);
#endif

 end:
  return r;

 err_unset:
#ifdef USE_CALL
  if (scan_env.num_call > 0) {
    unset_addr_list_end(&uslist);
  }
#endif
 err:
  if (IS_NOT_NULL(scan_env.error)) {
    if (IS_NOT_NULL(einfo)) {
      einfo->par     = scan_env.error;
      einfo->par_end = scan_env.error_end;
    }
  }

  onig_node_free(root);
  if (IS_NOT_NULL(scan_env.mem_env_dynamic))
      xfree(scan_env.mem_env_dynamic);
  return r;
}