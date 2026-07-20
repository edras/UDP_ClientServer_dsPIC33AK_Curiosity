/**
 * PINS Generated Driver Header File 
 * 
 * @file      pins.h
 *            
 * @defgroup  pinsdriver Pins Driver
 *            
 * @brief     The Pin Driver directs the operation and function of 
 *            the selected device pins using dsPIC MCUs.
 *
 * @skipline @version   PLIB Version 1.0.5
 *
 * @skipline  Device : dsPIC33AK512MPS506
*/

/*
© [2026] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#ifndef PINS_H
#define PINS_H
// Section: Includes
#include <xc.h>

/**
 * @ingroup  pinsdriver
 * @brief    Locks all the Peripheral Remapping registers and cannot be written.
 * @return   none  
 */
#define PINS_PPSLock()           (RPCONbits.IOLOCK = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Unlocks all the Peripheral Remapping registers and can be written.
 * @return   none  
 */
#define PINS_PPSUnlock()         (RPCONbits.IOLOCK = 0)

// Section: Device Pin Macros
/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA2 GPIO Pin which has a custom name of INT1 to High
 * @pre      The RA2 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define INT1_SetHigh()          (_LATA2 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA2 GPIO Pin which has a custom name of INT1 to Low
 * @pre      The RA2 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define INT1_SetLow()           (_LATA2 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RA2 GPIO Pin which has a custom name of INT1
 * @pre      The RA2 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define INT1_Toggle()           (_LATA2 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RA2 GPIO Pin which has a custom name of INT1
 * @param    none
 * @return   none  
 */
#define INT1_GetValue()         _RA2

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA2 GPIO Pin which has a custom name of INT1 as Input
 * @param    none
 * @return   none  
 */
#define INT1_SetDigitalInput()  (_TRISA2 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA2 GPIO Pin which has a custom name of INT1 as Output
 * @param    none
 * @return   none  
 */
#define INT1_SetDigitalOutput() (_TRISA2 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA10 GPIO Pin which has a custom name of CS1 to High
 * @pre      The RA10 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define CS1_SetHigh()          (_LATA10 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA10 GPIO Pin which has a custom name of CS1 to Low
 * @pre      The RA10 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define CS1_SetLow()           (_LATA10 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RA10 GPIO Pin which has a custom name of CS1
 * @pre      The RA10 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define CS1_Toggle()           (_LATA10 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RA10 GPIO Pin which has a custom name of CS1
 * @param    none
 * @return   none  
 */
#define CS1_GetValue()         _RA10

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA10 GPIO Pin which has a custom name of CS1 as Input
 * @param    none
 * @return   none  
 */
#define CS1_SetDigitalInput()  (_TRISA10 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA10 GPIO Pin which has a custom name of CS1 as Output
 * @param    none
 * @return   none  
 */
#define CS1_SetDigitalOutput() (_TRISA10 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB7 GPIO Pin which has a custom name of RST1 to High
 * @pre      The RB7 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define RST1_SetHigh()          (_LATB7 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB7 GPIO Pin which has a custom name of RST1 to Low
 * @pre      The RB7 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RST1_SetLow()           (_LATB7 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB7 GPIO Pin which has a custom name of RST1
 * @pre      The RB7 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define RST1_Toggle()           (_LATB7 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB7 GPIO Pin which has a custom name of RST1
 * @param    none
 * @return   none  
 */
#define RST1_GetValue()         _RB7

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB7 GPIO Pin which has a custom name of RST1 as Input
 * @param    none
 * @return   none  
 */
#define RST1_SetDigitalInput()  (_TRISB7 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB7 GPIO Pin which has a custom name of RST1 as Output
 * @param    none
 * @return   none  
 */
#define RST1_SetDigitalOutput() (_TRISB7 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC3 GPIO Pin which has a custom name of SW0 to High
 * @pre      The RC3 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define SW0_SetHigh()          (_LATC3 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RC3 GPIO Pin which has a custom name of SW0 to Low
 * @pre      The RC3 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SW0_SetLow()           (_LATC3 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RC3 GPIO Pin which has a custom name of SW0
 * @pre      The RC3 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define SW0_Toggle()           (_LATC3 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RC3 GPIO Pin which has a custom name of SW0
 * @param    none
 * @return   none  
 */
#define SW0_GetValue()         _RC3

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC3 GPIO Pin which has a custom name of SW0 as Input
 * @param    none
 * @return   none  
 */
#define SW0_SetDigitalInput()  (_TRISC3 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RC3 GPIO Pin which has a custom name of SW0 as Output
 * @param    none
 * @return   none  
 */
#define SW0_SetDigitalOutput() (_TRISC3 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD0 GPIO Pin which has a custom name of LED0 to High
 * @pre      The RD0 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED0_SetHigh()          (_LATD0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD0 GPIO Pin which has a custom name of LED0 to Low
 * @pre      The RD0 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED0_SetLow()           (_LATD0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD0 GPIO Pin which has a custom name of LED0
 * @pre      The RD0 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED0_Toggle()           (_LATD0 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD0 GPIO Pin which has a custom name of LED0
 * @param    none
 * @return   none  
 */
#define LED0_GetValue()         _RD0

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD0 GPIO Pin which has a custom name of LED0 as Input
 * @param    none
 * @return   none  
 */
#define LED0_SetDigitalInput()  (_TRISD0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD0 GPIO Pin which has a custom name of LED0 as Output
 * @param    none
 * @return   none  
 */
#define LED0_SetDigitalOutput() (_TRISD0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Initializes the PINS module
 * @param    none
 * @return   none  
 */
void PINS_Initialize(void);

/**
 * @ingroup  pinsdriver
 * @brief    This function is callback for INT1 Pin
 * @param    none
 * @return   none   
 */
void INT1_CallBack(void);


/**
 * @ingroup    pinsdriver
 * @brief      This function assigns a function pointer with a callback address
 * @param[in]  InterruptHandler - Address of the callback function 
 * @return     none  
 */
void INT1_SetInterruptHandler(void (* InterruptHandler)(void));


#endif
