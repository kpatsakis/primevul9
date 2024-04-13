XlaCompiler::Options GenerateCompilerOptions(
    const XlaCompilationCache& cache,
    const FunctionLibraryRuntime& function_library, DeviceBase* device,
    se::Stream* stream, const XlaPlatformInfo& platform_info,
    bool has_ref_vars) {
  XlaCompiler::Options options;
  options.client = static_cast<xla::LocalClient*>(cache.client());
  if (stream != nullptr) {
    options.device_ordinal = stream->parent()->device_ordinal();
  }
  options.device_type = cache.device_type();
  options.flib_def = function_library.GetFunctionLibraryDefinition();
  options.graph_def_version = function_library.graph_def_version();
  options.allow_cpu_custom_calls =
      (platform_info.platform_id() == se::host::kHostPlatformId);
  options.device_allocator = GetAllocator(device, stream, platform_info);
  if (platform_info.xla_device_metadata()) {
    options.shape_determination_fns =
        platform_info.xla_device_metadata()->default_shape_determination_fns();
  }
  // If reference variables are not present in the graph, we can safely alias
  // passthrough parameters without performing a copy.
  options.alias_passthrough_params =
      !has_ref_vars && !platform_info.is_on_xla_device();
  return options;
}