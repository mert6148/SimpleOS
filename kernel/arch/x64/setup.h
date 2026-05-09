#define SETUP_H

#include <iostream>
#include <setup.c>

void setup_idt();
void setup_gdt();
void setup_paging();
void setup_exceptions();
void setup_irq();
void setup_timer();
void setup_console();
void setup_keyboard();
void setup_mouse();
void setup_sound();
void setup_network();