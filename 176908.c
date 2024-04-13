RoleName::RoleName(StringData role, StringData dbname) {
    _fullName.resize(role.size() + dbname.size() + 1);
    std::string::iterator iter =
        std::copy(role.rawData(), role.rawData() + role.size(), _fullName.begin());
    *iter = '@';
    ++iter;
    iter = std::copy(dbname.rawData(), dbname.rawData() + dbname.size(), iter);
    dassert(iter == _fullName.end());
    _splitPoint = role.size();
}