#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_POINTS 100

typedef struct
{
    int x, y;
} Point;

Point points[MAX_POINTS];
Point hull[MAX_POINTS * 2];

int n, hullSize;

// Compare function for sorting
int compare(const void *a, const void *b)
{
    Point *p1 = (Point *)a;
    Point *p2 = (Point *)b;

    if (p1->x == p2->x)
        return p1->y - p2->y;

    return p1->x - p2->x;
}

// Cross product
int cross(Point O, Point A, Point B)
{
    return (A.x - O.x) * (B.y - O.y) -
           (A.y - O.y) * (B.x - O.x);
}

// Convex Hull Algorithm
void convexHull()
{
    qsort(points, n, sizeof(Point), compare);

    int k = 0;

    // Lower hull
    for(int i = 0; i < n; i++)
    {
        while(k >= 2 &&
              cross(hull[k-2], hull[k-1], points[i]) <= 0)
        {
            k--;
        }

        hull[k++] = points[i];
    }

    // Upper hull
    for(int i = n - 2, t = k + 1; i >= 0; i--)
    {
        while(k >= t &&
              cross(hull[k-2], hull[k-1], points[i]) <= 0)
        {
            k--;
        }

        hull[k++] = points[i];
    }

    hullSize = k - 1;
}

// Draw callback
static void draw_callback(GtkDrawingArea *area,
                          cairo_t *cr,
                          int width,
                          int height,
                          gpointer data)
{
    // Background white
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    // Draw all points in blue
    for(int i = 0; i < n; i++)
    {
        cairo_set_source_rgb(cr, 0, 0, 1);

        cairo_arc(cr,
                  points[i].x,
                  points[i].y,
                  5,
                  0,
                  2 * 3.14159);

        cairo_fill(cr);

        // Draw labels
        cairo_move_to(cr,
                      points[i].x + 10,
                      points[i].y - 10);

        char label[50];

        sprintf(label,
                "(%d,%d)",
                points[i].x,
                points[i].y);

        cairo_show_text(cr, label);
    }

    // Draw convex hull lines in red
    cairo_set_source_rgb(cr, 1, 0, 0);

    cairo_set_line_width(cr, 3);

    for(int i = 0; i < hullSize; i++)
    {
        cairo_move_to(cr,
                      hull[i].x,
                      hull[i].y);

        cairo_line_to(cr,
                      hull[(i + 1) % hullSize].x,
                      hull[(i + 1) % hullSize].y);
    }

    cairo_stroke(cr);

    // Highlight hull points in green
    for(int i = 0; i < hullSize; i++)
    {
        cairo_set_source_rgb(cr, 0, 1, 0);

        cairo_arc(cr,
                  hull[i].x,
                  hull[i].y,
                  8,
                  0,
                  2 * 3.14159);

        cairo_fill(cr);
    }
}

// Activate GTK window
static void activate(GtkApplication *app,
                     gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *drawing_area;

    window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window),
                         "Land Boundary Detection using Graham Scan");

    gtk_window_set_default_size(GTK_WINDOW(window),
                                800,
                                600);

    drawing_area = gtk_drawing_area_new();

    gtk_drawing_area_set_draw_func(
        GTK_DRAWING_AREA(drawing_area),
        draw_callback,
        NULL,
        NULL
    );

    gtk_window_set_child(GTK_WINDOW(window),
                         drawing_area);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    printf("=====================================\n");
    printf(" LAND BOUNDARY DETECTION SYSTEM\n");
    printf(" USING GRAHAM SCAN ALGORITHM\n");
    printf("=====================================\n\n");

    printf("Enter number of points: ");
    scanf("%d", &n);

    printf("\nEnter coordinates (x y):\n");

    for(int i = 0; i < n; i++)
    {
        printf("Point %d: ", i + 1);

        scanf("%d %d",
              &points[i].x,
              &points[i].y);
    }

    convexHull();

    GtkApplication *app;

    app = gtk_application_new(
        "com.example.ConvexHull",
        G_APPLICATION_DEFAULT_FLAGS
    );

    g_signal_connect(app,
                     "activate",
                     G_CALLBACK(activate),
                     NULL);

    int status = g_application_run(
        G_APPLICATION(app),
        argc,
        argv
    );

    g_object_unref(app);

    return status;
}