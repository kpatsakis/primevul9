void RoleName::_serializeToSubObj(BSONObjBuilder* sub) const {
    sub->append(AuthorizationManager::ROLE_NAME_FIELD_NAME, getRole());
    sub->append(AuthorizationManager::ROLE_DB_FIELD_NAME, getDB());
}