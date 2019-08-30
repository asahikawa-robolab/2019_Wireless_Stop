#include "Controller_Protocol.h"

#define num OP[bus_number].num
#define count OP[bus_number].count
#define Start_signal_has_come StP[bus_number].Start_signal_has_come
#define S_flag StP[bus_number].S_flag
#define Buffer_count StP[bus_number].Buffer_count
#define data_send_count SeP[bus_number].data_send_count
#define byte_data_complete SeP[bus_number].byte_data_complete
#define last_signal SeP[bus_number].last_signal

#define enable 1
#define disable 0

unsigned char not_of_before_data;
Organize_Parameter OP[rx_number];
Store_Parameter StP[rx_number];
Send_Parameter SeP[tx_number];

void Organize_Datas(Processed *RxData, Untreated *Buffer, int number_of_data, int bus_number) {
    count = 1;
    for (num = 0; num < number_of_data; num++) {
        RxData[num].upper_data = Buffer[count].upper_data;
        RxData[num].lower_data = Buffer[count + 1].upper_data;
        if ((Buffer[count].upper_data | Buffer[count + 1].lower_data) == 15)RxData[num].UD_error = 0;
        else RxData[num].UD_error = 1;
        if ((Buffer[count + 1].upper_data | Buffer[count + 2].lower_data) == 15)RxData[num].LD_error = 0;
        else RxData[num].LD_error = 1;
        count += 2;
    }
}

bool Store_Datas(Untreated *Buffer, unsigned char received_data, int number_of_receive_data, int bus_number, bool(*EUSART_ERROR)(void)) {
    if (EUSART_ERROR() == error) {
        S_flag = 0;
        Buffer_count = 0;
        Start_signal_has_come = 0;
        return error;
    } else {

        if (Start_signal_has_come == 0) {
            Buffer[S_flag].all_data = received_data;
            if (S_flag == 1) {
                if (Buffer[1].lower_data == 5)Start_signal_has_come = 1;
                S_flag = 0;
            }
            else if (Buffer[0].all_data == 'S')S_flag = 1;
        } else {
            Buffer_count++;
            Buffer[Buffer_count].all_data = received_data;
            if (Buffer[Buffer_count].all_data == 'C') {
                if (Buffer[Buffer_count - 1].upper_data == 3) {
                    Buffer_count = 1;
                    Start_signal_has_come = 0;
                }
            }
            if (Buffer_count == number_of_receive_data - 1) {
                Buffer_count = 1;
                Start_signal_has_come = 0;
            }
        }
        return not_error;
    }
}

void Send_StartSignal(void (*transmission)(unsigned char), void (*txinterrupt_control)(bool), bool TXIEbit) {
    if (!TXIEbit) {
        transmission('S');
        txinterrupt_control(enable);
        not_of_before_data = 0x05;
    }
}

bool Send_Till_EndSignal(unsigned char *TxData, void (*transmission)(unsigned char), void (*txinterrupt_control)(bool), int number_of_txdata, int bus_number) {
    if (data_send_count != number_of_txdata) {
        if(data_send_count == 0)not_of_before_data = 0x05;
        if (byte_data_complete == 1) {
            transmission((TxData[data_send_count] & 0xf0) | (not_of_before_data & 0x0f));
            byte_data_complete = 0;
        } else {
            not_of_before_data = ~TxData[data_send_count];
            transmission((TxData[data_send_count] << 4) | (not_of_before_data >> 4));
            byte_data_complete = 1;
            data_send_count++;
        }
    } else {
        if (last_signal == 0) {
            transmission(0x30 | (not_of_before_data & 0x0f));
            last_signal++;
        } else if (last_signal == 1) {
            transmission('C');
            last_signal++;
        } else {
            last_signal = 0;
            data_send_count = 0;
            txinterrupt_control(disable);
            return 1;
        }
    }
    return 0;
}

#undef count
#undef Buffer_count
#undef byte_data_complete

extern void Initialize_Parameters(void) {
    static int i;

    for (i = 0; i < rx_number; i++)OP[i].count = 1;
    for (i = 0; i < tx_number; i++) {
        StP[i].Buffer_count = 1;
        SeP[i].byte_data_complete = 1;
    }
}

#undef num
#undef Start_signal_has_come
#undef S_flag
#undef data_send_count
#undef last_signal

#undef enable
#undef disable