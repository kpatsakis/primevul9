  RegisterCustomInlineHeader(const LowerCaseString& header)
      : handle_(CustomInlineHeaderRegistry::registerInlineHeader<type>(header)) {}