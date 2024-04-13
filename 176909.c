void RoleName::serializeToBSON(StringData fieldName, BSONObjBuilder* bob) const {
    BSONObjBuilder sub(bob->subobjStart(fieldName));
    _serializeToSubObj(&sub);
}