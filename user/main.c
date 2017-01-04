/**
  ******************************************************************************
  * @file    I2C/I2C_EEPROM/main.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2C_EEPROM
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Uncomment the following line to enable using LCD screen for messages display */
#define sEE_SLAVE_WRITE_ADDRESS         0xA0
#define sEE_SLAVE_READ_ADDRESS         0xA1
#define sEE_WRITE_ADDRESS1        0x0002
#define sEE_READ_ADDRESS1         0x0002
#define BUFFER_SIZE1             25    //(countof(Tx1Buffer)-1)
#define BUFFER_SIZE2             25   //(countof(Tx2Buffer)-1)
#define sEE_WRITE_ADDRESS2       (sEE_WRITE_ADDRESS1 + BUFFER_SIZE1)
#define sEE_READ_ADDRESS2        (sEE_READ_ADDRESS1 + BUFFER_SIZE1)
#define sEE_MAX_TRIALS_NUMBER     300
#define sEE_I2C_TIMING           0x00210507
/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t Tx1Buffer[] = "The A0, A1, A2 inputs are used by the 24XX64 for multiple \
                        device operation. The levels on these inputs are \
                        compared with the corresponding bits in the slave \
                        address. ";

uint8_t Tx2Buffer[] = "ABCDEFGHIJKLMNOPQRSUVWXYZAAAAAABBBBBBB123456";

uint8_t Rx1Buffer[BUFFER_SIZE1], Rx2Buffer[BUFFER_SIZE2]; 
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile uint16_t NumDataRead = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
int sEE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
int sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);
uint32_t sEE_WaitEepromStandbyState(void);
uint32_t sEE_TIMEOUT_UserCallback(void);
void sEE_DeInit(void);
void sEE_Init(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */ 
  
  /* Initialize the I2C EEPROM driver ----------------------------------------*/
  sEE_Init(); 

  /* First write in the memory followed by a read of the written data --------*/
  /* Write on I2C EEPROM from sEE_WRITE_ADDRESS1 */
  sEE_WriteBuffer(Tx1Buffer, sEE_WRITE_ADDRESS1, BUFFER_SIZE1); 

  /* Wait for EEPROM standby state */
  sEE_WaitEepromStandbyState();  
  
  /* Set the Number of data to be read */
  NumDataRead = BUFFER_SIZE1;
  
  /* Read from I2C EEPROM from sEE_READ_ADDRESS1 */
  sEE_ReadBuffer(Rx1Buffer, sEE_READ_ADDRESS1, BUFFER_SIZE1); 
  
  /* Check if the data written to the memory is read correctly */
  TransferStatus1 = Buffercmp(Tx1Buffer, Rx1Buffer, BUFFER_SIZE1);

  /* Second write in the memory followed by a read of the written data -------*/
  /* Write on I2C EEPROM from sEE_WRITE_ADDRESS2 */
  sEE_WriteBuffer(Tx2Buffer, sEE_WRITE_ADDRESS2, BUFFER_SIZE2); 

  /* Wait for EEPROM standby state */
   sEE_WaitEepromStandbyState();  
  
  /* Set the Number of data to be read */
  NumDataRead = BUFFER_SIZE2;  
  
  /* Read from I2C EEPROM from sEE_READ_ADDRESS2 */
  sEE_ReadBuffer(Rx2Buffer, sEE_READ_ADDRESS2, NumDataRead);
  
  /* Check if the data written to the memory is read correctly */
  TransferStatus2 = Buffercmp(Tx2Buffer, Rx2Buffer, BUFFER_SIZE2);
  
  /* Free all used resources */
  sEE_DeInit();

  while (1)
  {
  }
}

/**
  * @brief  DeInitializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_DeInit(void)
{
  sEE_LowLevel_DeInit(); 
}

/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_Init(void)
{ 
  I2C_InitTypeDef  I2C_InitStructure;
  
  sEE_LowLevel_Init();
  
  /* I2C configuration */
  /* sEE_I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x00;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_Timing = sEE_I2C_TIMING;
  
  /* Apply sEE_I2C configuration after enabling it */
  I2C_Init(sEE_I2C, &I2C_InitStructure);
   
  /* sEE_I2C Peripheral Enable */
  I2C_Cmd(sEE_I2C, ENABLE);    
}

int sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
  uint32_t I2C_TimeOut = 0x3000;
   /* Test on BUSY Flag */
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_BUSY) != RESET)
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(sEE_I2C, sEE_SLAVE_READ_ADDRESS, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  
  /* Wait until TXIS flag is set */
  I2C_TimeOut = 0x3000;
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_TXIS) == RESET)
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  /* Send Register address */

  I2C_SendData(sEE_I2C, (uint8_t) (ReadAddr >> 8));
  /* Wait until TXIS flag is set */
  I2C_TimeOut = 0x3000;
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_TXIS) == RESET)
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  I2C_SendData(sEE_I2C, (uint8_t) (ReadAddr & 0xff));

  /* Wait until TC flag is set */
  I2C_TimeOut = 0x3000;
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_TC) == RESET)
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }  
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(sEE_I2C, sEE_SLAVE_READ_ADDRESS, NumByteToRead, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  
  /* Wait until all data are received */
  while (NumByteToRead)
  {   
    /* Wait until RXNE flag is set */
    I2C_TimeOut = 0x3000;
    while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_RXNE) == RESET)    
    {
      if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
    }
    
    /* Read data from RXDR */
    *pBuffer = I2C_ReceiveData(sEE_I2C);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
    
    /* Decrement the read bytes counter */
    NumByteToRead--;
  } 
  
  /* Wait until STOPF flag is set */
  I2C_TimeOut = 0x3000;
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_STOPF) == RESET)   
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /* Clear STOPF flag */
  I2C_ClearFlag(sEE_I2C, I2C_ICR_STOPCF);
  
  /* If all operations OK */
  return 0;  
}

int sEE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
  uint16_t wLength = 0;
  uint32_t I2C_TimeOut = 0x3000;
  /* Test on BUSY Flag */

  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_BUSY) != RESET)
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(sEE_I2C, sEE_SLAVE_WRITE_ADDRESS, 2, I2C_Reload_Mode, I2C_Generate_Start_Write);
  
  /* Wait until TXIS flag is set */
  I2C_TimeOut = 0x3000;  
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_TXIS) == RESET)   
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /* Send Register address */
//  if(( RegAddr >> 8) > 0)
  I2C_SendData(sEE_I2C, (uint8_t) (WriteAddr >> 8));
   /* Wait until TXIS flag is set */
  I2C_TimeOut = 0x3000;  
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_TXIS) == RESET)   
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  I2C_SendData(sEE_I2C, (uint8_t) (WriteAddr & 0xff));
  
  /* Wait until TCR flag is set */
  I2C_TimeOut = 0x3000;
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_TCR) == RESET)
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(sEE_I2C, sEE_SLAVE_WRITE_ADDRESS, NumByteToWrite, I2C_AutoEnd_Mode, 0);
       
  /* Wait until TXIS flag is set */
  I2C_TimeOut = 0x3000;
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_TXIS) == RESET)
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }  

  for(wLength=0; wLength<NumByteToWrite; wLength++)
  {
	/* Write data to TXDR */
	I2C_SendData(sEE_I2C, pBuffer[wLength]);
	
	/* Wait until TXE flag is set */
	I2C_TimeOut = 0x3000;
  	while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_TXE) == RESET)
  	{
    	if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  	}  
  }	
 	  
  /* Wait until STOPF flag is set */
  I2C_TimeOut = 0x3000;
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_STOPF) == RESET)
  {
    if((I2C_TimeOut--) == 0) return sEE_TIMEOUT_UserCallback();
  }   
  
  /* Clear STOPF flag */
  I2C_ClearFlag(sEE_I2C, I2C_ICR_STOPCF);
  
  return 0;
//  return I2C_Write(sEE_SLAVE_WRITE_ADDRESS, WriteAddr,  pBuffer, NumByteToWrite);
}

/**
  * @brief  Wait for EEPROM Standby state.
  * 
  * @note  This function allows to wait and check that EEPROM has finished the 
  *        last operation. It is mostly used after Write operation: after receiving
  *        the buffer to be written, the EEPROM may need additional time to actually
  *        perform the write operation. During this time, it doesn't answer to
  *        I2C packets addressed to it. Once the write operation is complete
  *        the EEPROM responds to its address.
  * 
  * @param  None
  *
  * @retval sEE_OK (0) if operation is correctly performed, else return value 
  *         different from sEE_OK (0) or the timeout user callback.
  */
uint32_t sEE_WaitEepromStandbyState(void)      
{
  __IO uint32_t sEETrials = 0;
  __IO uint32_t I2C_TimeOut = 0x1000;
  /* Keep looping till the slave acknowledge his address or maximum number 
  of trials is reached (this number is defined by sEE_MAX_TRIALS_NUMBER define
  in stm32373c_eval_i2c_ee.h file) */
  
  /* Configure CR2 register : set Slave Address and end mode */
  I2C_TransferHandling(sEE_I2C, sEE_SLAVE_WRITE_ADDRESS, 0, I2C_AutoEnd_Mode, I2C_No_StartStop);  
  
  do
  { 
    /* Initialize sEETimeout */
    I2C_TimeOut = 0x1000;
    
    /* Clear NACKF */
    I2C_ClearFlag(sEE_I2C, I2C_ICR_NACKCF | I2C_ICR_STOPCF);
    
    /* Generate start */
    I2C_GenerateSTART(sEE_I2C, ENABLE);
    
    /* Wait until timeout elapsed */
    while (I2C_TimeOut-- != 0); 
    
    /* Check if the maximum allowed number of trials has bee reached */
    if (sEETrials++ == sEE_MAX_TRIALS_NUMBER)
    {
      /* If the maximum number of trials has been reached, exit the function */
      return sEE_TIMEOUT_UserCallback();
    }
  }
  while(I2C_GetFlagStatus(sEE_I2C, I2C_ISR_NACKF) != RESET);
  
  /* Clear STOPF */
  I2C_ClearFlag(sEE_I2C, I2C_ICR_STOPCF);
  
  /* Return sEE_OK if device is ready */
  return 0;
}

#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Example of timeout situation management.
  * @param  None.
  * @retval None.
  */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* Use application may try to recover the communication by resetting I2C
    peripheral (calling the function I2C_SoftwareResetCmd()) then re-start
    the transmission/reception from a previously stored recover point.
    For simplicity reasons, this example only shows a basic way for errors 
    managements which consists of stopping all the process and requiring system
    reset. */
  sEE_DeInit();
  sEE_Init(); 

  /* Block communication and all processes */
  while (1)
  {   
  }  
}
#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;  
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
