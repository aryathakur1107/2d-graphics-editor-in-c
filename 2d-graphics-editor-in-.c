#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROWS 22
#define COLS 60
#define MAX_OBJECTS 100

// Define shape types
typedef enum { LINE, RECTANGLE, CIRCLE, TRIANGLE } ShapeType;

// Structure to track individual graphical objects
typedef struct {
    int id;
    ShapeType type;
    int x1, y1; // Point 1 / Center for Circle / Top-left for Rect
    int x2, y2; // Point 2 / Bottom-right for Rect
    int x3, y3; // Point 3 for Triangle
    int radius; // For Circle
    int active; // 1 if object exists, 0 if deleted
} Shape;

// Global variables
char canvas[ROWS][COLS];
Shape object_list[MAX_OBJECTS];
int object_counter = 0;

// Function Prototypes
void init_canvas();
void display_canvas();
void draw_line(int x1, int y1, int x2, int y2);
void draw_rectangle(int x1, int y1, int x2, int y2);
void draw_circle(int xc, int yc, int r);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void redraw_canvas();
void add_object();
void delete_object();
void modify_object();
void list_objects();

int main() {
    int choice;
    init_canvas();

    while (1) {
        printf("\n=== 2D TEXT GRAPHICS EDITOR ===\n");
        printf("1. Display Canvas\n");
        printf("2. Add Object (Line, Rectangle, Circle, Triangle)\n");
        printf("3. Delete Object\n");
        printf("4. Modify Object\n");
        printf("5. List Active Objects\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1:
                display_canvas();
                break;
            case 2:
                add_object();
                break;
            case 3:
                delete_object();
                break;
            case 4:
                modify_object();
                break;
            case 5:
                list_objects();
                break;
            case 6:
                printf("Exiting Graphics Editor. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}

// Initialize canvas with underscores
void init_canvas() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            canvas[i][j] = '_';
        }
    }
}

// Print canvas to screen
void display_canvas() {
    printf("\n");
    // Print column header index for reference
    printf("   ");
    for(int j=0; j<COLS; j++) {
        if(j % 10 == 0) printf("%d", j/10);
        else printf(" ");
    }
    printf("\n   ");
    for(int j=0; j<COLS; j++) printf("%d", j%10);
    printf("\n");

    for (int i = 0; i < ROWS; i++) {
        printf("%2d ", i); // Row index
        for (int j = 0; j < COLS; j++) {
            printf("%c", canvas[i][j]);
        }
        printf("\n");
    }
}

// Plot a pixel safely within canvas boundaries
void plot_pixel(int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        canvas[y][x] = '*';
    }
}

// Bresenham's Line Algorithm
void draw_line(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        plot_pixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

// Draw a rectangle using 4 lines
void draw_rectangle(int x1, int y1, int x2, int y2) {
    draw_line(x1, y1, x2, y1); // Top
    draw_line(x2, y1, x2, y2); // Right
    draw_line(x2, y2, x1, y2); // Bottom
    draw_line(x1, y2, x1, y1); // Left
}

// Helper for circle octants
void plot_circle_points(int xc, int yc, int x, int y) {
    plot_pixel(xc + x, yc + y);
    plot_pixel(xc - x, yc + y);
    plot_pixel(xc + x, yc - y);
    plot_pixel(xc - x, yc - y);
    plot_pixel(xc + y, yc + x);
    plot_pixel(xc - y, yc + x);
    plot_pixel(xc + y, yc - x);
    plot_pixel(xc - y, yc - x);
}

// Midpoint Circle Algorithm
void draw_circle(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    plot_circle_points(xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot_circle_points(xc, yc, x, y);
    }
}

// Draw a triangle using 3 lines
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// Clear canvas and redraw all active objects
void redraw_canvas() {
    init_canvas();
    for (int i = 0; i < object_counter; i++) {
        if (object_list[i].active) {
            switch (object_list[i].type) {
                case LINE:
                    draw_line(object_list[i].x1, object_list[i].y1, object_list[i].x2, object_list[i].y2);
                    break;
                case RECTANGLE:
                    draw_rectangle(object_list[i].x1, object_list[i].y1, object_list[i].x2, object_list[i].y2);
                    break;
                case CIRCLE:
                    draw_circle(object_list[i].x1, object_list[i].y1, object_list[i].radius);
                    break;
                case TRIANGLE:
                    draw_triangle(object_list[i].x1, object_list[i].y1, object_list[i].x2, object_list[i].y2, object_list[i].x3, object_list[i].y3);
                    break;
            }
        }
    }
}

// Add a new object
void add_object() {
    if (object_counter >= MAX_OBJECTS) {
        printf("Object storage full!\n");
        return;
    }

    int type_choice;
    printf("\nSelect Shape Type:\n1. Line\n2. Rectangle\n3. Circle\n4. Triangle\nChoice: ");
    scanf("%d", &type_choice);

    Shape new_shape;
    new_shape.id = object_counter + 1;
    new_shape.active = 1;

    if (type_choice == 1) {
        new_shape.type = LINE;
        printf("Enter coordinates (X1 Y1 X2 Y2) [Max X=%d, Y=%d]: ", COLS-1, ROWS-1);
        scanf("%d %d %d %d", &new_shape.x1, &new_shape.y1, &new_shape.x2, &new_shape.y2);
    } else if (type_choice == 2) {
        new_shape.type = RECTANGLE;
        printf("Enter Top-Left (X1 Y1) and Bottom-Right (X2 Y2): ");
        scanf("%d %d %d %d", &new_shape.x1, &new_shape.y1, &new_shape.x2, &new_shape.y2);
    } else if (type_choice == 3) {
        new_shape.type = CIRCLE;
        printf("Enter Center coordinates (Xc Yc) and Radius: ");
        scanf("%d %d %d", &new_shape.x1, &new_shape.y1, &new_shape.radius);
    } else if (type_choice == 4) {
        new_shape.type = TRIANGLE;
        printf("Enter 3 vertex coordinates (X1 Y1 X2 Y2 X3 Y3): ");
        scanf("%d %d %d %d %d %d", &new_shape.x1, &new_shape.y1, &new_shape.x2, &new_shape.y2, &new_shape.x3, &new_shape.y3);
    } else {
        printf("Invalid Shape choice!\n");
        return;
    }

    object_list[object_counter] = new_shape;
    object_counter++;
    redraw_canvas();
    printf("Shape added successfully with ID: %d\n", new_shape.id);
}

// Delete an object by resetting its active status
void delete_object() {
    list_objects();
    if (object_counter == 0) return;

    int target_id;
    printf("Enter the ID of the object to delete: ");
    scanf("%d", &target_id);

    for (int i = 0; i < object_counter; i++) {
        if (object_list[i].id == target_id && object_list[i].active) {
            object_list[i].active = 0;
            redraw_canvas();
            printf("Object %d deleted successfully.\n", target_id);
            return;
        }
    }
    printf("Object ID not found or already deleted.\n");
}

// Modify an existing object's properties
void modify_object() {
    list_objects();
    if (object_counter == 0) return;

    int target_id;
    printf("Enter the ID of the object to modify: ");
    scanf("%d", &target_id);

    for (int i = 0; i < object_counter; i++) {
        if (object_list[i].id == target_id && object_list[i].active) {
            printf("Modifying Object %d...\n", target_id);
            if (object_list[i].type == LINE) {
                printf("Enter new coordinates (X1 Y1 X2 Y2): ");
                scanf("%d %d %d %d", &object_list[i].x1, &object_list[i].y1, &object_list[i].x2, &object_list[i].y2);
            } else if (object_list[i].type == RECTANGLE) {
                printf("Enter new Top-Left (X1 Y1) and Bottom-Right (X2 Y2): ");
                scanf("%d %d %d %d", &object_list[i].x1, &object_list[i].y1, &object_list[i].x2, &object_list[i].y2);
            } else if (object_list[i].type == CIRCLE) {
                printf("Enter new Center (Xc Yc) and Radius: ");
                scanf("%d %d %d", &object_list[i].x1, &object_list[i].y1, &object_list[i].radius);
            } else if (object_list[i].type == TRIANGLE) {
                printf("Enter new 3 vertices (X1 Y1 X2 Y2 X3 Y3): ");
                scanf("%d %d %d %d %d %d", &object_list[i].x1, &object_list[i].y1, &object_list[i].x2, &object_list[i].y2, &object_list[i].x3, &object_list[i].y3);
            }
            redraw_canvas();
            printf("Object %d updated successfully.\n", target_id);
            return;
        }
    }
    printf("Object ID not found.\n");
}

// Helper to display current active shapes
void list_objects() {
    int count = 0;
    printf("\n--- Active Objects ---\n");
    for (int i = 0; i < object_counter; i++) {
        if (object_list[i].active) {
            count++;
            printf("ID: %d | Type: ", object_list[i].id);
            switch (object_list[i].type) {
                case LINE: printf("Line [(%d,%d) to (%d,%d)]\n", object_list[i].x1, object_list[i].y1, object_list[i].x2, object_list[i].y2); break;
                case RECTANGLE: printf("Rectangle [TL:(%d,%d), BR:(%d,%d)]\n", object_list[i].x1, object_list[i].y1, object_list[i].x2, object_list[i].y2); break;
                case CIRCLE: printf("Circle [Center:(%d,%d), Radius:%d]\n", object_list[i].x1, object_list[i].y1, object_list[i].radius); break;
                case TRIANGLE: printf("Triangle [(%d,%d), (%d,%d), (%d,%d)]\n", object_list[i].x1, object_list[i].y1, object_list[i].x2, object_list[i].y2, object_list[i].x3, object_list[i].y3); break;
            }
        }
    }
    if (count == 0) {
        printf("(None)\n");
    }
}