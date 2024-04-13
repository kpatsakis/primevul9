DynamicMetadataMapIterator::DynamicMetadataMapIterator(DynamicMetadataMapWrapper& parent)
    : parent_{parent}, current_{parent_.streamInfo().dynamicMetadata().filter_metadata().begin()} {}