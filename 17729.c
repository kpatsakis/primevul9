static bool is_prohibited_package_slow(Symbol* class_name) {
  // Caller has ResourceMark
  int length;
  jchar* unicode = class_name->as_unicode(length);
  return (length >= 5 &&
          unicode[0] == 'j' &&
          unicode[1] == 'a' &&
          unicode[2] == 'v' &&
          unicode[3] == 'a' &&
          unicode[4] == '/');
}