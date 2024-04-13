int mongo_env_close_socket( int socket ) {
    return closesocket( socket );
}