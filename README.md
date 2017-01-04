/**
  @page I2C_EEPROM  How to use the I2C to drive an EEPROM memory
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 HoyenTech *******************
  * @file    I2C/I2C_EEPROM/readme.txt 
  * @author  MCD Application Team
  * @version V0.9.0
  * @date    4-Jan-2016
  * @brief   Description of the I2C and 24LC64 EEPROM communication example.
  ******************************************************************************

@par Example Description 

This firmware provides a basic example of how to use the I2C firmware library and
an associate I2C EEPROM driver to communicate with an I2C EEPROM device (here the
example is interfacing with MicroChip 24LC64 EEPROM)

I2C peripheral is configured in Master transmitter during write operation and in
Master receiver during read operation from I2C EEPROM. 

The peripheral used is I2C1 but can be configured by modifying the defines values
in stm320518_eval.h or stm32072b_eval.h file.
The maximum speed of communication with M24LR64 is 400kHz.
The speed of communication is configured by setting the value of sEE_I2C_TIMING define
in stm320518_eval_i2c_ee.h or stm32072b_eval_i2c_ee.h file.

For 24LC64 devices all the memory is accessible through the two-bytes 
addressing mode and need to define block addresses. In this case, the physical 
address has to be defined according to the address pins (E1, E2) connection.

First, the content of Tx1Buffer is written to the sEE_WRITE_ADDRESS1 and the
written data are read. The written and the read buffers data are then compared.
Following the read operation, the program waits that the EEPROM reverts to its 
Standby state. A second write operation is, then, performed and this time, Tx2Buffer
is written to sEE_WRITE_ADDRESS2, which represents the address just after the last 
written one in the first write. After completion of the second write operation, the 
written data are read. The contents of the written and the read buffers are compared.

All transfers are managed in Polling mode by calling sEE_ReadBuffer() or 
sEE_WriteBuffer() function.


@par Directory contents 

  - I2C/I2C_EEPROM/stm32f0xx_conf.h    Library Configuration file
  - I2C/I2C_EEPROM/stm32f0xx_it.c      Interrupt handlers
  - I2C/I2C_EEPROM/stm32f0xx_it.h      Interrupt handlers header file
  - I2C/I2C_EEPROM/main.c              Main program
  - I2C/I2C_EEPROM/main.h              Main program header file   
  - I2C/I2C_EEPROM/system_stm32f0xx.c  STM32F0xx system source file
  
@note The "system_stm32f0xx.c" is generated by an automatic clock configuration 
      tool and can be easily customized to meet user application requirements. 
      To select different clock setup, use the "STM32F0xx_Clock_Configuration_VX.Y.Z.xls" 
      provided with the AN4055 package available on <a href="http://www.st.com/internet/mcu/class/1734.jsp">  ST Microcontrollers </a>
         
@par Hardware and Software environment

  - This example runs on STM32F0xx devices.
  
  - This example has been tested with STMicroelectronics STM32F0 Discovery that including STM32F051R8T6 devices
    and can be easily tailored to any other supported device and development board



 * <h3><center>&copy; COPYRIGHT Hoyen Tech</center></h3>
 */
