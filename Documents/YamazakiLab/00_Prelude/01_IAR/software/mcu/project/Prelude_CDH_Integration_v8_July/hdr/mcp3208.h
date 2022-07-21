/***************************************************************************************
 * @file     reb_timer.h
 * @version  V1.0
 * @date     31. March 2016
 *
 * @note
 * VORAGO Technologies 
 *
 * @note
 * Copyright (c) 2013-2016 VORAGO Technologies. 
 *
 * @par
 * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND BY 
 * ALL THE TERMS AND CONDITIONS OF THE VORAGO TECHNOLOGIES END USER LICENSE AGREEMENT. 
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. VORAGO TECHNOLOGIES 
 * SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ****************************************************************************************/
/**
 * @file mcp3208.h
 */


#ifndef MCP3208_H
#define MCP3208_H


/** Polarity setting for differential inputs.
 *
 * POL_EVEN_POSITIVE sets channel [0|2|4|6] as the positive side and channel
 * [1|3|5|7] as the negative side. POL_EVEN_NEGATIVE sets the opposite.
 */
enum Polarity {
    POL_EVEN_POSITIVE,
    POL_EVEN_NEGATIVE
};


/** Class for interfacing to the MCP3208 SPI-based ADC.
 *
 * This class will also allow interfacing to the MCP3204, but only four
 * inputs are provided by that chip, as opposed to the eight of the MCP3208.
 */
class MCP3208
{
public:
    /** Create an MCP3208 object.
     *
     * @param bus An SPI bus object.
     * @param cs The name of a pin to use as the chip select.
     */
    MCP3208(SPI bus, PinName cs);
    ~MCP3208();
    
    /** Read from a single-ended input.
     *
     * @param channel The channel number to read from.
     *
     * @param returns The sampled value as a float between 0.0 and 1.0.
     */
    float read_input(int channel);
    
    /** Read from a pair of differential inputs.
     *
     * In differential mode, the channels are referred to as 0 to 3, with
     * polarity set in a separate parameter. This avoids the user having to set
     * the polarity as part of the channel number or having channel numbers
     * increase by two (i.e. the channels being 0, 2, 4, and 6).
     *
     * @param channel The channel number to read from.
     * @param polarity The polarity of the differential signal.
     *
     * @param returns The sampled value as a float between 0.0 and 1.0.
     */
    float read_diff_input(int channel, Polarity polarity);
  
private:
    DigitalOut m_cs;
    SPI m_bus;
    
    void select();
    void deselect();
};


#endif  /* MCP3208_H */ 




