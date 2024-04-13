  Expression_Obj Parser::lex_almost_any_value_chars()
  {
    const char* match =
    lex <
      one_plus <
        alternatives <
          sequence <
            exactly <'\\'>,
            any_char
          >,
          sequence <
            negate <
              sequence <
                exactly < url_kwd >,
                exactly <'('>
              >
            >,
            neg_class_char <
              almost_any_value_class
            >
          >,
          sequence <
            exactly <'/'>,
            negate <
              alternatives <
                exactly <'/'>,
                exactly <'*'>
              >
            >
          >,
          sequence <
            exactly <'\\'>,
            exactly <'#'>,
            negate <
              exactly <'{'>
            >
          >,
          sequence <
            exactly <'!'>,
            negate <
              alpha
            >
          >
        >
      >
    >(false);
    if (match) {
      return SASS_MEMORY_NEW(String_Constant, pstate, lexed);
    }
    return {};
  }