std::shared_ptr<se::DeviceMemoryAllocator> GetAllocator(
    DeviceBase* device, se::Stream* stream,
    const XlaPlatformInfo& platform_info) {
  if (platform_info.custom_allocator()) {
    return platform_info.custom_allocator();
  }
  auto* alloc = device->GetAllocator({});
  if (!stream) {
    // Stream is not set for the host platform.
    se::Platform* platform =
        se::MultiPlatformManager::PlatformWithId(platform_info.platform_id())
            .ValueOrDie();
    return std::make_shared<se::TfAllocatorAdapter>(alloc, platform);
  }
  return std::make_shared<se::TfAllocatorAdapter>(alloc, stream);
}