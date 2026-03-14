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

// Double check if this should be in C or ASM.
void pin_mode(volatile uint32_t *gpio, int pin, int mode)
{
  int fsel = pin / 10; // This gets the fsel by dividing by 10 since the fsel is the first number, e.g., 27/10 = 2 fsel=2.
  int shift = (pin % 10) * 3; // Multiply second digit by 3 to get the shift value.

  __asm__ volatile(
    "LDR R2, %[gpio_base]    \n\t"   // R2 = base address for GPIO.
    "MOV R3, %[fsel]          \n\t"  // R3 = register offset.
    "LDR R4, [R2, R3, LSL#2] \n\t"   // R4 = current register value.
    "MOV R5, #0b111          \n\t"   // R5 = 3-bit mask.
    "MOV R6, %[shift]        \n\t"   // R6 = shift amount.
    "LSL R5, R5, R6          \n\t"   // shift mask to correct position.
    "BIC R4, R4, R5          \n\t"   // clear the 3 bits for this pin.
    "MOV R5, %[mode]         \n\t"   // R5 = mode, where 0 = input and 1 = output.
    "LSL R5, R5, R6          \n\t"   // shift mode to correct position.
    "ORR R4, R4, R5          \n\t"   // set the bits.
    "STR R4, [R2, R3, LSL#2] \n\t"   // write back to register.
    :
    : [gpio_base] "m" (*gpio),
      [fsel]       "r" (fsel),
      [shift]     "r" (shift),
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
  // offset 7 = GPSET0, offset 10 = GPCLR0
  int offset = (value == HIGH) ? 7 : 10;

  __asm__ volatile(
    "LDR R2, %[gpio_base]    \n\t"  // R2 = gpio base address.
    "MOV R3, #1              \n\t"  // R3 = 1.
    "MOV R4, %[pin]          \n\t"  // R4 = pin number.
    "LSL R3, R3, R4          \n\t"  // R3 = 1 << pin (bitmask).
    "MOV R4, %[offset]       \n\t"  // R4 = SET or CLR offset.
    "STR R3, [R2, R4, LSL#2] \n\t"  // write bitmask to SET/CLR register.
    :
    : [gpio_base] "m" (*gpio),
      [pin]       "r" (pin),
      [offset]    "r" (offset)
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
    "LDR R2, %[gpio_base]    \n\t"  // R2 = gpio base address.
    "MOV R3, #13             \n\t"  // R3 = GPLEV0 offset (13).
    "LDR R4, [R2, R3, LSL#2] \n\t"  // R4 = value of GPLEV0 register.
    "MOV R3, %[button]       \n\t"  // R3 = button pin number.
    "LSR R4, R4, R3          \n\t"  // shift right by pin number.
    "AND %[result], R4, #1   \n\t"  // mask lowest bit = pin state.
    : [result] "=r" (result)
    : [gpio_base] "m" (*gpio),
      [button]    "r" (button)
    : "r2", "r3", "r4"
  );

  return result;
}