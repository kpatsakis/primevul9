static void urlParsePart(struct URL *url, const char *buf, int len) {
  // Most browsers seem to forget quoting data in the header fields. This
  // means, it is quite possible for an HTML form to cause the submission of
  // unparseable "multipart/form-data". If this happens, we just give up
  // and ignore the malformed data.
  // Example:
  // <form method="POST" enctype="multipart/form-data">
  //   <input type="file" name="&quot;&#13;&#10;X: x=&quot;">
  //   <input type="submit">
  // </form>
  char *name           = NULL;
  for (const char *eol; !!(eol = urlMemstr(buf, len, "\r\n")); ) {
    if (buf == eol) {
      buf             += 2;
      len             -= 2;
      if (name) {
        char *value    = len ? urlMakeString(buf, len) : NULL;
        addToHashMap(&url->args, name, value);
        name           = NULL;
      }
      break;
    } else {
      if (!name && !urlMemcasecmp(buf, len, "content-disposition:")) {
        struct HashMap fields;
        initHashMap(&fields, urlDestroyHashMapEntry, NULL);
        urlParseHeaderLine(&fields, buf + 20, eol - buf - 20);
        if (getRefFromHashMap(&fields, "form-data")) {
          // We currently don't bother to deal with binary files (e.g. files
          // that include NUL characters). If this ever becomes necessary,
          // we could check for the existence of a "filename" field and use
          // that as an indicator to store the payload in something other
          // than "url->args".
          name         = (char *)getFromHashMap(&fields, "name");
          if (name && *name) {
            check(name = strdup(name));
          }
        }
        destroyHashMap(&fields);
      }
      len             -= eol - buf + 2;
      buf              = eol + 2;
    }
  }
  free(name);
}