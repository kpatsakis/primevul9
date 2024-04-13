  static std::string getVersionedMethodPath(const std::string& service_full_name_template,
                                            absl::string_view method_name,
                                            envoy::config::core::v3::ApiVersion api_version,
                                            bool use_alpha = false,
                                            const std::string& service_namespace = EMPTY_STRING) {
    return absl::StrCat("/",
                        getVersionedServiceFullName(service_full_name_template, api_version,
                                                    use_alpha, service_namespace),
                        "/", method_name);
  }