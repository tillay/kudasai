#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

volatile sig_atomic_t running = 1;

void handle_signal(int sig) {
    running = 0;
}

int get_usb_count(void) {
    libusb_context *ctx = NULL;
    libusb_device **devs = NULL;
    int count = -1;

    if (libusb_init(&ctx) == 0) {
        ssize_t cnt = libusb_get_device_list(ctx, &devs);
        if (cnt >= 0) count = (int)cnt;
        libusb_free_device_list(devs, 1);
        libusb_exit(ctx);
    }

    return count;
}

void play_mp3(const char *dirpath) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "mpg123 \"%s/$(ls %s | shuf -n 1)\" >/dev/null 2>&1", dirpath, dirpath);
    system(cmd);
}

int main(void) {
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);
    
    openlog("kudasai", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "kudasai started");
    
    int prev_count = get_usb_count();
    char* file_path = "/usr/share/kudasai";
    
    while (running) {
        int current_count = get_usb_count();
        if (current_count > prev_count) {
            syslog(LOG_INFO, "inserted >.<");
            play_mp3(file_path);
            prev_count = current_count;
        }
        if (current_count < prev_count) {
            syslog(LOG_INFO, "removed o.o");
            play_mp3(file_path);
            prev_count = current_count;
        }
    }
    
    syslog(LOG_INFO, "kudasai stopped");
    closelog();
    return 0;
}
