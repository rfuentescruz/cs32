#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#define NAME_LENGTH 3

#define STATUS_SECTION_EMPTY 1
#define STATUS_SECTION_CASE 2
#define STATUS_SECTION_COURSE 3
#define STATUS_SECTION_STUDENT 4

#define STATUS_COURSE_DEF 30
#define STATUS_COURSE_NAME 31

#define STATUS_STUDENT_DEF 40
#define STATUS_STUDENT_NAME 41

typedef struct Student {
    char name[NAME_LENGTH + 1];
    struct Student *next;
} Student;

typedef struct StudentList {
    Student *head;
} StudentList;

typedef struct Course {
    char name[NAME_LENGTH + 1];
    StudentList *students;
    struct Course *next;
} Course;

typedef struct CourseList {
    Course *head;
    Course *tail;
} CourseList;

void parse_courses(CourseList *);
void free_courses(CourseList *);
void push_course(CourseList *, char[NAME_LENGTH]);

void push_student(StudentList *, char[NAME_LENGTH]);
void free_students(StudentList *);

int main(int argc, char *argv[]) {
    CourseList *courses = (CourseList *) malloc(sizeof(CourseList));

    courses->head = NULL;
    courses->tail = NULL;

    parse_courses(courses);

    Course *c = courses->head;
    Student *s = NULL;

    while (c != NULL) {
        printf("%s:\n", c->name);
        if (c->students != NULL) {
            s = c->students->head;
            while (s != NULL) {
                printf("  %s\n", s->name);
                s = s->next;
            }
        }
        c = c->next;
    }

    free_courses(courses);
}

void parse_courses(CourseList *courses) {
    int status = STATUS_SECTION_COURSE,
        name_index = 1;

    char c, name[NAME_LENGTH];

    memset(name, '\0', NAME_LENGTH);

    Course *course_index = NULL;

    while ((c = fgetc(stdin)) != EOF) {
        if (status == STATUS_SECTION_COURSE && c == '(') {
            status = STATUS_COURSE_DEF;
        } else if (status == STATUS_COURSE_DEF && c == 'C') {
            status = STATUS_COURSE_NAME;
            name[0] = 'C';
            name_index = 1;
        } else if (status == STATUS_COURSE_NAME) {
            if (isdigit(c) && name_index < NAME_LENGTH) {
                name[name_index] = c;
                name_index++;
            } else {
                push_course(courses, name);
            }

            if (c == ',') {
                status = STATUS_COURSE_DEF;
            } else if (c == ')') {
                status = STATUS_SECTION_STUDENT;
            }
        } else if (status == STATUS_SECTION_STUDENT && c == '(') {
            status = STATUS_STUDENT_DEF;

            if (course_index == NULL) {
                course_index = courses->head;
            } else {
                course_index = course_index->next;
            }
        } else if (status == STATUS_STUDENT_DEF && isalpha(c)) {
            status = STATUS_STUDENT_NAME;
            name[0] = c;
            name_index = 1;
        } else if (status == STATUS_STUDENT_NAME) {
            if (isalpha(c) && name_index < NAME_LENGTH) {
                name[name_index] = c;
                name_index++;
            } else {
                push_student(course_index->students, name);
            }

            if (c == ',') {
                status = STATUS_STUDENT_DEF;
            } else if (c == ')') {
                status = STATUS_SECTION_STUDENT;
            }
        }
    }
}

void push_course(CourseList *list, char name[NAME_LENGTH]) {
    Course *c = malloc(sizeof(Course));
    c->next = NULL;

    memset(c->name, '\0', NAME_LENGTH + 1);
    strncpy(c->name, name, NAME_LENGTH);

    c->students = malloc(sizeof(StudentList));
    c->students->head = NULL;

    if (list->head == NULL) {
        list->head = c;
    }

    if (list->tail == NULL) {
        list->tail = c;
    } else {
        list->tail->next = c;
        list->tail = c;
    }
}

void push_student(StudentList *list, char name[NAME_LENGTH]) {
    Student *s = malloc(sizeof(Student));
    memset(s->name, '\0', NAME_LENGTH + 1);
    strncpy(s->name, name, NAME_LENGTH);

    s->next = list->head;
    list->head = s;
}

void free_courses(CourseList *list) {
    if (list == NULL) {
        return;
    }

    Course *temp = NULL;

    while (list->head != NULL) {
        temp = list->head;
        list->head = temp->next;
        if (temp->students != NULL) {
            free_students(temp->students);
        }
        free(temp);
    }
    free(list);
}

void free_students(StudentList *list) {
    if (list == NULL) {
        return;
    }
    Student *temp = NULL;
    while (list->head != NULL) {
        temp = list->head;
        list->head = temp->next;
        free(temp);
    }
    free(list);
}
