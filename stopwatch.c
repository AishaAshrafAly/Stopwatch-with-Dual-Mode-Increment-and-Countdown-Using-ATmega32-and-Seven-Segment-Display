#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Global variables
unsigned char modeFlag = 0; // Mode toggle flag
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;

// Button state flags
unsigned char decSecondsFlag = 0; // Decrement seconds button (PB5)
unsigned char incSecondsFlag = 0; // Increment seconds button (PB6)
unsigned char decMinutesFlag = 0; // Decrement minutes button (PB3)
unsigned char incMinutesFlag = 0; // Increment minutes button (PB4)
unsigned char decHoursFlag = 0;   // Decrement hours button (PB0)
unsigned char incHoursFlag = 0;   // Increment hours button (PB1)

// Interrupt Service Routines
ISR(INT0_vect) {
    // Reset stopwatch and timer
    TCNT1 = 0;
    hours = 0;
    minutes = 0;
    seconds = 0;
}

ISR(INT1_vect) {
    // Pause timer
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
}

ISR(INT2_vect) {
    // Resume timer
    TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12);
}

ISR(TIMER1_COMPA_vect) {
    if (!modeFlag) {
        // Stopwatch mode
        PORTD &= ~(1 << PD0); // Buzzer OFF
        PORTD |= (1 << PD4);  // Red LED ON
        PORTD &= ~(1 << PD5); // Yellow LED OFF

        seconds++;
        if (seconds == 60) {
            minutes++;
            seconds = 0;
        }
        if (minutes == 60) {
            hours++;
            minutes = 0;
        }
        if (hours == 100) { // Maximum hours
            hours = 0;
            minutes = 0;
            seconds = 0;
        }
    } else {
        // Timer mode
        PORTD &= ~(1 << PD4); // Red LED OFF
        PORTD |= (1 << PD5);  // Yellow LED ON
        if (seconds == 0) {
            if (minutes == 0) {
                if (hours == 0) {
                    PORTD |= (1 << PD0); // Buzzer ON
                } else {
                    hours--;
                    minutes = 59;
                    seconds = 59;
                }
            } else {
                minutes--;
                seconds = 59;
            }
        } else {
            seconds--;
        }
    }
}

void TIMER1_CTC_INIT(void) {
    TCNT1 = 0;
    TCCR1A = (1 << FOC1A);
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // Prescaler = 1024
    OCR1A = 15625; // Compare value
    TIMSK |= (1 << OCIE1A); // Enable Timer1 compare match interrupt
    SREG |= (1 << 7); // Enable global interrupts
}

void INT0_Init(void) {
    DDRD &= ~(1 << PD2); // Configure INT0/PD2 as input
    PORTD |= (1 << PD2); // Enable pull-up resistor
    MCUCR |= (1 << ISC01); // Falling edge interrupt
    MCUCR &= ~(1 << ISC00);
    GICR |= (1 << INT0); // Enable INT0
}

void INT1_Init(void) {
    DDRD &= ~(1 << PD3); // Configure INT1/PD3 as input
    MCUCR |= (1 << ISC11) | (1 << ISC10); // Rising edge interrupt
    GICR |= (1 << INT1); // Enable INT1
}

void INT2_Init(void) {
    DDRB &= ~(1 << PB2); // Configure INT2/PB2 as input
    PORTB |= (1 << PB2); // Enable pull-up resistor
    MCUCSR &= ~(1 << ISC2); // Falling edge interrupt
    GICR |= (1 << INT2); // Enable INT2
}

void Mode_Toggle(void) {
    modeFlag ^= 1; // Toggle mode
    PORTD ^= (1 << PD4); // Toggle Red LED
    PORTD ^= (1 << PD5); // Toggle Yellow LED
    _delay_ms(5); // Debounce delay
}

int main(void) {
    unsigned char buttonState = 0; // Current button state
    unsigned char prevButtonState = 0; // Previous button state

    // Configure ports
    DDRB &= 0x3F; // Configure first 6 pins of PORTB as input
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB4) | (1 << PB5) | (1 << PB6); // Enable pull-up resistors

    DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD0); // Configure PD4 (Red LED), PD5 (Yellow LED), PD0 (Buzzer) as output

    DDRB &= ~(1 << PB7); // Configure PB7 as input
    PORTB |= (1 << PB7); // Enable pull-up resistor

    DDRA |= 0x3F; // Configure first 6 pins of PORTA as output
    PORTA &= 0xC0; // Initial values for PORTA

    DDRC |= 0x0F; // Configure first 4 pins of PORTC as output
    PORTC &= 0xF0; // Initial values for PORTC

    PORTD |= (1 << PD4); // Turn on Red LED for increment mode

    // Initialize peripherals
    TIMER1_CTC_INIT();
    INT0_Init();
    INT1_Init();
    INT2_Init();

    while (1) {
        buttonState = PINB & (1 << PB7); // Read button state (PB7)

        if (buttonState == 0 && prevButtonState == (1 << PB7)) {
            Mode_Toggle(); // Toggle between increment and countdown modes
        }

        prevButtonState = buttonState;

        // Handle increment/decrement of hours, minutes, and seconds
        if (!(PINB & (1 << PB1)) && !incHoursFlag) {
            hours++;
            incHoursFlag = 1;
        } else if (PINB & (1 << PB1)) {
            incHoursFlag = 0;
        }

        if (!(PINB & (1 << PB0)) && !decHoursFlag && hours > 0) {
            hours--;
            decHoursFlag = 1;
        } else if (PINB & (1 << PB0)) {
            decHoursFlag = 0;
        }

        if (!(PINB & (1 << PB4)) && !incMinutesFlag) {
            minutes++;
            incMinutesFlag = 1;
        } else if (PINB & (1 << PB4)) {
            incMinutesFlag = 0;
        }

        if (!(PINB & (1 << PB3)) && !decMinutesFlag && minutes > 0) {
            minutes--;
            decMinutesFlag = 1;
        } else if (PINB & (1 << PB3)) {
            decMinutesFlag = 0;
        }

        if (!(PINB & (1 << PB6)) && !incSecondsFlag) {
            seconds++;
            incSecondsFlag = 1;
        } else if (PINB & (1 << PB6)) {
            incSecondsFlag = 0;
        }

        if (!(PINB & (1 << PB5)) && !decSecondsFlag && seconds > 0) {
            seconds--;
            decSecondsFlag = 1;
        } else if (PINB & (1 << PB5)) {
            decSecondsFlag = 0;
        }

        // Handle overflows
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
        }

        if (minutes >= 60) {
            minutes = 0;
            hours++;
        }

        if (hours >= 100) {
            hours = 0;
        }

        // Display values on seven-segment display
        // Display seconds
        PORTA = (1 << PA5);
        PORTC = (PORTC & 0xF0) | (seconds % 10);
        _delay_ms(5);

        PORTA = (1 << PA4);
        PORTC = (PORTC & 0xF0) | (seconds / 10);
        _delay_ms(5);

        // Display minutes
        PORTA = (1 << PA3);
        PORTC = (PORTC & 0xF0) | (minutes % 10);
        _delay_ms(5);

        PORTA = (1 << PA2);
        PORTC = (PORTC & 0xF0) | (minutes / 10);
        _delay_ms(5);

        // Display hours
        PORTA = (1 << PA1);
        PORTC = (PORTC & 0xF0) | (hours % 10);
        _delay_ms(5);

        PORTA = (1 << PA0);
        PORTC = (PORTC & 0xF0) | (hours / 10);
        _delay_ms(5);
    }
}
