parse_char_property(Node** np, OnigToken* tok, UChar** src, UChar* end,
		    ScanEnv* env)
{
  int r, ctype;
  CClassNode* cc;

  ctype = fetch_char_property_to_ctype(src, end, env);
  if (ctype < 0) return ctype;

  *np = node_new_cclass();
  CHECK_NULL_RETURN_MEMERR(*np);
  cc = CCLASS_(*np);
  r = add_ctype_to_cc(cc, ctype, 0, env);
  if (r != 0) return r;
  if (tok->u.prop.not != 0) NCCLASS_SET_NOT(cc);

  return 0;
}