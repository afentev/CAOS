#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    char first_buff[1024];
    fgets(first_buff, 1024, stdin);
    first_buff[strcspn(first_buff, "\n")] = 0;
    char* script_path = strchr(first_buff, ' ') + 1;
    char* end = strchr(script_path, ' ');
    *end = '\0';

    char* query_string = strchr(script_path, '?');
    size_t params_count = 0;
    if (query_string != NULL) {
        *query_string = '\0';
        ++query_string;
        params_count = 1;
    } else {
        query_string = end;
    }

    char* script_name = script_path;
    char* folder = strchr(script_name, '/');
    while (folder != NULL && folder < query_string) {
        script_name = folder + 1;
        folder = strchr(script_name, '/');
    }

    char host_buff[1024];
    fgets(host_buff, 1024, stdin);
    host_buff[strcspn(host_buff, "\n")] = 0;
    char* host_name = strchr(host_buff, ' ') + 1;

    char path_buff[query_string - script_path + 2];
    path_buff[0] = '.';
    strcpy(path_buff + 1, script_path);
    path_buff[query_string - script_path + 1] = '\0';
    struct stat file_stat;
    int res = stat(path_buff, &file_stat);
    if (res < 0) {
        printf("HTTP/1.1 404 ERROR\n");
    } else if (!(file_stat.st_mode & S_IXUSR)) {
        printf("HTTP/1.1 403 ERROR\n");
    } else {
        printf("HTTP/1.1 200 OK\n");
        fflush(stdout);
        pid_t pid = fork();
        if (pid == 0) {
            setenv("REQUEST_METHOD", "GET", 1);
            setenv("QUERY_STRING", query_string, 1);
            setenv("SCRIPT_NAME", script_name, 1);
            setenv("HTTP_HOST", host_name, 1);

            char* extension = strchr(script_name, '.');
            if (extension == NULL) {
                extension = query_string;
            }
            char* script = malloc(extension - script_name + 1);
            memcpy(script, script_name, extension - script_name);
            script[extension - script_name] = '\0';

            char* string_pos = query_string;
            while ((string_pos = strchr(string_pos, '&')) != NULL) {
                ++params_count;
                ++string_pos;
            }
            char** params = malloc((params_count + 2) * sizeof(char*));
            params[0] = script;
            for (size_t i = 1; i <= params_count; ++i) {
                params[i] = query_string;
                char* separator = strchr(query_string, '&');
                if (separator != NULL) {
                    *separator = '\0';
                    query_string = separator + 1;
                }
            }
            params[params_count + 1] = NULL;

            execv(path_buff, params);
            return 1;
        }
        int status;
        waitpid(pid, &status, 0);
    }
}