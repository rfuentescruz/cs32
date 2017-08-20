#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
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

/** ----------------- DECLARATIONS ----------------- **/
typedef struct Student Student;
typedef struct StudentList StudentList;

typedef struct Course Course;
typedef struct CourseList CourseList;

typedef struct CourseConflict CourseConflict;
typedef struct CourseConflictList CourseConflictList;

void parse_courses(CourseList *);
void check_course_conflicts(CourseList *);

void free_courses(CourseList *);
void push_course(CourseList *, char[NAME_LENGTH]);

void push_student(StudentList *, char[NAME_LENGTH]);
void free_students(StudentList *);

void push_course_conflict(CourseConflictList *, Course *);
void free_course_conflicts(CourseConflictList *);

/** ----------------- DEFINITIONS ----------------- **/
struct Course {
    char name[NAME_LENGTH + 1]; // Including the terminating NULL byte
    int schedule;

    StudentList *students;
    CourseConflictList *conflicts;
    Course *next;
};

struct CourseConflict {
    Course *course;
    CourseConflict *next;
};

struct Student {
    char name[NAME_LENGTH + 1]; // Including the terminating NULL byte
    Student *next;
};

struct CourseList {
    Course *head;
    Course *tail;
    int len;
};

struct CourseConflictList {
    CourseConflict *head;
};

struct StudentList {
    Student *head;
};

int main(int argc, char *argv[]) {
    CourseList *courses = (CourseList *) malloc(sizeof(CourseList));
    courses->len = 0;
    courses->head = NULL;
    courses->tail = NULL;

    parse_courses(courses);
    check_course_conflicts(courses);

    Course *c = courses->head;
    CourseConflict *cc = NULL;
    Student *s = NULL;

    while (c != NULL) {
        printf("%s:", c->name);
        if (c->conflicts != NULL) {
            printf(" Conflicts - ");
            cc = c->conflicts->head;
            while (cc != NULL) {
                printf("%s ", cc->course->name);
                cc = cc->next;
            }
        }
        printf(" \n");

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

void check_course_conflicts(CourseList *courses) {
    Course *current = courses->head;
    Course *check = NULL;
    bool conflict = false;

    while (current != NULL) {
        check = current->next;

        while (check != NULL) {
            Student *s1 = current->students->head;
            Student *s2 = NULL;
            conflict = false;

            while (!conflict && s1 != NULL) {
                conflict = false;
                s2 = check->students->head;
                while (!conflict && s2 != NULL) {
                    // printf("(%s) %s == (%s) %s\n", current->name, s1->name, check->name, s2->name);
                    if (strcasecmp(s1->name, s2->name) == 0) {
                        conflict = true;
                    }
                    s2 = s2->next;
                }
                s1 = s1->next;
            }

            if (conflict) {
                push_course_conflict(current->conflicts, check);
                push_course_conflict(check->conflicts, current);
            }

            check = check->next;
        }

        current = current->next;
    }
}

void push_course(CourseList *list, char name[NAME_LENGTH]) {
    Course *c = malloc(sizeof(Course));
    c->schedule = 0;
    c->next = NULL;

    memset(c->name, '\0', NAME_LENGTH + 1);
    strncpy(c->name, name, NAME_LENGTH);

    c->students = malloc(sizeof(StudentList));
    c->students->head = NULL;

    c->conflicts = malloc(sizeof(CourseConflictList));
    c->conflicts->head = NULL;

    if (list->head == NULL) {
        list->head = c;
    }

    if (list->tail == NULL) {
        list->tail = c;
    } else {
        list->tail->next = c;
        list->tail = c;
    }
    list->len++;
}

void push_student(StudentList *list, char name[NAME_LENGTH]) {
    Student *s = malloc(sizeof(Student));
    memset(s->name, '\0', NAME_LENGTH + 1);
    strncpy(s->name, name, NAME_LENGTH);

    s->next = list->head;
    list->head = s;
}

void push_course_conflict(CourseConflictList *list, Course *course) {
    CourseConflict *s = malloc(sizeof(CourseConflict));
    s->course = course;
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
        free_students(temp->students);
        free_course_conflicts(temp->conflicts);
        free(temp);
    }
    free(list);
}

void free_course_conflicts(CourseConflictList *list) {
    if (list == NULL) {
        return;
    }

    CourseConflict *temp = NULL;
    while (list->head != NULL) {
        temp = list->head;
        list->head = temp->next;
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
