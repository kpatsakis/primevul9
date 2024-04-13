onig_detect_can_be_slow_pattern(const UChar* pattern,
  const UChar* pattern_end, OnigOptionType option, OnigEncoding enc,
  OnigSyntaxType* syntax)
{
  int r;
  regex_t* reg;
  Node* root;
  ScanEnv scan_env;
  SlowElementCount count;

  reg = (regex_t* )xmalloc(sizeof(regex_t));
  if (IS_NULL(reg)) return ONIGERR_MEMORY;

  r = onig_reg_init(reg, option, ONIGENC_CASE_FOLD_DEFAULT, enc, syntax);
  if (r != 0) {
    xfree(reg);
    return r;
  }

  root = 0;
  r = onig_parse_tree(&root, pattern, pattern_end, reg, &scan_env);
  if (r == 0) {
    count.prec_read          = 0;
    count.look_behind        = 0;
    count.backref_with_level = 0;
    count.call               = 0;

    r = node_detect_can_be_slow(root, &count);
    if (r == 0) {
      int n = count.prec_read + count.look_behind
            + count.backref_with_level + count.call;
      r = n;
    }
  }

  if (IS_NOT_NULL(scan_env.mem_env_dynamic))
    xfree(scan_env.mem_env_dynamic);

  onig_node_free(root);
  onig_free(reg);
  return r;
}