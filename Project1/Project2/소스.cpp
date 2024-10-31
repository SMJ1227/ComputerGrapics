#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define MAX 1000 // 최대 문자열 크기
#define MAX_LINES 10 // 최대 줄 수

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
    printf("\n대문자로 시작하는 단어의 수: %d\n", count);
    free(copy_str);
}

void reverse_string(char* str)   {
    char* copy_str = _strdup(str);
    char* first_line = strtok(copy_str, "\n"); // 첫 번째 줄을 추출

    if (first_line == NULL) {
        printf("문자열에 줄이 없습니다.\n");
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
    int words = 0; // 단어 수를 세는 변수
    char* copy_str = _strdup(str);
    char* token = copy_str;

    while (*token != '\0') {
        if (*token == ' ' || *token == '\t') {
            // 공백 또는 탭을 만나면 단어 끝을 의미
            if (words > 0) {
                words++;
            }
        }
        else if (*token == '\n') {
            // 줄바꿈 문자를 만나면 현재 줄의 단어 수 저장
            line[line_index] = words;
            line_index++;
            words = 0; // 다음 줄의 단어 수를 새로 계산
        }
        else {
            // 단어를 구성하는 문자일 경우
            if (words == 0) {
                words = 1; // 단어의 시작을 나타냄
            }
        }
        token++;
    }

    // 마지막 줄의 단어 수 저장
    if (line_index < MAX_LINES) {
        line[line_index] = words;
    }

    // 결과 출력
    for (int i = 0; i <= line_index; i++) {
        printf("줄 %d의 단어 수: %d\n", i + 1, line[i]);
    }

    free(copy_str); // 복사한 문자열 메모리 해제
}

void print_nth_line(char* str, int n) {
    char* copy_str = _strdup(str);  // 원본 문자열 복사
    char* token = strtok(copy_str, "\n"); // \n 기준으로 줄을 나눔
    int line_number = 1;

    while (token != NULL) {
        if (line_number == n) {
            printf("줄 %d: %s\n", n, token);
            free(copy_str);
            return;
        }
        token = strtok(NULL, "\n");
        line_number++;
    }

    // n번째 줄이 존재하지 않을 경우
    if (line_number < n) {
        printf("문자열에 줄 %d가 존재하지 않습니다.\n", n);
    }

    free(copy_str); // 복사한 문자열 메모리 해제
}

void sort_by_word_count(int* line, int* line_order, int num_lines, int goupdown) {
    int used[MAX_LINES] = { 0 }; // 0:x, 1:o)
    if (goupdown == 1) {
        for (int i = 0; i < num_lines; i++) {
            int min_index = 100;
            int min_value = 100;

            // 현재 사용되지 않은 줄 중에서 가장 작은 단어 수를 가진 인덱스를 찾음
            for (int j = 0; j < num_lines; j++) {
                if (!used[j] && line[j] < min_value) {
                    min_value = line[j];
                    min_index = j;
                }
            }

            if (min_index != 100) {
                line_order[i] = min_index;  // 가장 작은 값을 가진 인덱스를 line_order에 저장
                used[min_index] = 1;        // 해당 인덱스를 사용했다고 표시
            }
        }
    }
    else if (goupdown == 2) {
        for (int i = 0; i < num_lines; i++) {
            int max_index = -1;
            int max_value = -1;

            // 현재 사용되지 않은 줄 중에서 가장 큰 단어 수를 가진 인덱스를 찾음
            for (int j = 0; j < num_lines; j++) {
                if (!used[j] && line[j] > max_value) {
                    max_value = line[j];
                    max_index = j;
                }
            }

            if (max_index != -1) {
                line_order[i] = max_index;  // 가장 큰 값을 가진 인덱스를 line_order에 저장
                used[max_index] = 1;        // 해당 인덱스를 사용했다고 표시
            }
        }
    }
}

void replace_lines(char* str, int goupdown) {
    count_words_in_lines(str);
    int line_order[MAX_LINES];
    int num_lines = 0;

    for (int i = 0; i < MAX_LINES; i++) {
        line_order[i] = i;  // 각 줄의 번호 저장
    }

    for (int i = 0; i < MAX_LINES; i++) {
        if (line[i] > 0) {
            line_order[num_lines] = i;  // 각 줄의 번호 저장
            num_lines++;
        }
    }

    if (goupdown == 0) {
        printf("%s\n", str);
    }
    else if (goupdown == 1) {   // 오름차순
        sort_by_word_count(line, line_order, num_lines, goupdown);
        for (int i = 0; i < num_lines; i++) {
            print_nth_line(str, line_order[i] + 1);  // 줄 번호는 1부터 시작하므로 +1
        }
    }
    else if (goupdown == 2) {   // 내림차순
        sort_by_word_count(line, line_order, num_lines, goupdown);
        for (int i = 0; i < num_lines; i++) {
            print_nth_line(str, line_order[i] + 1);  // 줄 번호는 1부터 시작하므로 +1
        }
    }
}

void highlight_word(const char* original_str, const char* word) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    char* str = _strdup(original_str);  // 원본 문자열 복사
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
    printf("\n'%s' 단어의 수: %d\n", word, count);

    free(str); // 복사한 문자열 메모리 해제
}

int main() {
    char filename[100], buffer[MAX + 1];
    FILE* file;
    size_t bytesRead;

    printf("파일 이름을 입력하세요: ");
    scanf("%s", filename);

    file = fopen(filename, "r");
    if (!file) {
        printf("파일을 열 수 없습니다.\n");
        return 1;
    }

    bytesRead = fread(buffer, sizeof(char), MAX, file);
    buffer[bytesRead] = '\0';  // 파일 끝에 null 문자를 추가하여 문자열로 만듦
    fclose(file);

    remove_extra_spaces(buffer);
    printf("%s\n", buffer);

    char input;
    int goupdown = 1;   // 0 == main, 1 == up, 2 == down

    while (1) {
        printf("\n명령어를 입력하세요 (c, d, e, f, g, h, r, s, q): ");
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
            printf("바꿀 문자와 새로 입력할 문자를 입력하세요: ");
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
            printf("찾을 단어를 입력하세요: ");
            scanf("%s", word);
            highlight_word(buffer, word);
            break;
        }
        case 'q':
            printf("프로그램을 종료합니다.\n");
            return 0;
        default:
            printf("잘못된 명령어입니다.\n");
        }
    }

    // 프로그램 종료
    return 0;
}
