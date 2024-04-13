XlaPlatformInfo XlaPlatformInfoFromDevice(DeviceBase* device_base) {
  auto device = static_cast<Device*>(device_base);
  se::Platform::Id platform_id = nullptr;
  const XlaDevice::Metadata* xla_device_metadata = nullptr;
  std::shared_ptr<se::DeviceMemoryAllocator> custom_allocator;

  if (device->device_type() == DEVICE_CPU) {
    platform_id = se::host::kHostPlatformId;
  } else if (device->device_type() == DEVICE_GPU) {
    platform_id = device->tensorflow_gpu_device_info()
                      ->stream->parent()
                      ->platform()
                      ->id();
  } else if (XlaDevice::GetMetadataFromDevice(device, &xla_device_metadata)
                 .ok()) {
    // If we are on an XlaDevice, use the underlying XLA platform's allocator
    // directly. We could use the StreamExecutor's allocator which may
    // theoretically be more correct, but XLA returns a nice OOM message in a
    // Status and StreamExecutor does not.
    //
    // Importantly we can't use ctx->device()->GetAllocator() as the allocator
    // (which xla_allocator above uses) as on an XlaDevice, this is a dummy
    // allocator that returns XlaTensor objects. The XlaCompiler needs a real
    // allocator to allocate real buffers.
    platform_id = xla_device_metadata->platform()->id();
    custom_allocator =
        xla_device_metadata->client()->backend().shared_memory_allocator();
  }

  return XlaPlatformInfo(DeviceType(device->device_type()), platform_id,
                         xla_device_metadata, custom_allocator);
}