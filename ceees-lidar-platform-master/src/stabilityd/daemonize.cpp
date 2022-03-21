#include <iostream>
#include <csignal>
#include <cstdio>
#include <functional>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

long daemon_exit(long exit_code) {
    return (exit_code & 0xff) - 127;
}

long daemon_wait(unsigned int skip_us, bool include_heartbeat=true) {
    if (skip_us == 0) return -128;
    if (include_heartbeat) {
        return skip_us + 128;
    } else {
        return -1 * skip_us - 128;
    }
}

long daemon_continue() {
    return daemon_wait(0, false);
}

static void _make_child() {
    pid_t pid = fork();
    if (pid > 0) {
        std::exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        std::cerr << "Could not fork.\n";
        std::exit(EXIT_FAILURE);
    }
}

static void _set_session_leader() {
    pid_t sid = setsid();
    if (sid < 0) {
        std::cerr << "Could not generate session ID.\n";
        std::exit(EXIT_FAILURE);
    }
}

static void _install_signal_handler() {
    struct sigaction ignored_signals;
    sigemptyset(&ignored_signals.sa_mask);
    sigaddset(&ignored_signals.sa_mask, SIGCHLD); // ignore children
    sigaddset(&ignored_signals.sa_mask, SIGHUP);  // ignore SIGHUP (shouldn't happen anyway) Many daemons will reload their configuration files and reopen their logfiles instead of exiting when receiving this signal.[4] nohup is a command to make a command ignore the signal. 
    sigaddset(&ignored_signals.sa_mask, SIGTSTP); // ignore TTY stop
    sigaddset(&ignored_signals.sa_mask, SIGTTOU); // ignore TTY bg write
    sigaddset(&ignored_signals.sa_mask, SIGTTIN); // ignore TTY bg read

    sigprocmask(SIG_BLOCK, &ignored_signals.sa_mask, nullptr); // ignore aforementioned

    // TODO: add signal handling for SIGTERM, SIGINT, etc. user-defined signals
}

static void _set_file_permissions() {
    umask(0);
}

static void _disown_working_directory() {
    chdir("/");
}

class _PIDFile {
public:
    _PIDFile(const char *daemon_name) : pid_file_path(fmt::format("/var/run/{}.pid", daemon_name)) {
        int pid_file_handle = open(pid_file_path.c_str(), O_RDWR|O_CREAT, 0600);
        if (pid_file_handle == -1) {
            std::cerr << "Could not open PID file.\n";
            std::exit(EXIT_FAILURE);
        }

        if (lockf(pid_file_handle, F_TLOCK, 0) == -1) {
            std::cerr << "Could not lock PID file.\n";
            std::exit(EXIT_FAILURE);
        }

        char pid_str[10];
        ssize_t c = write(pid_file_handle, pid_str, sprintf(pid_str, "%d\n", getpid()));
        if (c == -1) {
            std::cerr << "Could not write to PID file.\n";
            close(pid_file_handle);
            std::exit(EXIT_FAILURE);
        }
    }

    ~_PIDFile() {
        if (std::remove(pid_file_path.c_str()) != 0) {
            std::cerr << "Could not unlink PID file\n";
        }
    }

private:
    const std::string pid_file_path;
};

static void _close_std_streams() {
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

static auto _open_audit_log(const char *daemon_name) {
    auto audit = spdlog::get(daemon_name);
    if (!audit) {
        std::cerr << "Could not find a log named '" << daemon_name << "'.\n";
        std::exit(EXIT_FAILURE);
    }

    return audit;
}

static int _run_main_loop(const char *daemon_name, std::function<long(void)> heartbeat) {
    for (std::chrono::system_clock::time_point t = std::chrono::system_clock::now(); true; t = std::chrono::system_clock::now()) {

        // TODO: catch exceptions
        long return_value = heartbeat();

        if (return_value > 128) {
            std::this_thread::sleep_until(std::chrono::microseconds(return_value - 128) + t);
            continue;
        }

        if (-127 <= return_value && return_value <= 128) {
            return return_value + 127;
        }

        if (return_value <= -128) {
            std::this_thread::sleep_for(std::chrono::milliseconds(-1 * return_value - 128));
            continue;
        }
    }
}

int daemonize(const char *daemon_name, std::function<long(void)> heartbeat) {
    _make_child();
    _set_session_leader();
    _install_signal_handler();
    _make_child();
    _set_file_permissions();
    _disown_working_directory();
    _PIDFile p(daemon_name);
    auto audit = _open_audit_log(daemon_name);
    //_close_std_streams();
    audit->info("{} starting with PID {}", daemon_name, getpid());
    int return_value = _run_main_loop(daemon_name, heartbeat);
    audit->info("{} exiting with status code {}", daemon_name, return_value);
    return return_value;
}

