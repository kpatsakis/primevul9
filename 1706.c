int64_t GetDnnWorkspaceLimit(const string& envvar_in_mb,
                             int64_t default_value_in_bytes) {
  const char* workspace_limit_in_mb_str = getenv(envvar_in_mb.c_str());
  if (workspace_limit_in_mb_str != nullptr &&
      strcmp(workspace_limit_in_mb_str, "") != 0) {
    int64_t scratch_limit_in_mb = -1;
    if (strings::safe_strto64(workspace_limit_in_mb_str,
                              &scratch_limit_in_mb)) {
      return scratch_limit_in_mb * (1 << 20);
    } else {
      LOG(WARNING) << "Invalid value for env-var " << envvar_in_mb << ": "
                   << workspace_limit_in_mb_str;
    }
  }
  return default_value_in_bytes;
}