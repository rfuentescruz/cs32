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
typedef struct Course Course;

typedef struct StudentNode StudentNode;
typedef struct StudentList StudentList;

typedef struct CourseNode CourseNode;
typedef struct CourseList CourseList;

Course *create_course(char[NAME_LENGTH]);
void parse_courses(CourseList *);
void check_course_conflicts(CourseList *);

void free_course_list(CourseList *, bool);
void push_course(CourseList *, Course *);

void push_student(StudentList *, char[NAME_LENGTH]);
void free_students(StudentList *);

/** ----------------- DEFINITIONS ----------------- **/
struct Course {
    char name[NAME_LENGTH + 1]; // Including the terminating NULL byte
    int schedule;

    StudentList *students;
    CourseList *conflicts;
};

struct CourseNode {
    Course *course;
    CourseNode *next;
};

struct StudentNode {
    char name[NAME_LENGTH + 1]; // Including the terminating NULL byte
    StudentNode *next;
};

struct CourseList {
    CourseNode *head;
    CourseNode *tail;
    int len;
};

struct StudentList {
    StudentNode *head;
};

int main(int argc, char *argv[]) {
    CourseList *courses = (CourseList *) malloc(sizeof(CourseList));
    courses->len = 0;
    courses->head = NULL;
    courses->tail = NULL;

    parse_courses(courses);
    check_course_conflicts(courses);

    CourseNode *node = courses->head;
    Course *course = NULL;
    StudentNode *s = NULL;

    while (node != NULL) {
        course = node->course;
        printf("%s:", course->name);
        if (course->conflicts != NULL) {
            printf(" Conflicts - ");
            CourseNode *conflict_node = course->conflicts->head;
            while (conflict_node != NULL) {
                printf("%s ", conflict_node->course->name);
                conflict_node = conflict_node->next;
            }
        }
        printf(" \n");

        if (course->students != NULL) {
            s = course->students->head;
            while (s != NULL) {
                printf("  %s\n", s->name);
                s = s->next;
            }
        }

        node = node->next;
    }

    free_course_list(courses, true);
}

void parse_courses(CourseList *courses) {
    int status = STATUS_SECTION_COURSE,
        name_index = 1;

    char c, name[NAME_LENGTH];

    memset(name, '\0', NAME_LENGTH);

    CourseNode *course_index = NULL;

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
                push_course(courses, create_course(name));
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
                push_student(course_index->course->students, name);
            }

            if (c == ',') {
                status = STATUS_STUDENT_DEF;
            } else if (c == ')') {
                status = STATUS_SECTION_STUDENT;
            }
        }
    }
}

Course *create_course(char name[NAME_LENGTH]) {
    Course *c = malloc(sizeof(Course));

    c->schedule = 0;
    memset(c->name, '\0', NAME_LENGTH + 1);
    strncpy(c->name, name, NAME_LENGTH);

    c->students = malloc(sizeof(StudentList));
    c->students->head = NULL;

    c->conflicts = malloc(sizeof(CourseList));
    c->conflicts->head = NULL;
    c->conflicts->tail = NULL;

    return c;
}

void check_course_conflicts(CourseList *courses) {
    if (courses->head == NULL) {
        return;
    }

    CourseNode *current = courses->head;
    CourseNode *next = NULL;
    bool conflict = false;

    while (current != NULL) {
        next = current->next;

        while (next != NULL) {
            Course *c = current->course;

            StudentNode *s1 = c->students->head;
            StudentNode *s2 = NULL;
            conflict = false;

            while (!conflict && s1 != NULL) {
                conflict = false;
                s2 = next->course->students->head;
                while (!conflict && s2 != NULL) {
                    // printf("(%s) %s == (%s) %s\n", current->name, s1->name, next->name, s2->name);
                    if (strcasecmp(s1->name, s2->name) == 0) {
                        conflict = true;
                    }
                    s2 = s2->next;
                }
                s1 = s1->next;
            }

            if (conflict) {
                push_course(c->conflicts, next->course);
                push_course(next->course->conflicts, c);
            }

            next = next->next;
        }

        current = current->next;
    }
}

void push_course(CourseList *list, Course *course) {
    CourseNode *c = malloc(sizeof(CourseNode));
    c->course = course;
    c->next = NULL;

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
    StudentNode *s = malloc(sizeof(StudentNode));
    memset(s->name, '\0', NAME_LENGTH + 1);
    strncpy(s->name, name, NAME_LENGTH);

    s->next = list->head;
    list->head = s;
}

void free_course(Course *c) {
    free_students(c->students);
    // Free the list nodes but do not free the course items themselves
    free_course_list(c->conflicts, false);
    free(c);
}

void free_course_list(CourseList *list, bool free_course_object) {
    if (list == NULL) {
        return;
    }

    CourseNode *temp = NULL;

    while (list->head != NULL) {
        temp = list->head;
        list->head = temp->next;
        if (free_course_object) {
            free_course(temp->course);
        }
        free(temp);
    }
    free(list);
}

void free_students(StudentList *list) {
    if (list == NULL) {
        return;
    }
    StudentNode *temp = NULL;
    while (list->head != NULL) {
        temp = list->head;
        list->head = temp->next;
        free(temp);
    }
    free(list);
}
