int mongo_env_close_socket( int socket ) {
#ifdef _WIN32
    return closesocket( socket );
#else
    return close( socket );
#endif
}