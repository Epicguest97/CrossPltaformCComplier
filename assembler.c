#include <stdio.h>
#include <string.h>


void generate_assembly(int return_value) {
    FILE *file = fopen("output.s", "w");
    if (!file) {
        fprintf(stderr, "Error: Could not create assembly file.\n");
        return;
    }

   
    fprintf(file, ".intel_syntax noprefix\n");
    fprintf(file, ".global _start\n");

    
    fprintf(file, "_start:\n");

    \
    fprintf(file, "    mov eax, %d\n", return_value);  
    fprintf(file, "    ret\n");  

    
    fclose(file);

    printf("Assembly code has been generated in output.s\n");
}

int main() {
    
    generate_assembly(42);
    return 0;
}
