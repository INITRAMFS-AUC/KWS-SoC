// flash_read_test.c
volatile int my_initialized_var = 42; // Goes to .data (copied from Flash)
volatile int my_zeroed_var;           // Goes to .bss (zeroed by crt0.s)

int main() {
    // If crt0.s worked, my_initialized_var will be 42, not garbage.
    my_initialized_var += 1;
    my_zeroed_var = my_initialized_var;

    while(1) {}
    return 0;
}
