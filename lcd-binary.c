#include <stdio.h>  // debugging only
#include "gpio.h"
#include "lcd-binary.h"
#include "cw2-aux.h"


/* ***************************************************************************** */
/* HINT: use the CPP variable ASM with ifdef's to select Asm (or C) versions of the code. */
/* ***************************************************************************** */


/*
  Hardware Interface function.
  Set the mode for pin number @pin@ to @mode@ (can be INPUT or OUTPUT (encoded as int)).
*/

void pin_mode(volatile uint32_t *gpio, int pin, int mode)
{
  __asm__ volatile(
    "MOV R2, %[gpio_base]    \n\t"  // R2 = gpio base address
    "MOV R3, %[pin]          \n\t"  // R3 = pin number

    // Calculate fsel = pin / 10 using shifts and subtracts
    // Multiply pin by 1/10 approximation: (pin * 205) >> 11
    "MOV R5, #205            \n\t"  // R5 = magic number for div by 10
    "MUL R5, R3, R5          \n\t"  // R5 = pin * 205
    "LSR R5, R5, #11         \n\t"  // R5 = pin / 10 (fsel)

    // Calculate shift = (pin % 10) * 3
    // pin % 10 = pin - (fsel * 10)
    "MOV R4, #10             \n\t"  // R4 = 10
    "MUL R4, R5, R4          \n\t"  // R4 = fsel * 10
    "SUB R6, R3, R4          \n\t"  // R6 = pin % 10
    "MOV R4, #3              \n\t"  // R4 = 3
    "MUL R6, R6, R4          \n\t"  // R6 = (pin % 10) * 3 = shift

    // Read, modify, write the GPFSEL register
    "LDR R4, [R2, R5, LSL#2] \n\t"  // R4 = current GPFSEL register value
    "MOV R3, #7              \n\t"  // R3 = 0b111 mask
    "LSL R3, R3, R6          \n\t"  // shift mask to pin's bit position
    "BIC R4, R4, R3          \n\t"  // clear the 3 bits for this pin
    "MOV R3, %[mode]         \n\t"  // R3 = mode value
    "LSL R3, R3, R6          \n\t"  // shift mode to pin's bit position
    "ORR R4, R4, R3          \n\t"  // set the mode bits
    "STR R4, [R2, R5, LSL#2] \n\t"  // write back to GPFSEL register
    :
    : [gpio_base] "r" (gpio),
      [pin]       "r" (pin),
      [mode]      "r" (mode)
    : "r2", "r3", "r4", "r5", "r6"
  );
}

/*
  Hardware Interface function.
  Send a @value@ along pin number @pin@. Values should be LOW or HIGH (encoded as int).
*/
void digital_write(volatile uint32_t *gpio, int pin, int value)
{
  __asm__ volatile(
    "MOV R2, %[gpio_base]    \n\t"  // R2 = gpio base address.
    "MOV R3, #1              \n\t"  // R3 = 1.
    "MOV R4, %[pin]          \n\t"  // R4 = pin number.
    "LSL R3, R3, R4          \n\t"  // R3 = 1 << pin (bitmask).
    "MOV R4, %[value]        \n\t"  // R4 = value (HIGH or LOW).
    "CMP R4, #1              \n\t"  // compare value to 1 (HIGH).
    "MOVEQ R4, #7            \n\t"  // if HIGH, offset = 7 (GPSET0).
    "MOVNE R4, #10           \n\t"  // if LOW,  offset = 10 (GPCLR0).
    "STR R3, [R2, R4, LSL#2] \n\t"  // write bitmask to SET/CLR register.
    :
    : [gpio_base] "r" (gpio),
      [pin]       "r" (pin),
      [value]     "r" (value)
    : "r2", "r3", "r4"
  );
}

/*
  Hardware Interface function.
  Read input from a button device connected to pin @button@.. Result can be LOW or HIGH (encoded as int).
*/
int read_button(volatile uint32_t *gpio, int button)
{
  int result = 0;

  __asm__ volatile(
    "MOV R2, %[gpio_base]    \n\t"  // R2 = gpio base address.
    "MOV R3, #13             \n\t"  // R3 = GPLEV0 offset (13).
    "LDR R4, [R2, R3, LSL#2] \n\t" // R4 = value of GPLEV0 register.
    "MOV R3, %[button]       \n\t"  // R3 = button pin number.
    "LSR R4, R4, R3          \n\t"  // shift right by pin number.
    "AND %[result], R4, #1   \n\t"  // mask lowest bit = pin state.
    : [result]    "=r" (result)
    : [gpio_base] "r"  (gpio),
      [button]    "r"  (button)
    : "r2", "r3", "r4"
  );

  return result;
}