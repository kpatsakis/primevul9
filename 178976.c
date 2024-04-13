static bool is_string_in_set(set<string>& s, string h) {
  if ((s.find("*") != s.end()) || 
          (s.find(h) != s.end())) {
    return true;
  }
  /* The header can be Content-*-type, or Content-* */
  for(set<string>::iterator it = s.begin();
      it != s.end(); ++it) {
    size_t off;
    if ((off = (*it).find("*"))!=string::npos) {
      list<string> ssplit;
      unsigned flen = 0;
      
      get_str_list((*it), "* \t", ssplit);
      if (off != 0) {
        string sl = ssplit.front();
        flen = sl.length();
        dout(10) << "Finding " << sl << ", in " << h << ", at offset 0" << dendl;
        if (!boost::algorithm::starts_with(h,sl))
          continue;
        ssplit.pop_front();
      }
      if (off != ((*it).length() - 1)) {
        string sl = ssplit.front();
        dout(10) << "Finding " << sl << ", in " << h 
          << ", at offset not less than " << flen << dendl;
        if (h.size() < sl.size() ||
	    h.compare((h.size() - sl.size()), sl.size(), sl) != 0)
          continue;
        ssplit.pop_front();
      }
      if (!ssplit.empty())
        continue;
      return true;
    }
  }
  return false;
}