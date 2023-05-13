#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>//....................................................................temporary off...............................................................
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define WRONG_BEHAVIOUR EXIT_FAILURE
#define BUFFER_SIZE 4096
#define SEEK_ERROR -1
#define DELETE_ERROR -1
#define CREATE_ERROR -1
#define OPEN_ERROR -1
//#define ssize_t size_t//...............................................................temporary on................................................................
//#define mode_t unsigned short//........................................................temporary on................................................................
#define UNLINK_ERROR -1
#define SYMLINK_ERROR -1
#define HARDLINK_ERROR -1
#define STAT_ERROR -1
#define CHMODE_ERROR -1
#define RIGHT_BEHAVIOUR EXIT_SUCCESS
#define STR_DOES_MATCH RIGHT_BEHAVIOUR
#define STRTOL_ERROR 0

int parse_file(char* function, char* path, char* arguments, char* str) {
    char *tmp, *tmp2, tmp3[PATH_MAX];
    tmp2 = strtok(str, "/");
    memcpy(tmp3, tmp2, strlen(tmp2));
    while (tmp2 != NULL) {
        memcpy(tmp3, tmp2, strlen(tmp2));
        tmp2 = strtok(NULL, "/");
    }
    tmp = strtok(tmp3, " ");
    if (tmp == NULL) {
        fprintf(stderr, "Error in file parsing: no function name\n");
        return EXIT_FAILURE;
    }
    memcpy(function, tmp, PATH_MAX);
    tmp = strtok(NULL, " ");
    if (tmp == NULL) {
        fprintf(stderr, "Error in file parsing: no path name\n");
        return EXIT_FAILURE;
    }
    memcpy(path, tmp, PATH_MAX);
    tmp = strtok(NULL, " ");
    if (tmp != NULL) {
        memcpy(arguments, tmp, PATH_MAX);
    }
    else {
        str = NULL;
    }
    return EXIT_SUCCESS;
}

int create_directory(const char* path) {
    mode_t file_mode = 0755;
    int mkdir_status = mkdir(path, file_mode);
    if (mkdir_status == CREATE_ERROR) {
        perror("mkdir");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int print_directory(const char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
    return EXIT_SUCCESS;
}

int delete_directory(const char* path) {
    int return_value = EXIT_SUCCESS;
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    struct dirent *dir_info;
    char filepath[PATH_MAX];
    while ((dir_info = readdir(dir)) != NULL) {
        if (strcmp(dir_info->d_name, ".") == 0 || strcmp(dir_info->d_name, "..") == 0) {
            continue;
        }

        snprintf(filepath, sizeof(filepath), "%s/%s", path, dir_info->d_name);
        struct stat st;
        int stat_status = stat(filepath, &st);
        if (stat_status == STAT_ERROR) {
            perror("stat");
            return_value = EXIT_FAILURE;
            break;
        }

        if (S_ISDIR(st.st_mode)) {
            delete_directory(filepath);
        }
        else {
            int unlink_status = unlink(filepath);
            if (unlink_status == UNLINK_ERROR) {
                perror("unlink");
                return_value = EXIT_FAILURE;
                break;
            }
        }
    }

    closedir(dir);

    int rmdir_status = rmdir(path);
    if (rmdir_status == DELETE_ERROR) {
        perror("rmdir");
        return_value = EXIT_FAILURE;
    }
    return return_value;
}

int create_file(const char* path) {
    mode_t file_mode = 0644;
    int fd = open(path, O_CREAT, file_mode);
    if (fd == OPEN_ERROR) {
        perror("open");
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}

int read_write_analyse(ssize_t bytes_read_wrote, size_t bytes_to_read_write) {
    if (bytes_read_wrote == WRONG_BEHAVIOUR) {
        perror("Error reading source file\n");
        return EXIT_FAILURE;
    }
    if (bytes_read_wrote != bytes_to_read_write) {
        fprintf(stderr, "Error in reading/writing source file : bytes read/write isn't equal to bytes to read/write\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int print_file(const char* path) {
    int return_value = EXIT_SUCCESS;
    int file = open(path, O_RDONLY);
    if (file == OPEN_ERROR) {
        perror("open");
        return EXIT_FAILURE;
    }

    off_t pos = lseek(file, 0, SEEK_END);
    if (pos == SEEK_ERROR) {
        perror("Error seeking in file\n");
        close(file);
        return EXIT_FAILURE;
    }
    off_t num_chars = pos;
    pos = lseek(file, 0, SEEK_SET);

    char buffer[BUFFER_SIZE];
    while (pos<num_chars) {

        size_t bytes_to_read = ((num_chars - pos - BUFFER_SIZE) > 0) * BUFFER_SIZE  + (num_chars - pos) * !((num_chars - pos - BUFFER_SIZE) > 0);
        size_t bytes_to_write = bytes_to_read;

        ssize_t bytes_read = read(file, buffer, BUFFER_SIZE);

        int read_analyse_status = read_write_analyse(bytes_read, bytes_to_read);
        if (read_analyse_status == WRONG_BEHAVIOUR) {
            return_value = EXIT_FAILURE;
            break;
        }

        ssize_t bytes_wrote = write(1, buffer, bytes_to_write);
        int write_analyse_status = read_write_analyse(bytes_wrote, bytes_to_write);
        if (write_analyse_status == WRONG_BEHAVIOUR) {
            return_value = EXIT_FAILURE;
            break;
        }
        pos = lseek(file, 0, SEEK_CUR);
    }
    close(file);
    return return_value;
}

int delete_file(const char* path) {
    int unlink_status = unlink(path);
    if (unlink_status == UNLINK_ERROR) {
        perror("unlink");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int create_symlink(const char* path, const char* link_path) {
    int symlink_status = symlink(path, link_path);
    if (symlink_status == SYMLINK_ERROR) {
        perror("symlink");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int print_symlink(const char* link_path) {
    char buf[PATH_MAX];
    ssize_t len = readlink(link_path, buf, sizeof(buf) - 1);
    if (len == WRONG_BEHAVIOUR) {
        perror("readlink");
        return EXIT_FAILURE;
    }
    buf[len] = '\0';
    printf("%s\n", buf);
    return EXIT_SUCCESS;
}

int print_symlink_content(const char* link_path) {
    int return_value = EXIT_SUCCESS;
    char buf[PATH_MAX];
    ssize_t len = readlink(link_path, buf, sizeof(buf) - 1);
    if (len == WRONG_BEHAVIOUR) {
        perror("readlink");
        return EXIT_FAILURE;
    }
    buf[len] = '\0';
    return_value = print_file(buf);
    return return_value;
}

int delete_symlink(const char* link_path) {
    int return_value = EXIT_SUCCESS;
    return_value = delete_file(link_path);
    return return_value;
}

int create_hardlink(const char* path, const char* link_path) {
    int link_status = link(path, link_path);
    if (link_status == HARDLINK_ERROR) {
        perror("hardlink");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int delete_hardlink(const char* link_path) {
    int return_value = EXIT_SUCCESS;
    return_value = delete_file(link_path);
    return return_value;
}

int print_file_info(const char* path) {
    struct stat sb;
    int stat_status = stat(path, &sb);
    if (stat_status == STAT_ERROR) {
        perror("stat");
        return EXIT_FAILURE;
    }
    printf("Owner permissions: ");
    if (sb.st_mode & S_IRUSR) printf("r"); else printf("-");
    if (sb.st_mode & S_IWUSR) printf("w"); else printf("-");
    if (sb.st_mode & S_IXUSR) printf("x"); else printf("-");

    printf("\nGroup permissions: ");
    if (sb.st_mode & S_IRGRP) printf("r"); else printf("-");
    if (sb.st_mode & S_IWGRP) printf("w"); else printf("-");
    if (sb.st_mode & S_IXGRP) printf("x"); else printf("-");

    printf("\nOther permissions: ");
    if (sb.st_mode & S_IROTH) printf("r"); else printf("-");
    if (sb.st_mode & S_IWOTH) printf("w"); else printf("-");
    if (sb.st_mode & S_IXOTH) printf("x"); else printf("-");
    printf("\nLinks: %li\n", sb.st_nlink);
    return EXIT_SUCCESS;
}

int change_file_permissions(const char* path, mode_t mode) {
    int chmod_status = chmod(path, mode);
    if (chmod_status == CHMODE_ERROR) {
        perror("chmod");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {

    char function[128];
    char path[PATH_MAX];
    char arguments[PATH_MAX];
    int parse_status = parse_file(function, path, arguments, argv[0]);
    if (parse_status == WRONG_BEHAVIOUR) {
        return EXIT_FAILURE;
    }
    if (strcmp(function, "create_directory") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = create_directory(path);
        return return_value;
    }
    if (strcmp(function, "print_directory") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = print_directory(path);
        return return_value;
    }
    if (strcmp(function, "delete_directory") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = delete_directory(path);
        return return_value;
    }
    if (strcmp(function, "create_file") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = create_file(path);
        return return_value;
    }
    if (strcmp(function, "print_file") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = print_file(path);
        return return_value;
    }
    if (strcmp(function, "delete_file") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = delete_file(path);
        return return_value;
    }
    if (strcmp(function, "create_symlink") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = create_symlink(path, arguments);
        return return_value;
    }
    if (strcmp(function, "print_symlink") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = print_symlink(path);
        return return_value;
    }
    if (strcmp(function, "print_symlink_content") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = print_symlink_content(path);
        return return_value;
    }
    if (strcmp(function, "delete_symlink") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = delete_symlink(path);
        return return_value;
    }
    if (strcmp(function, "create_hardlink") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = create_hardlink(path, arguments);
        return return_value;
    }
    if (strcmp(function, "delete_hardlink") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = delete_hardlink(path);
        return return_value;
    }
    if (strcmp(function, "print_file_info") == STR_DOES_MATCH) {
        int return_value = EXIT_SUCCESS;
        return_value = print_file_info(path);
        return return_value;
    }
    if (strcmp(function, "change_file_permissions") == STR_DOES_MATCH) {
        mode_t mode = (mode_t)strtol(arguments, NULL, 8);
        if (mode == STRTOL_ERROR) {
            perror("strtol");
            return EXIT_FAILURE;
        }
        int return_value = EXIT_SUCCESS;
        return_value = change_file_permissions(path, mode);
        return return_value;
    }
    fprintf(stderr, "Unknown function: %s\n", function);
    return EXIT_FAILURE;
}
