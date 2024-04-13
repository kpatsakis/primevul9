  template <Type type> static RegistrationMap& mutableRegistrationMap() {
    MUTABLE_CONSTRUCT_ON_FIRST_USE(RegistrationMap);
  }