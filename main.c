#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include "react.h"

typedef struct FileList {
    int len;
    char *name_buffer;
    char **name;
    int *is_dir;
} FileList;

FileList sort_file(FileList list)
{
    for (int y = 0; y < list.len - 1; y++) {
        for (int i = 0; i < list.len - y - 1; i++) {
            if ((!list.is_dir[i] && list.is_dir[i+1]) || (list.name[i][0] > list.name[i+1][0])) {
                int swapi = list.is_dir[i];
                list.is_dir[i] = list.is_dir[i+1];
                list.is_dir[i+1] = swapi;

                char *swap = list.name[i];
                list.name[i] = list.name[i+1];
                list.name[i+1] = swap;
            }
        }
    }
    return list;
}

FileList list_file(const char *path)
{
    FileList list = {0};

    struct dirent *entry;
    DIR *dp;
    struct stat file_stat;

    dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return list;
    }

    int list_cap = 10;
    int buffer_cap = list_cap * 8;
    list.len = 0;
    // NOTE: malloc/calloc/realloc/free is shit
    // TODO: use mmap, probably making an arena
    list.name_buffer = calloc(buffer_cap, sizeof(list.name_buffer[0]));
    list.name  = calloc(list_cap, sizeof(list.name[0]));
    list.is_dir = calloc(list_cap, sizeof(list.is_dir[0]));

    list.name[0] = list.name_buffer;

    while ((entry = readdir(dp)) != NULL) {
        if (stat(entry->d_name, &file_stat) == -1)
            continue;
        if (entry->d_name[0] == '.' && entry->d_name[1] == '\0')
            continue;

        if ((list.len >= list_cap))
            break;
        list.is_dir[list.len] = S_ISDIR(file_stat.st_mode);

        int file_len = strlen(entry->d_name) + 1;
        if (list.name[list.len] + file_len * sizeof(list.name_buffer[0]) > (list.name_buffer + buffer_cap * sizeof(list.name_buffer[0])))
            break;

        strcpy(list.name[list.len], entry->d_name);
        list.name[list.len+1] = list.name[list.len] + file_len * sizeof(list.name_buffer[0]);

        list.len++;
    }

    closedir(dp);
    return sort_file(list);
}

void free_file_list(FileList list)
{
    free(list.name_buffer);
    free(list.name);
    free(list.is_dir);
}

void le_gros_la_ba(int i)
{
    h1("get='le_gros_la' style='color: red'") {
        textf("gros caca nb %d", i);
    }
}

void le_gros_la()
{
    static int count = 0;
    count++;
    char txt[100];
    snprintf(txt, 100, "get='le_gros_la_ba_%d'", count);
    h1(txt) {
        textf("hihiha: %d", count);
    }
}

void the_page()
{
    html_base_template("gros caca") {
        ul("") {
            FileList list = list_file(".");
            for (int i = 0; i < list.len; i++) {
                li("") {
                    if (list.is_dir[i])
                        textf("[dire] %s", list.name[i]);
                    else
                        textf("[file] %s", list.name[i]);
                }
            }
            // free_file_list(list);
        }
        html_main("style='text-align: center; background-color: blue'") {
            range (0, 5) {
                le_gros_la_ba(i);
            }
        }
    }
}

int main()
{
    open_server(6969);

    while (1) {
        accept_connection();
        read_request();

        char path[50];
        int path_len = get_path(path, 50);
        // printf("[%.*s]\n", path_len, path);

        if (url_match("/le_gros_la", path)) {
            http_request_code(200);
            le_gros_la();
        }
        else if (url_begin_with("/le_gros_la_ba", path)) {
            http_request_code(200);
            int l = sizeof("/le_gros_la_ba");
            le_gros_la_ba(atoi(path + l));
        }
        else if (url_match("/", path)) {
            http_request_code(200);
            the_page();
        }
        else {
            http_request_code(404);
            html_base_template("page introuvable") {
                h1("") {
                    text("Err: page introuvable");
                }
            }
        }

        write_request();
        close_connection();
    }

    close_server();
    return 0;
}
