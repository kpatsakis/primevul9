ProcessorGenerator::ProcessorGenerator(t_cpp_generator* generator,
                                       t_service* service,
                                       const string& style)
  : generator_(generator),
    service_(service),
    f_header_(generator->f_header_),
    f_out_(generator->gen_templates_ ? generator->f_service_tcc_ : generator->f_service_),
    service_name_(generator->service_name_),
    style_(style) {
  if (style_ == "Cob") {
    pstyle_ = "Async";
    class_name_ = service_name_ + pstyle_ + "Processor";
    if_name_ = service_name_ + "CobSvIf";

    finish_cob_ = "tcxx::function<void(bool ok)> cob, ";
    finish_cob_decl_ = "tcxx::function<void(bool ok)>, ";
    cob_arg_ = "cob, ";
    ret_type_ = "void ";
  } else {
    class_name_ = service_name_ + "Processor";
    if_name_ = service_name_ + "If";

    ret_type_ = "bool ";
    // TODO(edhall) callContext should eventually be added to TAsyncProcessor
    call_context_ = ", void* callContext";
    call_context_arg_ = ", callContext";
    call_context_decl_ = ", void*";
  }

  factory_class_name_ = class_name_ + "Factory";

  if (generator->gen_templates_) {
    template_header_ = "template <class Protocol_>\n";
    template_suffix_ = "<Protocol_>";
    typename_str_ = "typename ";
    class_name_ += "T";
    factory_class_name_ += "T";
  }

  if (service_->get_extends() != NULL) {
    extends_ = type_name(service_->get_extends()) + pstyle_ + "Processor";
    if (generator_->gen_templates_) {
      // TODO(simpkins): If gen_templates_ is enabled, we currently assume all
      // parent services were also generated with templates enabled.
      extends_ += "T<Protocol_>";
    }
  }
}