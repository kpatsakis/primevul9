  t_cpp_generator(t_program* program,
                  const std::map<std::string, std::string>& parsed_options,
                  const std::string& option_string)
    : t_oop_generator(program) {
    (void)option_string;
    std::map<std::string, std::string>::const_iterator iter;

    iter = parsed_options.find("pure_enums");
    gen_pure_enums_ = (iter != parsed_options.end());

    iter = parsed_options.find("dense");
    gen_dense_ = (iter != parsed_options.end());

    iter = parsed_options.find("include_prefix");
    use_include_prefix_ = (iter != parsed_options.end());

    iter = parsed_options.find("cob_style");
    gen_cob_style_ = (iter != parsed_options.end());

    iter = parsed_options.find("no_client_completion");
    gen_no_client_completion_ = (iter != parsed_options.end());

    iter = parsed_options.find("no_default_operators");
    gen_no_default_operators_ = (iter != parsed_options.end());

    iter = parsed_options.find("templates");
    gen_templates_ = (iter != parsed_options.end());

    gen_templates_only_ = (iter != parsed_options.end() && iter->second == "only");

    iter = parsed_options.find("moveable_types");
    gen_moveable_ = (iter != parsed_options.end());

    out_dir_base_ = "gen-cpp";
  }