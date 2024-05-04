#include <stdio.h>
#include <stdlib.h>

int main() {
    // Allocate memory for an array of 5 integers
    int *ptr = (int*)malloc(5 * sizeof(int));
    if (ptr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Populate the array with some initial values
    for (int i = 0; i < 5; i++) {
        ptr[i] = i + 1;
    }

    // Output the original array
    printf("Original array: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", ptr[i]);
    }
    printf("\n");

    // Resize the array to hold 8 integers
    int *new_ptr = (int*)realloc(ptr, 8 * sizeof(int));
    if (new_ptr == NULL) {
        printf("Memory reallocation failed\n");
        free(ptr); // Free original memory block before exiting
        return 1;
    }

    // Populate the array with new values
    for (int i = 5; i < 8; i++) {
        new_ptr[i] = i + 1;
    }

    // Update pointer to the newly allocated memory block
    ptr = new_ptr;

    // Output the resized array (with uninitialized values for the new elements)
    printf("Resized array: ");
    for (int i = 0; i < 8; i++) {
        printf("%d ", ptr[i]);
    }
    printf("\n");

    // Free dynamically allocated memory
    free(ptr);

    return 0;
}
