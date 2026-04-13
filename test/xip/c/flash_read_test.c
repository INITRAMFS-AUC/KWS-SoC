// flash_read_test.c
volatile int my_initialized_var = 42;
volatile int my_zeroed_var;

int main() {
    volatile int *ptr = &my_initialized_var;
    int val = *ptr;     // Explicit read
    val += 1;           // Increment
    *ptr = val;         // Write back
    my_zeroed_var = val;

    while(1) {}
    return 0;
}
