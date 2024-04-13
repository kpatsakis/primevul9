getSortAndGroupStagesFromPipeline(const Pipeline::SourceContainer& sources) {
    boost::intrusive_ptr<DocumentSourceSort> sortStage = nullptr;
    boost::intrusive_ptr<DocumentSourceGroup> groupStage = nullptr;

    auto sourcesIt = sources.begin();
    if (sourcesIt != sources.end()) {
        sortStage = dynamic_cast<DocumentSourceSort*>(sourcesIt->get());
        if (sortStage) {
            if (!sortStage->hasLimit()) {
                ++sourcesIt;
            } else {
                // This $sort stage was previously followed by a $limit stage.
                sourcesIt = sources.end();
            }
        }
    }

    if (sourcesIt != sources.end()) {
        groupStage = dynamic_cast<DocumentSourceGroup*>(sourcesIt->get());
    }

    return std::make_pair(sortStage, groupStage);
}