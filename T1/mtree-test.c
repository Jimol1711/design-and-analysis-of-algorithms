#include "mtree.h"
#include "cp.h"
#include "ss.h"

int main() {
    // Create a root node for the M-tree
    Node* root = create_node();

    // Create some points for testing
    Point p1 = {1.0, 1.0};
    Point p2 = {2.0, 2.0};
    Point p3 = {3.0, 3.0};
    Point p4 = {4.0, 4.0};

    // Insert entries into the root node
    root->entrys[0].p = p1;
    root->entrys[0].cr = 1.0;
    root->entrys[0].a = create_node();

    root->entrys[1].p = p2;
    root->entrys[1].cr = 1.5;
    root->entrys[1].a = create_node();

    root->entrys[2].p = p3;
    root->entrys[2].cr = 2.0;
    root->entrys[2].a = create_node();

    // Create a query
    Query query;
    query.q = p1;  // Query point
    query.r = 1.5; // Query radius

    // Search points within the specified radius
    Point* result = search_points_in_radio(root, query);

    // Print the points found
    printf("Points within the radius:\n");
    for (int i = 0; result[i].x != 0 || result[i].y != 0; ++i) {
        printf("Point %d: (%.2f, %.2f)\n", i + 1, result[i].x, result[i].y);
    }

    // Free the memory allocated for the result array
    free(result);

    return 0;
}
