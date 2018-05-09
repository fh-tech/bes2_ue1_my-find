//
// Created by vik on 09.05.18.
//

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <memory.h>
#include <ctype.h>

#include "search.h"

void printFoundFile(char *pid, char *filename, char *abs_path) {
    printf("%s: %s: %s\n", pid, filename, abs_path);
}

char *get_absPath(char *dirname, char *name) {
    char *combined_name = calloc(strlen(dirname) + strlen(name) + 3, 1);
    strcat(combined_name, dirname);
    strcat(combined_name, "/");
    strcat(combined_name, name);

    char * real_path = realpath(combined_name, NULL);
    free(combined_name);
    //TODO: review how can this work - program can not know how long char * for real_path will be!
    return real_path;
}

int compare_filenames(char *string1, char *string2, int case_insensitive) {
    size_t len1 = strlen(string1);
    size_t len2 = strlen(string2);
    if(len1 != len2) return 0;

    for(int i = 0; i < len1; i++) {
        char c1 = case_insensitive ? (char)tolower(string1[i]) : string1[i];
        char c2 = case_insensitive ? (char)tolower(string2[i]) : string2[i];
        if(c1 != c2) return 0;
    }
    // all were equal
    return 1;
}

void searchFile(const char *dirname, char *toSearch, int recursive, int case_insensitive) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(dirname))) {
        fprintf(stderr, "Failed to open directory ");
        perror(dirname);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char *filename = entry->d_name;

        switch (entry->d_type) {
            case DT_DIR: {
                if (recursive) {
                    if (!strcmp(filename, ".") == 0 && !strcmp(filename, "..") == 0) {
                        searchFile(filename, toSearch, case_insensitive, recursive);
                    }
                }
                break;
            }
            case DT_REG: {
                int equal = compare_filenames(filename, toSearch, case_insensitive);
                if (equal) {
                    char *abs_path = get_absPath(dirname, filename);
                    printFoundFile("", filename, abs_path);
                    free(abs_path);
                }
                break;
            }
            default:
                break;
        }
    }
    // EINTR is error code for disrupted -- dont forget to close the file
    while ((closedir(dir) == -1) && (errno == EINTR));
}