BSONObj PipelineD::getPostBatchResumeToken(const Pipeline* pipeline) {
    if (auto docSourceCursor =
            dynamic_cast<DocumentSourceCursor*>(pipeline->_sources.front().get())) {
        return docSourceCursor->getPostBatchResumeToken();
    }
    return BSONObj{};
}