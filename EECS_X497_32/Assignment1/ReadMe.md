# Basic I/O Program

Below are the instructions you saw in the preview. This assignment is not graded.
 
int main (void)
{
// Set PORTR Direction Register (DIR) as output
*((volatile unsigned char *)0x7E0) = 0xFF;
// Turn LED0 ON and LED 1 OFF, by writing 0xFE to PORTR Toggle Register(OUTTGL), bit 0 set to 0 to
turn On LED 0
*((volatile unsigned char *)0x7E7) = 0xFE;
}

