void InstanceKlass::deallocate_record_components(ClassLoaderData* loader_data,
                                                 Array<RecordComponent*>* record_components) {
  if (record_components != NULL && !record_components->is_shared()) {
    for (int i = 0; i < record_components->length(); i++) {
      RecordComponent* record_component = record_components->at(i);
      MetadataFactory::free_metadata(loader_data, record_component);
    }
    MetadataFactory::free_array<RecordComponent*>(loader_data, record_components);
  }
}