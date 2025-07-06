#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char *result;
    
    // Test realloc with NULL pointer (should act like malloc)
    result = realloc(NULL, 10);
    if (result) {
        strcpy(result, "Bonjour");
        printf("%s\n", result);
        
        // Test realloc with size 0 (should act like free and return NULL)
        result = realloc(result, 0);
        if (result == NULL) {
            printf("realloc(ptr, 0) correctly returned NULL\n");
        }
    }
    
    return 0;
}
