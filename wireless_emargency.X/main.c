#include "mcc_generated_files/mcc.h"
#include "Controller_Protocol.h"

#define Bluetooth_Reset_pin LATCbits.LATC1
#define Bluetooth_Reset_TRIS TRISCbits.TRISC1
#define emargency_switch PORTCbits.RC2
#define debug_LED0 LATCbits.LATC5

void EUSART_Write_emargency(unsigned char);
void EUSART_TxInterrupt_Control_emargency(bool);

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    void Initialize_Parameters();
    PIE1bits.TXIE = 1;
    Bluetooth_Reset_TRIS = 1;
    
    unsigned char emargency_data = 0;

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    __delay_ms(50);

    while (1)
    {
        // Add your application code
        
        debug_LED0 = emargency_switch;
        TxData0[0] = emargency_switch;
        Send_StartSignal(EUSART_Write_emargency, EUSART_TxInterrupt_Control_emargency, PIE1bits.TXIE);
        
        
    }
}
/**
 End of File
*/

void EUSART_Write_emargency(unsigned char txData) {
    //配列 txDataの先頭アドレスを渡す
    TXREG = txData;
}

void EUSART_TxInterrupt_Control_emargency(bool enable_or_disable) {
    PIE1bits.TXIE = enable_or_disable;
}


void interrupt INTERRUPT_InterruptManager (void)
{
    // interrupt handler
    if(INTCONbits.PEIE == 1 && PIE1bits.TXIE == 1 && PIR1bits.TXIF == 1)
    {
        //EUSART_Transmit_ISR();
        //TXREG = emargency_switch;
        Send_Till_EndSignal(TxData0, EUSART_Write_emargency, EUSART_TxInterrupt_Control_emargency, number_of_txdata0, 0);
        PIR1bits.TXIF = 0;
    }
    else if(INTCONbits.PEIE == 1 && PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1)
    {
        //EUSART_Receive_ISR();
    }
    else
    {
        //Unhandled Interrupt
    }
}
