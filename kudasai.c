#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#include <string.h>

char *insert_path = NULL, *remove_path = NULL;
char *base_path = "/usr/share/kudasai/";

void play_mp3_filepath(const char *path) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "play \"$(find \"%s\" -type f | shuf -n 1)\">/dev/null 2>&1", path);
    system(cmd);
}

void list_sound_dirs() {
    char cmd[512];
    printf("Valid options for use with -s (insert/remove pair):\n");
    snprintf(cmd, sizeof(cmd), "find \"%s\" -mindepth 1 -type d -exec sh -c 'if [ -f \"{}/insert.mp3\" ] && [ -f \"{}/remove.mp3\" ]; then basename \"{}\" ; fi' \\;", base_path);
    system(cmd);
}

int hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev, libusb_hotplug_event event, void *_) {
    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED){
        if (insert_path != NULL) play_mp3_filepath(insert_path);
    }
    else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        if (remove_path != NULL) play_mp3_filepath(remove_path);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1 || argv[1][0] != '-') {
        printf(
            "Usage: kudasai [OPTION]... [FILE]...\n"
            "-f <path> Path for both insertion and removal sounds\n"
            "-i <path> Path for USB insertion sound\n"
            "-r <path> Path for USB removal sound\n"
            "-s <name> Name of premade audio to use.\n"
            "-l List premade audio packs to use with -s\n"
        );
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] != '-') continue;
        
        switch (arg[1]) {
            case 'f':
                if (++i < argc) insert_path = remove_path = argv[i];
                else return printf("please add a file path\n"), 1;
                break;
            case 'i':
                if (++i < argc) insert_path = argv[i];
                else return printf("please add a file path\n"), 1;
                break;
            case 'r':
                if (++i < argc) remove_path = argv[i];
                else return printf("please add a file path\n"), 1;
                break;
            case 's':
                if (++i < argc) {
                    char *pack_name = argv[i];
                    char insert_buffer[512];
                    char remove_buffer[512];
                    
                    snprintf(insert_buffer, sizeof(insert_buffer), "%s%s/insert.mp3", base_path, pack_name);
                    snprintf(remove_buffer, sizeof(remove_buffer), "%s%s/remove.mp3", base_path, pack_name);
                    
                    insert_path = strdup(insert_buffer);
                    remove_path = strdup(remove_buffer);
                } else return printf("please add a pack name. run kudasai -l to get options\n"), 1;
                break;
            case 'l':
                list_sound_dirs();
                return 0;
            default:
                return printf("invalid flag!\n"), 1;
        }
    }

    libusb_context *ctx = NULL;
    libusb_hotplug_callback_handle cb;
    libusb_init(&ctx);
    libusb_hotplug_register_callback(ctx, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL, &cb);
    
    while (1) libusb_handle_events_completed(ctx, NULL);
    return 0;
}
