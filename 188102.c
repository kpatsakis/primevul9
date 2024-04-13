propname2ctype(ScanEnv* env, const char* propname)
{
  UChar* name = (UChar* )propname;
  UChar* name_end = name + strlen(propname);
  int ctype = env->enc->property_name_to_ctype(ONIG_ENCODING_ASCII,
      name, name_end);
  if (ctype < 0) {
    onig_scan_env_set_error_string(env, ctype, name, name_end);
  }
  return ctype;
}