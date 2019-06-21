#include "VirtualSerial.h"
#include "XMC4500.h"
#include "sodium.h"
#include "testkey.h"
#include "base64.h"
#include "stdbool.h"
//#include "randombytes_dilbert.h"


const int HEADER_TEXT_LENGTH=4;
const int HEADER_NONCE=32;
const int HEADER_TEXT_D_LENGTH=3;
const int HEADER_D_NONCE=24;

int16_t Bytes = 0;

typedef struct received_packet{
    
    size_t encoded_text_length;
    size_t decoded_text_length;
    unsigned char* encoded_text;
    unsigned char* decoded_text;
    unsigned char header_length_e[4];
    unsigned char header_nonce_e[32];
    unsigned char header_length_d[3];
    unsigned char header_nonce_d[24];

}received_packet;


typedef struct encrypted_packet{

    size_t decoded_cipher_text_length;
    size_t encoded_cipher_text_length;
    unsigned char cipher_header_length_decoded[3];
    unsigned char cipher_header_length_encoded[4];
    unsigned char* encoded_cipher_text;
    unsigned char* decoded_cipher_text;
	
}encrypted_packet;

/* Clock configuration */
XMC_SCU_CLOCK_CONFIG_t clock_config =
{
    .syspll_config.p_div = 2,
    .syspll_config.n_div = 80,
    .syspll_config.k_div = 4,
    .syspll_config.mode = XMC_SCU_CLOCK_SYSPLL_MODE_NORMAL,
    .syspll_config.clksrc = XMC_SCU_CLOCK_SYSPLLCLKSRC_OSCHP,
    .enable_oschp = true,
    .calibration_mode = XMC_SCU_CLOCK_FOFI_CALIBRATION_MODE_FACTORY,
    .fsys_clksrc = XMC_SCU_CLOCK_SYSCLKSRC_PLL,
    .fsys_clkdiv = 1,
    .fcpu_clkdiv = 1,
    .fccu_clkdiv = 1,
    .fperipheral_clkdiv = 1
};


void SystemCoreClockSetup(void)
{
    /* Setup settings for USB clock */
    XMC_SCU_CLOCK_Init(&clock_config);

    XMC_SCU_CLOCK_EnableUsbPll();
    XMC_SCU_CLOCK_StartUsbPll(2, 64);
    XMC_SCU_CLOCK_SetUsbClockDivider(4);
    XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
    XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_USB);

    SystemCoreClockUpdate();
}


bool write_text(unsigned char* buff, int len)
{
    for(int i=0;i<len;i++)
    {
        buff[i] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
        --Bytes;
        if(buff[i] == 0x01 || !isbase64(buff[i]))
        {
            return false;
        }   
    }    
    return true;
}

size_t uchar_to_int(unsigned char* h_d_length)
{
    size_t text_length;
    text_length = h_d_length[2] << 16 | h_d_length[1] << 8 | h_d_length[0];
    return text_length;
}



unsigned char key[32] = {0};
void get_chipid()
{
    for(int i = 0; i<16;i++)
    {
        key[i] = g_chipid[i];
        key[i+16] = g_chipid[i];
    }
}

/*

void Send_Packet(unsigned char* c_header, unsigned char* c_text, unsigned int t_len)
{
    
    CDC_Device_SendByte(&VirtualSerial_CDC_Interface, 0x01); //Header Flag
    for(int i=0; i<HEADER_TEXT_LENGTH; i++)
    {
        CDC_Device_SendByte(&VirtualSerial_CDC_Interface, c_header[i]);
    }
    CDC_Device_SendByte(&VirtualSerial_CDC_Interface, 0x02);
    for(int i=0; i<t_len; i++)
    {
        CDC_Device_SendByte(&VirtualSerial_CDC_Interface, c_text[i]);
    }
    CDC_Device_SendByte(&VirtualSerial_CDC_Interface, 0x03);
    
}

*/
void Clear_USB()
{
    for(int i = 0; i<64; i++)
    {
        CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    }    
}

void Complete_Encrypted_Packet(encrypted_packet* a_Package)
{
    size_t  tmp = a_Package->encoded_cipher_text_length;
    
    a_Package->cipher_header_length_decoded[2] = (tmp >> 16) & 0xFF;
    a_Package->cipher_header_length_decoded[1] = (tmp >>  8) & 0xFF;
    a_Package->cipher_header_length_decoded[0] = (tmp      ) & 0xFF;
    

    base64_encode(a_Package->cipher_header_length_decoded, 3 , a_Package->cipher_header_length_encoded, 4);
    
    
}

void Send_Package(encrypted_packet* a_package, unsigned char* buff)
{
    buff[0] = 0x01;
    for(int i = 1; i<5; i++)
    {
        buff[i] = a_package->cipher_header_length_encoded[i-1];
    }
    buff[5] = 0x02;
    for(int i = 6; i < a_package->encoded_cipher_text_length+6; i++)
    {
        buff[i] = a_package->encoded_cipher_text[i-6];
    }
    buff[a_package->encoded_cipher_text_length+6] = 0x03;
    Clear_USB();
    CDC_Device_SendData(&VirtualSerial_CDC_Interface, buff, a_package->encoded_cipher_text_length+7);
    
}
void Clear_Package(received_packet* aPackage){
    for(int i = 0; i < 32; i++) {
        if(i < 3) {
            aPackage->header_length_d[i] = 0x00;
            aPackage->header_length_e[i] = 0x00;
            aPackage->header_nonce_d[i] = 0x00;
            aPackage->header_nonce_e[i] = 0x00;
        }
        else if (i >= 3 && i < 4){
            aPackage->header_length_e[i] = 0x00;
            aPackage->header_nonce_d[i] = 0x00;
            aPackage->header_nonce_e[i] = 0x00;
        }
        else if (i >= 4 && i < 24){
            aPackage->header_nonce_d[i] = 0x00;
            aPackage->header_nonce_e[i] = 0x00;
        }
        else{
            aPackage->header_nonce_e[i] = 0x00;
        }
    }
    aPackage->encoded_text_length = 0;
    aPackage->decoded_text_length = 0;
}

void Clear_Crypto_Package(encrypted_packet* aPackage){
    for(int i = 0; i < 4; i++) {
        if(i < 3) {
            aPackage->cipher_header_length_decoded[i] = 0x00;
            aPackage->cipher_header_length_encoded[i] = 0x00;
        }
        else{
            aPackage->cipher_header_length_encoded[i] = 0x00;
        }
    }
    aPackage->decoded_cipher_text_length = 0;
    aPackage->encoded_cipher_text_length = 0;
}


int main(void)
{
 

    
    received_packet r_packet;
    encrypted_packet e_packet;
    
    USB_Init();
    /*
    randombytes_set_implementation(&randombytes_dilbert_implementation);
    
    if(sodium_init()<0)
    
    {
    }
    */
    get_chipid();
    
    while (1)
    {        
        // Check if data received 
        Bytes = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
        
        if(Bytes>0)
        {
            if(CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface)==0x01) // Check for the first Header Flag
            {
                ABORT:
                --Bytes;
                write_text(r_packet.header_length_e, HEADER_TEXT_LENGTH);
                if(!write_text(r_packet.header_nonce_e, HEADER_NONCE))
                {
                    goto ABORT;
                }
                    Bytes = Bytes-36;
                    if(CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface)==0x02) //Check for the start of text flag
                    {
                        --Bytes;
                        size_t tmp_d = HEADER_TEXT_D_LENGTH;
                        size_t tmp_n = HEADER_D_NONCE;
                        base64_decode(r_packet.header_length_e, HEADER_TEXT_LENGTH, r_packet.header_length_d, &tmp_d);
                        base64_decode(r_packet.header_nonce_e, HEADER_NONCE, r_packet.header_nonce_d, &tmp_n);
                        r_packet.encoded_text_length = uchar_to_int(r_packet.header_length_d);
                        if(r_packet.encoded_text_length>2048)
                        {
                            goto INVALID1;
                        }
                        r_packet.encoded_text = (unsigned char*)calloc(r_packet.encoded_text_length, sizeof(unsigned char));
                        for(int i = 0; i<r_packet.encoded_text_length ; i++) // Because of the last flag
                        {
                            if(Bytes<=0)
                            {   
                                Clear_USB();
                                Bytes = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
                            }
                            r_packet.encoded_text[i] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                            --Bytes;
                            if(!isbase64(r_packet.encoded_text[i])) 
                            {
                                goto INVALID;
                            }
                            
                        }
                        if(Bytes<=0)
                        {
                            Clear_USB();
                        }
                        if(CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface)==0x03)
                        {
                            
                            size_t* tmp_size = NULL;
                            size_t tmp = b64d_size(r_packet.encoded_text_length);
                            tmp_size = &tmp;
                            r_packet.decoded_text_length = *tmp_size;
                            r_packet.decoded_text = (unsigned char*)calloc(r_packet.decoded_text_length, sizeof(unsigned char));
                            
                            //unsigned char p_d_Text[t_d_size];
                            base64_decode(r_packet.encoded_text, r_packet.encoded_text_length, r_packet.decoded_text, tmp_size);
                            
                            e_packet.decoded_cipher_text_length = crypto_secretbox_MACBYTES + r_packet.decoded_text_length; //deneme yap
                            e_packet.encoded_cipher_text_length = b64e_size(e_packet.decoded_cipher_text_length);
                            e_packet.decoded_cipher_text = (unsigned char*)calloc(e_packet.decoded_cipher_text_length, sizeof(unsigned char*));                        
                            e_packet.encoded_cipher_text = (unsigned char*)calloc(e_packet.encoded_cipher_text_length, sizeof(unsigned char*));
                            
                            crypto_secretbox_easy(e_packet.decoded_cipher_text, r_packet.decoded_text, r_packet.decoded_text_length, r_packet.header_nonce_d, key);
                                                    
                            *tmp_size = e_packet.encoded_cipher_text_length;
                            base64_encode(e_packet.decoded_cipher_text, e_packet.decoded_cipher_text_length, e_packet.encoded_cipher_text, *tmp_size);
                            
                            Complete_Encrypted_Packet(&e_packet);
                            
                            unsigned char* send_buffer = (unsigned char*)calloc(3 + 4 + e_packet.encoded_cipher_text_length , sizeof(unsigned char));
                            
                            Send_Package(&e_packet, send_buffer);
                            
                            
                            /***** EXIT POLICY ******/
                            Clear_Package(&r_packet);
                            Clear_Crypto_Package(&e_packet);
                            
                            free(r_packet.encoded_text);
                            free(r_packet.decoded_text);
                            free(e_packet.encoded_cipher_text);
                            free(e_packet.decoded_cipher_text);
                            free(send_buffer); 
                        }
                        else
                        {
                            /***** EXIT POLICY ******/
                            free(r_packet.decoded_text);
                            INVALID:
                            free(r_packet.encoded_text);
                            INVALID1:                            
                            Clear_Package(&r_packet);
                        }
                        
                    }
                
                    else
                    {
                        /***** EXIT POLICY ******/
                        Clear_Package(&r_packet);
                    }
                

            
            }
            
        }
        Clear_USB();
    }  
    
}


    

/**
 * Main program entry point. This routine configures the hardware required by
 * the application, then enters a loop to run the application tasks in sequence.
 */