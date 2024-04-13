std::pair<DocumentSourceSample*, DocumentSourceInternalUnpackBucket*> extractSampleUnpackBucket(
    const Pipeline::SourceContainer& sources) {
    DocumentSourceSample* sampleStage = nullptr;
    DocumentSourceInternalUnpackBucket* unpackStage = nullptr;

    auto sourcesIt = sources.begin();
    if (sourcesIt != sources.end()) {
        sampleStage = dynamic_cast<DocumentSourceSample*>(sourcesIt->get());
        if (sampleStage) {
            return std::pair{sampleStage, unpackStage};
        }

        unpackStage = dynamic_cast<DocumentSourceInternalUnpackBucket*>(sourcesIt->get());
        ++sourcesIt;

        if (unpackStage && sourcesIt != sources.end()) {
            sampleStage = dynamic_cast<DocumentSourceSample*>(sourcesIt->get());
            return std::pair{sampleStage, unpackStage};
        }
    }

    return std::pair{sampleStage, unpackStage};
}