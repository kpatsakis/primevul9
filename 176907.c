void RoleName::serializeToBSON(BSONArrayBuilder* bob) const {
    BSONObjBuilder sub(bob->subobjStart());
    _serializeToSubObj(&sub);
}