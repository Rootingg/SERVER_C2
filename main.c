#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cli.h"

int main(void) {
    srand((unsigned int)time(NULL));
    run_cli();
    return 0;
}