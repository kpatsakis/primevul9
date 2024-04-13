BuilderPtr createBuilder(Protobuf::Arena* arena) {
  google::api::expr::runtime::InterpreterOptions options;

  // Security-oriented defaults
  options.enable_comprehension = false;
  options.enable_regex = true;
  options.regex_max_program_size = 100;
  options.enable_string_conversion = false;
  options.enable_string_concat = false;
  options.enable_list_concat = false;

  // Enable constant folding (performance optimization)
  if (arena != nullptr) {
    options.constant_folding = true;
    options.constant_arena = arena;
  }

  auto builder = google::api::expr::runtime::CreateCelExpressionBuilder(options);
  auto register_status =
      google::api::expr::runtime::RegisterBuiltinFunctions(builder->GetRegistry(), options);
  if (!register_status.ok()) {
    throw CelException(
        absl::StrCat("failed to register built-in functions: ", register_status.message()));
  }
  return builder;
}