    static size_t size() {
      // The size of the lookup table is finalized when the singleton lookup table is created. This
      // allows for late binding of custom headers as well as envoy header prefix changes. This
      // does mean that once the first header map is created of this type, no further changes are
      // possible.
      // TODO(mattklein123): If we decide to keep this implementation, it is conceivable that header
      // maps could be created by an API factory that is owned by the listener/HCM, thus making
      // O(1) header delivery over xDS possible.
      return ConstSingleton<StaticLookupTable>::get().size_;
    }