  template <Type type> static const RegistrationMap& headers() {
    ASSERT(mutableFinalized<type>());
    return mutableRegistrationMap<type>();
  }