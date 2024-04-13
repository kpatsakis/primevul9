StringData extractGeoNearFieldFromIndexes(OperationContext* opCtx,
                                          const CollectionPtr& collection) {
    invariant(collection);

    std::vector<const IndexDescriptor*> idxs;
    collection->getIndexCatalog()->findIndexByType(opCtx, IndexNames::GEO_2D, idxs);
    uassert(ErrorCodes::IndexNotFound,
            str::stream() << "There is more than one 2d index on " << collection->ns().ns()
                          << "; unsure which to use for $geoNear",
            idxs.size() <= 1U);
    if (idxs.size() == 1U) {
        for (auto&& elem : idxs.front()->keyPattern()) {
            if (elem.type() == BSONType::String && elem.valueStringData() == IndexNames::GEO_2D) {
                return elem.fieldNameStringData();
            }
        }
        MONGO_UNREACHABLE;
    }

    // If there are no 2d indexes, look for a 2dsphere index.
    idxs.clear();
    collection->getIndexCatalog()->findIndexByType(opCtx, IndexNames::GEO_2DSPHERE, idxs);
    uassert(ErrorCodes::IndexNotFound,
            "$geoNear requires a 2d or 2dsphere index, but none were found",
            !idxs.empty());
    uassert(ErrorCodes::IndexNotFound,
            str::stream() << "There is more than one 2dsphere index on " << collection->ns().ns()
                          << "; unsure which to use for $geoNear",
            idxs.size() <= 1U);

    invariant(idxs.size() == 1U);
    for (auto&& elem : idxs.front()->keyPattern()) {
        if (elem.type() == BSONType::String && elem.valueStringData() == IndexNames::GEO_2DSPHERE) {
            return elem.fieldNameStringData();
        }
    }
    MONGO_UNREACHABLE;
}