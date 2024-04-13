MATCHER_P(DecodedResourcesEq, expected, "") {
  const bool equal = std::equal(arg.begin(), arg.end(), expected.begin(), expected.end(),
                                TestUtility::decodedResourceEq);
  if (!equal) {
    const auto format_resources =
        [](const std::vector<Config::DecodedResourceRef>& resources) -> std::string {
      std::vector<std::string> resource_strs;
      std::transform(
          resources.begin(), resources.end(), std::back_inserter(resource_strs),
          [](const Config::DecodedResourceRef& resource) -> std::string {
            return fmt::format(
                "<name: {}, aliases: {}, version: {}, resource: {}>", resource.get().name(),
                absl::StrJoin(resource.get().aliases(), ","), resource.get().version(),
                resource.get().hasResource() ? resource.get().resource().DebugString() : "(none)");
          });
      return absl::StrJoin(resource_strs, ", ");
    };
    *result_listener << "\n"
                     << TestUtility::addLeftAndRightPadding("Expected resources:") << "\n"
                     << format_resources(expected) << "\n"
                     << TestUtility::addLeftAndRightPadding("are not equal to actual resources:")
                     << "\n"
                     << format_resources(arg) << "\n"
                     << TestUtility::addLeftAndRightPadding("") // line full of padding
                     << "\n";
  }
  return equal;
}