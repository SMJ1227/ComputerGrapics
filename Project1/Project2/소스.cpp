#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define MAX 1000 // �ִ� ���ڿ� ũ��
#define MAX_LINES 10 // �ִ� �� ��

int line[MAX_LINES] = { 0 };

void remove_extra_spaces(char* str) {
    int i, x;
    for (i = x = 0; str[i]; ++i)
        if (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1])))
            str[x++] = str[i];
    str[x] = '\0';
}

void highlight_uppercase_words(char* str) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    char* copy_str = _strdup(str);
    char* token = strtok(copy_str, " ");
    int count = 0;
    WORD defaultAttributes;

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    defaultAttributes = consoleInfo.wAttributes;

    while (token != NULL) {
        if (isupper(token[0])) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("%s ", token);
            count++;
        }
        else {
            SetConsoleTextAttribute(hConsole, defaultAttributes);
            printf("%s ", token);
        }
        token = strtok(NULL, " ");
    }
    SetConsoleTextAttribute(hConsole, defaultAttributes);
    printf("\n�빮�ڷ� �����ϴ� �ܾ��� ��: %d\n", count);
    free(copy_str);
}

void reverse_string(char* str)   {
    char* copy_str = _strdup(str);
    char* first_line = strtok(copy_str, "\n"); // ù ��° ���� ����

    if (first_line == NULL) {
        printf("���ڿ��� ���� �����ϴ�.\n");
        return;
    }

    int len = strlen(first_line);
    for (int i = len - 1; i >= 0; i--) {
        printf("%c", first_line[i]);
    }
    printf("\n");
    free(copy_str);
}

void insert_at_every_third_char(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        printf("%c", str[i]);
        if ((i + 1) % 3 == 0) {
            printf("@@");
        }
    }
    printf("\n");
}

void reverse_words(char* str) {
    char* copy_str = _strdup(str);
    char* token = strtok(copy_str, " ");
    while (token != NULL) {
        for (int i = strlen(token) - 1; i >= 0; i--) {
            printf("%c", token[i]);
        }
        printf(" ");
        token = strtok(NULL, " ");
    }
    printf("\n");
    free(copy_str);
}

void replace_char(char* str, char old_char, char new_char) {
    char* copy_str = _strdup(str);
    for (int i = 0; str[i]; i++) {
        if (copy_str[i] == old_char) {
            copy_str[i] = new_char;
        }
    }
    printf("%s\n", copy_str);
    free(copy_str);
}

void count_words_in_lines(char* str) {
    int line_index = 0;
    int words = 0; // �ܾ� ���� ���� ����
    char* copy_str = _strdup(str);
    char* token = copy_str;

    while (*token != '\0') {
        if (*token == ' ' || *token == '\t') {
            // ���� �Ǵ� ���� ������ �ܾ� ���� �ǹ�
            if (words > 0) {
                words++;
            }
        }
        else if (*token == '\n') {
            // �ٹٲ� ���ڸ� ������ ���� ���� �ܾ� �� ����
            line[line_index] = words;
            line_index++;
            words = 0; // ���� ���� �ܾ� ���� ���� ���
        }
        else {
            // �ܾ �����ϴ� ������ ���
            if (words == 0) {
                words = 1; // �ܾ��� ������ ��Ÿ��
            }
        }
        token++;
    }

    // ������ ���� �ܾ� �� ����
    if (line_index < MAX_LINES) {
        line[line_index] = words;
    }

    // ��� ���
    for (int i = 0; i <= line_index; i++) {
        printf("�� %d�� �ܾ� ��: %d\n", i + 1, line[i]);
    }

    free(copy_str); // ������ ���ڿ� �޸� ����
}

void print_nth_line(char* str, int n) {
    char* copy_str = _strdup(str);  // ���� ���ڿ� ����
    char* token = strtok(copy_str, "\n"); // \n �������� ���� ����
    int line_number = 1;

    while (token != NULL) {
        if (line_number == n) {
            printf("�� %d: %s\n", n, token);
            free(copy_str);
            return;
        }
        token = strtok(NULL, "\n");
        line_number++;
    }

    // n��° ���� �������� ���� ���
    if (line_number < n) {
        printf("���ڿ��� �� %d�� �������� �ʽ��ϴ�.\n", n);
    }

    free(copy_str); // ������ ���ڿ� �޸� ����
}

void sort_by_word_count(int* line, int* line_order, int num_lines, int goupdown) {
    int used[MAX_LINES] = { 0 }; // 0:x, 1:o)
    if (goupdown == 1) {
        for (int i = 0; i < num_lines; i++) {
            int min_index = 100;
            int min_value = 100;

            // ���� ������ ���� �� �߿��� ���� ���� �ܾ� ���� ���� �ε����� ã��
            for (int j = 0; j < num_lines; j++) {
                if (!used[j] && line[j] < min_value) {
                    min_value = line[j];
                    min_index = j;
                }
            }

            if (min_index != 100) {
                line_order[i] = min_index;  // ���� ���� ���� ���� �ε����� line_order�� ����
                used[min_index] = 1;        // �ش� �ε����� ����ߴٰ� ǥ��
            }
        }
    }
    else if (goupdown == 2) {
        for (int i = 0; i < num_lines; i++) {
            int max_index = -1;
            int max_value = -1;

            // ���� ������ ���� �� �߿��� ���� ū �ܾ� ���� ���� �ε����� ã��
            for (int j = 0; j < num_lines; j++) {
                if (!used[j] && line[j] > max_value) {
                    max_value = line[j];
                    max_index = j;
                }
            }

            if (max_index != -1) {
                line_order[i] = max_index;  // ���� ū ���� ���� �ε����� line_order�� ����
                used[max_index] = 1;        // �ش� �ε����� ����ߴٰ� ǥ��
            }
        }
    }
}

void replace_lines(char* str, int goupdown) {
    count_words_in_lines(str);
    int line_order[MAX_LINES];
    int num_lines = 0;

    for (int i = 0; i < MAX_LINES; i++) {
        line_order[i] = i;  // �� ���� ��ȣ ����
    }

    for (int i = 0; i < MAX_LINES; i++) {
        if (line[i] > 0) {
            line_order[num_lines] = i;  // �� ���� ��ȣ ����
            num_lines++;
        }
    }

    if (goupdown == 0) {
        printf("%s\n", str);
    }
    else if (goupdown == 1) {   // ��������
        sort_by_word_count(line, line_order, num_lines, goupdown);
        for (int i = 0; i < num_lines; i++) {
            print_nth_line(str, line_order[i] + 1);  // �� ��ȣ�� 1���� �����ϹǷ� +1
        }
    }
    else if (goupdown == 2) {   // ��������
        sort_by_word_count(line, line_order, num_lines, goupdown);
        for (int i = 0; i < num_lines; i++) {
            print_nth_line(str, line_order[i] + 1);  // �� ��ȣ�� 1���� �����ϹǷ� +1
        }
    }
}

void highlight_word(const char* original_str, const char* word) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    char* str = _strdup(original_str);  // ���� ���ڿ� ����
    char* token = strtok(str, " ");
    int count = 0;
    WORD defaultAttributes;

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    defaultAttributes = consoleInfo.wAttributes;

    while (token != NULL) {
        if (_stricmp(token, word) == 0) {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("%s ", token);
            count++;
        }
        else {
            SetConsoleTextAttribute(hConsole, defaultAttributes);
            printf("%s ", token);
        }
        token = strtok(NULL, " ");
    }
    SetConsoleTextAttribute(hConsole, defaultAttributes);
    printf("\n'%s' �ܾ��� ��: %d\n", word, count);

    free(str); // ������ ���ڿ� �޸� ����
}

int main() {
    char filename[100], buffer[MAX + 1];
    FILE* file;
    size_t bytesRead;

    printf("���� �̸��� �Է��ϼ���: ");
    scanf("%s", filename);

    file = fopen(filename, "r");
    if (!file) {
        printf("������ �� �� �����ϴ�.\n");
        return 1;
    }

    bytesRead = fread(buffer, sizeof(char), MAX, file);
    buffer[bytesRead] = '\0';  // ���� ���� null ���ڸ� �߰��Ͽ� ���ڿ��� ����
    fclose(file);

    remove_extra_spaces(buffer);
    printf("%s\n", buffer);

    char input;
    int goupdown = 1;   // 0 == main, 1 == up, 2 == down

    while (1) {
        printf("\n��ɾ �Է��ϼ��� (c, d, e, f, g, h, r, s, q): ");
        scanf(" %c", &input);

        switch (input) {
        case 'c':
            highlight_uppercase_words(buffer);
            break;
        case 'd':
            reverse_string(buffer);
            break;
        case 'e':
            insert_at_every_third_char(buffer);
            break;
        case 'f':
            reverse_words(buffer);
            break;
        case 'g': {
            char old_char, new_char;
            printf("�ٲ� ���ڿ� ���� �Է��� ���ڸ� �Է��ϼ���: ");
            scanf(" %c %c", &old_char, &new_char);
            replace_char(buffer, old_char, new_char);
            break;
        }
        case 'h':
            count_words_in_lines(buffer);
            break;
        case 'r':
            replace_lines(buffer, goupdown);
            goupdown = (goupdown + 1) % 3;
            break;
        case 's': {
            char word[100];
            printf("ã�� �ܾ �Է��ϼ���: ");
            scanf("%s", word);
            highlight_word(buffer, word);
            break;
        }
        case 'q':
            printf("���α׷��� �����մϴ�.\n");
            return 0;
        default:
            printf("�߸��� ��ɾ��Դϴ�.\n");
        }
    }

    // ���α׷� ����
    return 0;
}
