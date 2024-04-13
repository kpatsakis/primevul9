std::ostream& operator<<(std::ostream& os, const RoleName& name) {
    return os << name.getFullName();
}