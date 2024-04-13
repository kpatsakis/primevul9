AP_DECLARE(worker_score *) ap_get_scoreboard_worker_from_indexes(int x, int y)
{
    if (((x < 0) || (x >= server_limit)) ||
        ((y < 0) || (y >= thread_limit))) {
        return(NULL); /* Out of range */
    }
    return &ap_scoreboard_image->servers[x][y];
}