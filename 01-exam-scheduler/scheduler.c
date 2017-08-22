#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define NAME_LENGTH 3

#define STATUS_SECTION_EMPTY 1
#define STATUS_SECTION_CASE 2
#define STATUS_SECTION_COURSE 3
#define STATUS_SECTION_STUDENT 4
#define STATUS_SECTION_END 5

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
char *parse_courses(CourseList *);
void check_course_conflicts(CourseList *);
void color_courses(CourseList *);

void push_course(CourseList *, Course *);
void merge_course_list(CourseList *, CourseList *);
bool course_list_contains(CourseList *, Course *);
void free_course_list(CourseList *, bool);

void push_student(StudentList *, char[NAME_LENGTH]);
void free_student_list(StudentList *);

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
};

struct StudentList {
    StudentNode *head;
};

int main(int argc, char *argv[]) {
    int num_cases = 0;
    scanf("%d\n", &num_cases);

    for (int i = 0; i < num_cases; i++) {
        CourseList *courses = (CourseList *) malloc(sizeof(CourseList));
        courses->head = NULL;
        courses->tail = NULL;

        char *case_name = parse_courses(courses);
        check_course_conflicts(courses);
        color_courses(courses);

        #if !DEBUG
            printf("Case %s: (", case_name);
        #endif

        for (CourseNode *n = courses->head; n != NULL; n = n->next) {
            Course *course = n->course;

            #if DEBUG
                printf("%s (%d):", course->name, course->schedule);
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
                    StudentNode *s = course->students->head;
                    while (s != NULL) {
                        printf("  %s\n", s->name);
                        s = s->next;
                    }
                }
            #else
                printf("%s:%d", course->name, course->schedule);
                if (n->next != NULL) {
                    printf(", ");
                }
            #endif
        }

        #if !DEBUG
            printf(")\n");
        #endif
        free_course_list(courses, true);
        free(case_name);
    }
}

char *parse_courses(CourseList *courses) {
    int status = STATUS_SECTION_EMPTY,
        name_index = 1,
        case_name_index = 0;

    char c, name[NAME_LENGTH], *case_name = NULL;
    case_name = malloc((size_t) 10);
    memset(name, '\0', NAME_LENGTH);
    memset(case_name, '\0', 10);

    CourseNode *course_index = NULL;

    while ((c = fgetc(stdin)) != EOF) {
        if (c == '\n') {
            break;
        } else if (status == STATUS_SECTION_EMPTY && c == 'C') {
            status = STATUS_SECTION_CASE;
        } else if (status == STATUS_SECTION_CASE) {
            if (c == ':') {
                status = STATUS_SECTION_COURSE;
                case_name_index = 0;
            } else if (isdigit(c)) {
                case_name[case_name_index++] = c;
            }
        } else if (status == STATUS_SECTION_COURSE && c == '(') {
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
            } else if (course_index->next != NULL) {
                course_index = course_index->next;
            } else {
                printf("Unexpected student list\n");
                exit(1);
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

    return case_name;
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

    bool conflict = false;

    for (CourseNode *current = courses->head; current != NULL; current = current->next) {
        for (CourseNode *next = current->next; next != NULL; next = next->next) {
            Course *c = current->course;
            Course *c_next = next->course;

            StudentNode *s1 = c->students->head;
            StudentNode *s2 = NULL;

            conflict = false;

            while (s1 != NULL) {
                s2 = c_next->students->head;
                while (s2 != NULL) {
                    if (
                        s1->name[0] == s2->name[0] &&
                        s1->name[1] == s2->name[1] &&
                        s1->name[3] == s2->name[3]
                    ) {
                        conflict = true;
                        break;
                    }
                    s2 = s2->next;
                }
                if (conflict) {
                    break;
                }
                s1 = s1->next;
            }

            if (conflict) {
                push_course(c->conflicts, c_next);
                push_course(c_next->conflicts, c);
            }
        }
    }
}

void color_courses(CourseList *list) {
    Course *c = NULL;
    Course *c_next = NULL;

    CourseNode *current = list->head;
    CourseNode *next = NULL;

    int available_color = 1;
    for (current = list->head; current != NULL; current = current->next) {
        c = current->course;
        if (c->schedule != 0) {
            continue;
        }

        c->schedule = available_color;

        CourseList *group = malloc(sizeof(CourseList));
        group->head = NULL;
        group->tail = NULL;

        merge_course_list(group, c->conflicts);

        for (next = current->next; next != NULL; next = next->next) {
            c_next = next->course;
            if (c_next->schedule != 0 || course_list_contains(group, c_next)) {
                continue;
            }

            push_course(group, c_next);
            merge_course_list(group, c_next->conflicts);

            c_next->schedule = available_color;
        }
        free_course_list(group, false);

        // At this point we should've colored all nodes not adjacent to
        // the current node. Make a new color available.
        available_color++;
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
}

void push_student(StudentList *list, char name[NAME_LENGTH]) {
    StudentNode *s = malloc(sizeof(StudentNode));
    memset(s->name, '\0', NAME_LENGTH + 1);
    s->name[0] = name[0];
    s->name[1] = name[1];
    s->name[2] = name[2];

    s->next = list->head;
    list->head = s;
}

void merge_course_list(CourseList *a, CourseList *b) {
    for (CourseNode *n = b->head; n != NULL; n = n->next) {
        if (!course_list_contains(a, n->course)) {
            push_course(a, n->course);
        }
    }
}

bool course_list_contains(CourseList *list, Course *course) {
    CourseNode *n = list->head;
    while (n != NULL) {
        if (strcmp(n->course->name, course->name) == 0) {
            return true;
        }
        n = n->next;
    }
    return false;
}

void free_course(Course *c) {
    free_student_list(c->students);
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

void free_student_list(StudentList *list) {
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
