string t_cpp_generator::get_include_prefix(const t_program& program) const {
  string include_prefix = program.get_include_prefix();
  if (!use_include_prefix_ || (include_prefix.size() > 0 && include_prefix[0] == '/')) {
    // if flag is turned off or this is absolute path, return empty prefix
    return "";
  }

  string::size_type last_slash = string::npos;
  if ((last_slash = include_prefix.rfind("/")) != string::npos) {
    return include_prefix.substr(0, last_slash)
           + (get_program()->is_out_path_absolute() ? "/" : "/" + out_dir_base_ + "/");
  }

  return "";
}