/* KBDUS means US Keyboard Layout. This is a scancode table
 *  used to layout a standard US keyboard. I have left some
 *  comments in to give you an idea of what key is what, even
 *  though I set it's array index to 0. You can change that to
 *  whatever you want using a macro, if you wish! */
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',/* 9 */
    '9', '0', '-', '=', '\b',/* Backspace */
    '\t',/* Tab */
    'q', 'w', 'e', 'r',/* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',/* Enter key */
    0,/* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',/* 39 */
    '\'', '`',   0,/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',/* 49 */
    'm', ',', '.', '/',   0,/* Right shift */
    '*',
    0,/* Alt */
    ' ',/* Space bar */
    0,/* Caps lock */
    0,/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,/* < ... F10 */
    0,/* 69 - Num lock*/
    0,/* Scroll Lock */
    0,/* Home key */
    0,/* Up Arrow */
    0,/* Page Up */
    '-',
    0,/* Left Arrow */
    0,
    0,/* Right Arrow */
    '+',
    0,/* 79 - End key*/
    0,/* Down Arrow */
    0,/* Page Down */
    0,/* Insert Key */
    0,/* Delete Key */
    0,   0,   0,
    0,/* F11 Key */
    0,/* F12 Key */
    0,/* All other keys are undefined */
};

unsigned int pressed_keys[32]; // 256 keys

void center_string(int color, const char *string, int y) {
    const char *ptr = string;
    int len = 0;
    while(*ptr++ != 0) len++;
    volatile char *video = (volatile char*)0xB8000;
    video += 2 * 80 * y;
    video += 2 * (40 - (len / 2));
    while(*string != 0) {
        *video++ = *string++;
        *video++ = color;
    }
}

void print_string(int color, const char *string, int x, int y) {
    volatile char *video = (volatile char*)0xB8000;
    video += 2 * 80 * y;
    video += 2 * x;
    while(*string != 0) {
        *video++ = *string++;
        *video++ = color;
    }
}

void print_char(int color, const char c, int x, int y) {
    volatile char *video = (volatile char*)0xB8000;
    video += 2 * 80 * y;
    video += 2 * x;
    *video++ = c;
    *video++ = color;
}

void print_integer(int color, int i, int x, int y) {
    volatile char *video = (volatile char*)0xB8000;
    video += 2 * 80 * y;
    video += 2 * x;
    char c;
    if(i < 0) {
	c = '-';
	print_integer(color, -i, x + 1, y);
    }
    if(i < 10) {
	c = '0' + i;
    } else {
	c = '0' + (i % 10);
	print_integer(color, i / 10, x + 1, y);
    }
    *video++ = c;
    *video++ = color;
}

void clear(int color) {
    volatile char *video = (volatile char*)0xB8000;
    int i;
    for(i = 0; i < 80 * 25 * 2; i++) {
	*video++ = ' ';
	*video++ = color;
    }
}

// http://www.osdever.net/bkerndev/Docs/creatingmain.htm
unsigned char inportb(unsigned short _port) {
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb(unsigned short _port, unsigned char _data) {
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void poll(void) {
    unsigned char k;
    k = inportb(0x60);
    if(k & 0x80) {
	k &= ~0x80;
	k = kbdus[k];
	int seg = k / 16;
	int part = k - (seg * 16);
	pressed_keys[seg] &= ~(1 << part);
    } else {
	k = kbdus[k];
	int seg = k / 16;
	int part = k - (seg * 16);
	pressed_keys[seg] |= (1 << part);
    }
}

int is_key_down(unsigned char key) {
    int seg = key / 16;
    int part = key - (seg * 16);
    return pressed_keys[seg] & (1 << part);
};

int x = 40;
int y = 12;
void kmain(void) {
    clear(0x1F);
    center_string(0xF1, " STUPIDKERNEL ", 7);
    print_string(0x1F, "A fatal exception WAT has occured at 0000:DEADBEEF in VXD UMM(01) +", 4, 10);
    print_string(0x1F, "1BADBOO2.  The current application will be terminated.", 4, 11);
    print_string(0x1F, "*  Press any key to terminate the curent application.", 4, 13);
    print_string(0x1F, "*  Press CTRL+ALT+DEL to restart your computer.", 4, 14);
    center_string(0x1F, "Press any key to continue _", 16);

    unsigned int timer = 0;
    while(1) {
	timer++;
	if(((timer + timer + timer + timer + timer + timer + timer + timer) ^ ~timer) == timer) {
	    timer ^= timer ^ timer;
	}
	if(timer > 400000) {
	    timer = 0;
	    poll();
	    print_string(0x1F, "  ", x, y);
	    if(is_key_down('a')) {
		x -= 2;
	    }
	    if(is_key_down('d')) {
		x += 2;
	    }
	    if(is_key_down('w')) {
		y -= 1;
	    }
	    if(is_key_down('s')) {
		y += 1;
	    }
	    print_string(0x1F, "##", x, y);
	}
    }
}
