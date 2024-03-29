#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define WRONG_BEHAVIOUR 1
#define OPEN_ERROR -1
#define SEEK_ERROR -1
#define DIR_CREATE_ERROR -1

void reverse_string(char* str);
void reverse_directory_name(const char* src, char* dest);
void reverse_buffer(char* buffer, size_t bytes_read);
int copy_reverse_content(off_t pos, int src_file, int dest_file, char* buffer);
int reverse_file(const char* src, const char* dest);
int recursive_directory_reverse(const char* src, const char* dest, DIR* dir);
int reverse_directory(const char* src, const char* dest);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: <source directory>\n");
        return EXIT_FAILURE;
    }

    char* src = argv[1];
    char dest[PATH_MAX];
    strcpy(dest, src);
    reverse_string(dest);

    int return_value = reverse_directory(src, dest);

    return return_value;
}

void reverse_string(char* str) {
    char* last_part = strrchr(str, '/');
    if (last_part != NULL) {
        last_part++;
        size_t len = strlen(last_part);
        char temp;
        for (size_t i = 0; i < len / 2; i++) {
            temp = last_part[i];
            last_part[i] = last_part[len - i - 1];
            last_part[len - i - 1] = temp;
        }
    }
}

void reverse_directory_name(const char* src, char* dest) {
    strcpy(dest, src);
    reverse_string(dest);
}

void reverse_buffer char* buffer, size_t bytes_read) {
    for (unsigned int i = 0; i < bytes_read / 2; i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[bytes_read - 1 - i];
        buffer[bytes_read - 1 - i] = tmp;
    }
}

int copy_reverse_content(off_t pos, int src_file, int dest_file, char* buffer) {
    int return_value = EXIT_SUCCESS;
    while (pos > 0) {
        off_t read_start = pos - BUFFER_SIZE > 0 ? pos - BUFFER_SIZE : 0;
        size_t bytes_to_read = pos - read_start;
        size_t bytes_to_write = bytes_to_read;
        size_t lseek_return = lseek(src_file, read_start, SEEK_SET);
        if (lseek_return == SEEK_ERROR) {
            perror("Error seeking in file\n");
            return_value = EXIT_FAILURE;
        }

        ssize_t bytes_read = read(src_file, buffer, bytes_to_read);
        if (bytes_read != bytes_to_read) {
            perror("Error reading source file\n");
            return_value = EXIT_FAILURE;
        }

        reverse_buffer(buffer, bytes_read);
        ssize_t bytes_wrote = write(dest_file, buffer, bytes_to_write);
        if (bytes_wrote != bytes_to_write) {
            perror("Error writing destination file\n");
            return_value = EXIT_FAILURE;
        }

        pos = read_start;
    }
    return return_value;
}

int reverse_file(const char* src, const char* dest) {
    int return_value = EXIT_SUCCESS;
    int src_file = open(src, O_RDONLY);
    if (src_file == OPEN_ERROR) {
        perror("Error opening source file\n");
        return EXIT_FAILURE;
    }

    struct stat st;
    int fstat_status = fstat(src_file, &st);
    if (fstat_status == OPEN_ERROR) {
        perror("Error getting source file permissions");
        close(src_file);
        return EXIT_FAILURE;
    }

    int dest_file = open(dest, O_WRONLY | O_CREAT, st.st_mode);
    if (dest_file == OPEN_ERROR) {
        perror("Error opening destination file\n");
        close(src_file);
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];

    off_t pos = lseek(src_file, 0, SEEK_END);
    if (pos == SEEK_ERROR) {
        perror("Error seeking in file\n");
        close(src_file);
        close(dest_file);
        return EXIT_FAILURE;
    }


    return_value = copy_reverse_content(pos, src_file, dest_file, buffer);

    close(src_file);
    close(dest_file);
    return return_value;
}

int recursive_directory_reverse(const char* src, const char* dest, DIR* dir) {
    int return_value = EXIT_SUCCESS;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[PATH_MAX];
        char dest_path[PATH_MAX];
        snprintf(src_path, PATH_MAX, "%s/%s", src, entry->d_name);
        snprintf(dest_path, PATH_MAX, "%s/%s", dest, entry->d_name);

        struct stat st;
        int stat_status = stat(src_path, &st);
        if (stat_status == OPEN_ERROR) {
            perror("Error reading file or directory\n");
            return_value = EXIT_FAILURE;
            break;
        }

        if (S_ISDIR(st.st_mode)) {
            char reversed_dir[PATH_MAX];
            reverse_directory_name(dest_path, reversed_dir);
            int reverse_result = reverse_directory(src_path, reversed_dir);
            if (reverse_result == WRONG_BEHAVIOUR) {
                return_value = EXIT_FAILURE;
                break;
            }
        }
        else if (S_ISREG(st.st_mode)) {
            reverse_string(dest_path);
            int reverse_result = reverse_file(src_path, dest_path);
            if (reverse_result == WRONG_BEHAVIOUR) {
                return_value = EXIT_FAILURE;
                break;
            }
        }
    }
    return return_value;
}

int reverse_directory(const char* src, const char* dest) {
    int return_value = EXIT_SUCCESS;
    DIR* dir = opendir(src);
    if (dir == NULL) {
        perror("Error opening source directory\n");
        return EXIT_FAILURE;
    }
    struct stat dir_info;
    int stat_status = stat(src, &dir_info);
    if (stat_status == OPEN_ERROR) {
        perror("Error reading directory\n");
        return EXIT_FAILURE;
    }
    int mkdir_result = mkdir(dest, dir_info.st_mode);
    if (mkdir_result == DIR_CREATE_ERROR) {
        perror("Error creating destination directory\n");
        closedir(dir);
        return EXIT_FAILURE;
    }

    return_value = recursive_directory_reverse(src, dest, dir);

    closedir(dir);
    return return_value;
}
