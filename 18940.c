_find_by_keyword(smartlist_t *s, directory_keyword keyword,
                 const char *keyword_as_string)
{
  directory_token_t *tok = find_opt_by_keyword(s, keyword);
  if (PREDICT_UNLIKELY(!tok)) {
    log_err(LD_BUG, "Missing %s [%d] in directory object that should have "
         "been validated. Internal error.", keyword_as_string, (int)keyword);
    tor_assert(tok);
  }
  return tok;
}