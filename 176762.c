static void generate_password(std::string *password, int size) {
  std::stringstream ss;
  bool srnd;
  constexpr const char *prefix = "yx9!A-";
  ss << prefix;
  size -= strlen(prefix);
  while (size > 0) {
    int ch = ((int)(my_rnd_ssl(&srnd) * 100)) % get_allowed_pwd_chars().size();
    ss << get_allowed_pwd_chars()[ch];
    --size;
  }
  password->assign(ss.str());
}