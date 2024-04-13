void RGWCORSRule::format_exp_headers(string& s) {
  s = "";
  for (const auto& header : exposable_hdrs) {
    if (s.length() > 0)
      s.append(",");
    // these values are sent to clients in a 'Access-Control-Expose-Headers'
    // response header, so we escape '\n' to avoid header injection
    boost::replace_all_copy(std::back_inserter(s), header, "\n", "\\n");
  }
}