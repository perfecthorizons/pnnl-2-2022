#include <functional>

extern long daemon_exit(long exit_code);
extern long daemon_wait(unsigned int skip_us, bool include_heartbeat=true);
extern long daemon_continue(); // Equivalent to daemon_wait(0);
extern int daemonize(const char *daemon_name, std::function<long(void)> heartbeat);

/*
template<typename state_type_t>
using heartbeat_t = std::function<long(state_type_t)>;

template<typename state_type_t>
int daemonize(const char *daemon_name, heartbeat_t<state_type_t> heartbeat, state_type_t state) {
    daemonize(daemon_name, [&heartbeat, &state](void) -> long { return heartbeat(state); });
}
*/
