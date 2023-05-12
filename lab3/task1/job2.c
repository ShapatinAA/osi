#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/io.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define WRONG_BEHAVIOUR 1
#define OPEN_ERROR -1
#define SEEK_ERROR -1
#define DIR_CREATE_ERROR -1
#define DIRSYM '/'
#define MAX_QUEUE_SIZE PATH_MAX
#define RIGHT_BEHAVIOUR 0

typedef struct StackNode {
    char path[PATH_MAX];
    struct StackNode* prev;
} StackNode;

int push(StackNode** stack_top, const char* path) {
    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));
    if (new_node == NULL) {
        return EXIT_FAILURE;
    }

    strcpy(new_node->path, path);
    new_node->prev = *stack_top;
    *stack_top = new_node;
    return EXIT_SUCCESS;
}

const char* pop(StackNode** stack_top, char* buffer) {

    StackNode* top_node = *stack_top;

    strncpy(buffer, top_node->path, PATH_MAX);
    *stack_top = top_node->prev;
    free(top_node);
    return buffer;
}

void clear_stack(StackNode* stack_top) {
    while (stack_top != NULL) {
        StackNode* temp = stack_top;
        stack_top = stack_top->prev;
        free(temp);
    }
}

void join_paths(char* result, const char* path1, const char* path2);
void reverse_last_dir_name(char* str);
void reverse_buffer(char* buffer, size_t bytes_read);
int read_analyse(ssize_t bytes_read, size_t bytes_to_read);
int write_analyse(ssize_t bytes_wrote, size_t bytes_to_write);
int copy_reverse_content(off_t pos, int src_file, int dest_file, char* buffer);
int reverse_file(const char* src, const char* dest);
void build_reversed_dest_path(char* path, const char* base_path);
int cyclic_reverse(StackNode** queue, char* buffer, const char* dest);
int dir_walkthroug_and_analyse(const char* current_src_dir, const char* current_dest_dir, struct stat dir_info, DIR* dir, struct StackNode** queue);
int reverse_directory(const char* src, const char* dest);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: <source directory>\n");
        return EXIT_FAILURE;
    }

    char* src = argv[1];
    char dest[PATH_MAX];
    strcpy(dest, src);
    reverse_last_dir_name(dest);

    int return_value = reverse_directory(src, dest);

    return return_value;
}

void join_paths(char* result, const char* path1, const char* path2) {
    if (strlen(path2) == 0) {
        snprintf(result, PATH_MAX, "%s", path1);
    }
    else {
        snprintf(result, PATH_MAX, "%s%c%s", path1, DIRSYM, path2);
    }
}

void reverse_last_dir_name(char* str) {
    char* last_part = strrchr(str, DIRSYM);
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

void reverse_buffer(char* buffer, size_t bytes_read) {
    for (unsigned int i = 0; i < bytes_read / 2; i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[bytes_read - 1 - i];
        buffer[bytes_read - 1 - i] = tmp;
    }
}

int read_analyse(ssize_t bytes_read, size_t bytes_to_read) {
    if (bytes_read == -1) {
        perror("Error reading source file\n");
        return EXIT_FAILURE;
    }
    if (bytes_read != bytes_to_read) {
        fprintf(stderr, "Error in reading source file : bytes read isn't equal to bytes to read\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int write_analyse(ssize_t bytes_wrote, size_t bytes_to_write) {
    if (bytes_wrote == -1) {
        perror("Error writing in destination file\n");
        return EXIT_FAILURE;
    }
    if (bytes_wrote != bytes_to_write) {
        fprintf(stderr, "Error writing in destination file : bytes wrote isn't equal to bytes to write\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int copy_reverse_content(off_t pos, int src_file, int dest_file, char* buffer) {
    while (pos > 0) {
        off_t read_start = pos - BUFFER_SIZE > 0 ? pos - BUFFER_SIZE : 0;
        size_t bytes_to_read = pos - read_start;
        size_t bytes_to_write = bytes_to_read;

        size_t lseek_return = lseek(src_file, read_start, SEEK_SET);
        if (lseek_return == SEEK_ERROR) {
            perror("Error seeking in file\n");
            return EXIT_FAILURE;
        }

        ssize_t bytes_read = read(src_file, buffer, bytes_to_read);

        int read_analyse_status = read_analyse(bytes_read, bytes_to_read);
        if (read_analyse_status == WRONG_BEHAVIOUR) {
            return EXIT_FAILURE;
            break;
        }

        reverse_buffer(buffer, bytes_read);

        ssize_t bytes_wrote = write(dest_file, buffer, bytes_to_write);

        int write_analyse_status = write_analyse(bytes_wrote, bytes_to_write);
        if (write_analyse_status == WRONG_BEHAVIOUR) {
            return EXIT_FAILURE;
        }

        pos = read_start;
    }
    return EXIT_SUCCESS;
}

void close_files(int file1, int file2) {
    if (file1 != 0) {
        close(file1);
    }
    if (file2 != 0) {
        close(file2);
    }
}

int reverse_file(const char* src, const char* dest) {

    int src_file = open(src, O_RDONLY);
    if (src_file == OPEN_ERROR) {
        perror("Error opening source file\n");
        return EXIT_FAILURE;
    }

    struct stat st;

    int fstat_status = fstat(src_file, &st);
    if (fstat_status == OPEN_ERROR) {
        perror("Error getting source file permissions\n");
        close_files(src_file, 0);
        return EXIT_FAILURE;
    }

    int dest_file = open(dest, O_WRONLY | O_CREAT, st.st_mode);
    if (dest_file == OPEN_ERROR) {
        perror("Error opening destination file\n");
        close_files(src_file, 0);
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];

    off_t pos = lseek(src_file, 0, SEEK_END);
    if (pos == SEEK_ERROR) {
        perror("Error seeking in file\n");
        close_files(src_file, dest_file);
        return EXIT_FAILURE;
    }

    int copy_reverse_content_return = copy_reverse_content(pos, src_file, dest_file, buffer);

    close_files(src_file, dest_file);
    return copy_reverse_content_return;
}

void build_reversed_dest_path(char* path, const char* base_path) {
    int base_path_len = strlen(base_path);
    char* current = path + base_path_len;

    bool not_end_or_null = (strchr(current + 1, DIRSYM) != 0) && (*current != '\0');
    while (not_end_or_null) {
        current++;
        size_t length_of_dir_to_reverse = strlen(current) - strlen(strchr(current, DIRSYM));

        reverse_buffer(current, length_of_dir_to_reverse);

        current += length_of_dir_to_reverse;
        not_end_or_null = (strchr(current + 1, DIRSYM) != 0) && (*current != '\0');
    }
    reverse_last_dir_name(current);
}

int dir_walkthroug_and_analyse(const char* current_src_dir, const char* current_dest_dir, struct stat dir_info, DIR* dir, struct StackNode** queue) {
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_entry_path[PATH_MAX], dest_entry_path[PATH_MAX];
        join_paths(src_entry_path, current_src_dir, entry->d_name);

        stat(src_entry_path, &dir_info);
        int push_result = RIGHT_BEHAVIOUR, reverse_file_result = RIGHT_BEHAVIOUR;
        if (S_ISDIR(dir_info.st_mode)) {
            push_result = push(queue, src_entry_path);
        }
        if (S_ISREG(dir_info.st_mode)) {
            join_paths(dest_entry_path, current_dest_dir, entry->d_name);
            reverse_last_dir_name(dest_entry_path);
            reverse_file_result = reverse_file(src_entry_path, dest_entry_path);
        }
        if (push_result == WRONG_BEHAVIOUR || reverse_file_result == WRONG_BEHAVIOUR) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int cyclic_reverse(StackNode** queue, char* buffer, const char* dest) {
    while (*queue != NULL) {
        char current_src_dir[PATH_MAX];
        strncpy(current_src_dir, pop(queue, buffer), PATH_MAX);
        char current_dest_dir[PATH_MAX];
        strncpy(current_dest_dir, current_src_dir, PATH_MAX);

        void* memcpy_result = memcpy(current_dest_dir, dest, strlen(dest));
        if (memcpy_result == NULL) {
            perror("Error copying memory\n");
            return EXIT_FAILURE;
        }

        build_reversed_dest_path(current_dest_dir, dest);

        struct stat dir_info;

        int stat_status = stat(current_src_dir, &dir_info);
        if (stat_status == OPEN_ERROR) {
            perror("Error reading file or directory\n");
            return EXIT_FAILURE;
        }

        int mkdir_result = mkdir(current_dest_dir, dir_info.st_mode);
        DIR* dir = opendir(current_src_dir);

        if (mkdir_result == DIR_CREATE_ERROR || dir == NULL) {
            perror("Error creating destination directory or opening directory\n");
            return EXIT_FAILURE;
        }

        int dir_walkthroug_and_analyse_result = dir_walkthroug_and_analyse(current_src_dir, current_dest_dir, dir_info, dir, queue);
        if (dir_walkthroug_and_analyse_result == WRONG_BEHAVIOUR) {
            closedir(dir);
            return EXIT_FAILURE;
        }

        closedir(dir);
    }
    return EXIT_SUCCESS;
}

int reverse_directory(const char* src, const char* dest) {
    StackNode* queue = NULL;
    int push_result = push(&queue, src);
    if (push_result == WRONG_BEHAVIOUR) {
        return EXIT_FAILURE;
    }
    char buffer[PATH_MAX];
    int return_value = EXIT_SUCCESS;
    return_value = cyclic_reverse(&queue, buffer, dest);
    //pop(&queue, buffer);
    clear_stack(queue);
    return return_value;
}
