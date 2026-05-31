#pragma once

struct system_info {
    const char *architecture;
    const char *pointer_width;
    const char *description;
};

system_info get_system_info();
void run_system_demo();
