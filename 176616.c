  void Inspect::operator()(Number_Ptr n)
  {

    std::string res;

    // reduce units
    n->reduce();

    // check if the fractional part of the value equals to zero
    // neat trick from http://stackoverflow.com/a/1521682/1550314
    // double int_part; bool is_int = modf(value, &int_part) == 0.0;

    // this all cannot be done with one run only, since fixed
    // output differs from normal output and regular output
    // can contain scientific notation which we do not want!

    // first sample
    std::stringstream ss;
    ss.precision(12);
    ss << n->value();

    // check if we got scientific notation in result
    if (ss.str().find_first_of("e") != std::string::npos) {
      ss.clear(); ss.str(std::string());
      ss.precision(std::max(12, opt.precision));
      ss << std::fixed << n->value();
    }

    std::string tmp = ss.str();
    size_t pos_point = tmp.find_first_of(".,");
    size_t pos_fract = tmp.find_last_not_of("0");
    bool is_int = pos_point == pos_fract ||
                  pos_point == std::string::npos;

    // reset stream for another run
    ss.clear(); ss.str(std::string());

    // take a shortcut for integers
    if (is_int)
    {
      ss.precision(0);
      ss << std::fixed << n->value();
      res = std::string(ss.str());
    }
    // process floats
    else
    {
      // do we have have too much precision?
      if (pos_fract < opt.precision + pos_point)
      { ss.precision((int)(pos_fract - pos_point)); }
      else { ss.precision(opt.precision); }
      // round value again
      ss << std::fixed << n->value();
      res = std::string(ss.str());
      // maybe we truncated up to decimal point
      size_t pos = res.find_last_not_of("0");
      // handle case where we have a "0"
      if (pos == std::string::npos) {
        res = "0.0";
      } else {
        bool at_dec_point = res[pos] == '.' ||
                            res[pos] == ',';
        // don't leave a blank point
        if (at_dec_point) ++ pos;
        res.resize (pos + 1);
      }
    }

    // some final cosmetics
    if (res == "0.0") res = "0";
    else if (res == "") res = "0";
    else if (res == "-0") res = "0";
    else if (res == "-0.0") res = "0";
    else if (opt.output_style == COMPRESSED)
    {
      // check if handling negative nr
      size_t off = res[0] == '-' ? 1 : 0;
      // remove leading zero from floating point in compressed mode
      if (n->zero() && res[off] == '0' && res[off+1] == '.') res.erase(off, 1);
    }

    // add unit now
    res += n->unit();

    // output the final token
    append_token(res, n);
  }