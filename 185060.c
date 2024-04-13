    HeaderLocationSpec(const Http::LowerCaseString& header, const std::string& value_prefix)
        : header_(header), value_prefix_(value_prefix) {}