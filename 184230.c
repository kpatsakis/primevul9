check_call_reference(CallNode* cn, ScanEnv* env, int state)
{
  MemEnv* mem_env = SCANENV_MEMENV(env);

  if (cn->by_number != 0) {
    int gnum = cn->called_gnum;

    if (env->num_named > 0 &&
        IS_SYNTAX_BV(env->syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
        ! OPTON_CAPTURE_GROUP(env->options)) {
      return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;
    }

    if (gnum > env->num_mem) {
      onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_GROUP_REFERENCE,
                                     cn->name, cn->name_end);
      return ONIGERR_UNDEFINED_GROUP_REFERENCE;
    }

  set_call_attr:
    NODE_CALL_BODY(cn) = mem_env[cn->called_gnum].mem_node;
    if (IS_NULL(NODE_CALL_BODY(cn))) {
      onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_NAME_REFERENCE,
                                     cn->name, cn->name_end);
      return ONIGERR_UNDEFINED_NAME_REFERENCE;
    }

    NODE_STATUS_ADD(NODE_CALL_BODY(cn), REFERENCED);
  }
  else {
    int *refs;

    int n = onig_name_to_group_numbers(env->reg, cn->name, cn->name_end, &refs);
    if (n <= 0) {
      onig_scan_env_set_error_string(env, ONIGERR_UNDEFINED_NAME_REFERENCE,
                                     cn->name, cn->name_end);
      return ONIGERR_UNDEFINED_NAME_REFERENCE;
    }
    else if (n > 1) {
      onig_scan_env_set_error_string(env, ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL,
                                     cn->name, cn->name_end);
      return ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL;
    }
    else {
      cn->called_gnum = refs[0];
      goto set_call_attr;
    }
  }

  return 0;
}