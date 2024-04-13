onig_reg_init(regex_t* reg, OnigOptionType option, OnigCaseFoldType case_fold_flag,
              OnigEncoding enc, OnigSyntaxType* syntax)
{
  int r;

  xmemset(reg, 0, sizeof(*reg));

  if (onig_inited == 0) {
#if 0
    return ONIGERR_LIBRARY_IS_NOT_INITIALIZED;
#else
    r = onig_initialize(&enc, 1);
    if (r != 0)
      return ONIGERR_FAIL_TO_INITIALIZE;

    onig_warning("You didn't call onig_initialize() explicitly");
#endif
  }

  if (IS_NULL(reg))
    return ONIGERR_INVALID_ARGUMENT;

  if (ONIGENC_IS_UNDEF(enc))
    return ONIGERR_DEFAULT_ENCODING_IS_NOT_SETTED;

  if ((option & (ONIG_OPTION_DONT_CAPTURE_GROUP|ONIG_OPTION_CAPTURE_GROUP))
      == (ONIG_OPTION_DONT_CAPTURE_GROUP|ONIG_OPTION_CAPTURE_GROUP)) {
    return ONIGERR_INVALID_COMBINATION_OF_OPTIONS;
  }

  if ((option & ONIG_OPTION_NEGATE_SINGLELINE) != 0) {
    option |= syntax->options;
    option &= ~ONIG_OPTION_SINGLELINE;
  }
  else
    option |= syntax->options;

  (reg)->enc              = enc;
  (reg)->options          = option;
  (reg)->syntax           = syntax;
  (reg)->optimize         = 0;
  (reg)->exact            = (UChar* )NULL;
  (reg)->int_map          = (int* )NULL;
  (reg)->int_map_backward = (int* )NULL;
  REG_EXTPL(reg) = NULL;

  (reg)->p                = (UChar* )NULL;
  (reg)->alloc            = 0;
  (reg)->used             = 0;
  (reg)->name_table       = (void* )NULL;

  (reg)->case_fold_flag   = case_fold_flag;
  return 0;
}