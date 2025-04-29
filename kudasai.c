#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#include <string.h>

const char *insert_path = NULL, *remove_path = NULL, *rand_dir = NULL;
void play_random_mp3(const char *dirpath) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "play \"%s/$(ls %s | shuf -n 1)\" >/dev/null 2>&1", dirpath, dirpath);
    system(cmd);
}

void play_mp3_filepath(const char *filepath) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "play \"%s\" >/dev/null 2>&1", filepath);
    system(cmd);
}

int hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev, libusb_hotplug_event event, void *_) {
    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
        use_windows ? play_mp3_filepath("/home/$USER/kudasai/windows/insert.mp3") : insert_path ? play_mp3_filepath(insert_path) : rand_dir ? play_random_mp3(rand_dir) : 0;
    else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)
        use_windows ? play_mp3_filepath("/home/$USER/kudasai/windows/remove.mp3") : remove_path ? play_mp3_filepath(remove_path) : rand_dir ? play_random_mp3(rand_dir) : 0;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf(
            "Usage: kudasai [OPTION]... [FILE]...\n"

            "For the following:"
            "A lowercase letter means one singular file."
            "A capital letter means a directory to pull a random file from."
            "Absolute or relative file paths are both ok"

            "-f -F <path>   Path for both insertion and removal sounds"
            "-i -I <path>   Path for USB insertion sound\n"
            "-r -R <path>   Path for USB removal sound\n"
        );
        return 0;
    }    

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-f") && i + 1 < argc) rand_dir = argv[++i];
        else if (!strcmp(argv[i], "-i") && i + 1 < argc) insert_path = argv[++i];
        else if (!strcmp(argv[i], "-r") && i + 1 < argc) remove_path = argv[++i];
    }

    libusb_context *ctx = NULL;
    libusb_hotplug_callback_handle cb;
    libusb_init(&ctx);
    libusb_hotplug_register_callback(ctx, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL, &cb);
    while (1) libusb_handle_events_completed(ctx, NULL);
    return 0;
}
