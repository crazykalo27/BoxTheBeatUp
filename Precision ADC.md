---
excalidraw-plugin: parsed
tags:
  - excalidraw
excalidraw-open-md: true
---

# [[Precision ADC]]
[[2025-02-16]]
[[17:43]]

This section of the [[msp432p4XX Technical Reference Manual.pdf#page=842&selection=14,0,14,13|msp432p4XX Technical Reference Manual]] is about the precision ADC on the MSP432P4111. It is a summary of the whole chapter and covers the most important parts while putting it into my own words.

#### ADC Core
The ADC core is configured by the control registers ADC14CTL0 and ADC14CTL1. The core is reset when ADC14ON = 0. When ADC14ON = 1, the reset is removed and the core is ready to power up once a valid conversion is triggered. The ADC control bits can be modified ==only when ADC14ENC = 0==. Also, ==ADC14ENC== must be set to 1 before any conversion can take place. All conversion results are always stored in **binary unsigned** format. There is, however, a data format bit, ADC14DF, that allows the user to read the conversion results as binary unsigned or 2s complement.

#### Voltage References

There are three selectable voltage levels of 1.2V, 1.45V, and 2.5V. To change these, we'll need to refer to the REF module chapter. The reference voltages can be used internally and externally on pin VREF+. 

#### Sample and Conversion Timing

An analog-to-digital measurement is initiated with a rising edge of the sample input signal SHI. The source for this signal can be the ADC14SC bit or a bunch of other sources like timer outputs.
The ADC14CLK is used both as the conversion clock. It's source can be connected to various clocks we've used before like MODCLK, SYSCLK, or SMCLK. The analog-to-digital conversion requires 16 ADC14CLK cycles for 14-bit conversion. 
###### Pulse-Sample Mode
The pulse sample mode is what the final project is using and it works by requesting the clock, waiting to sync with the selected clock, starting the sampling, stopping the sampling, starting the conversion, completing the conversion (over the course of 16 ADC14CLK cycles (specifically in our final project code)) and then storing the data. The other option is called *extended sample mode*. The difference between the two is that PSM has a configurable sample interval based on the ADC14SHT0x and ADC14SHT1x bits that control the period $t_{sample}$. This is in contrast to *extended sample mode* which has the SHI signal directly control this period.

#### Conversion Memory

There are 32 ADC14MEMx conversion memory registers to store the conversion results. Each is configured with its associated ADC14MCTLx register. ==When conversion results are written to a selected ADC14MEMx, the corresponding flag in the ADC14IFGRx register is set.== CSTARTADDx bits define the first ADC14MCTLx used for any conversion. If the conversion mode is single-channel or repeat-single-channel, the CSTARTADDx points to the single ADC14MCTLx to be used.

#### Precision ADC Conversion Modes

There are four operating modes selected by the CONSEQx bits.

1. Single-channel single-conversion  = A single channel is converted once. 
2. Sequence-of-channels (autoscan) = A sequence of channels is converted once. 
3. Repeat-single-channel = A single channel is converted repeatedly. 
4. Repeat-sequence-of-channels (repeated autoscan) = A sequence of channels is converted repeatedly. 

###### Single-channel single-conversion
When ADC14SC triggers a conversion, successive conversions can be triggered by the ADC14SC bit. When any other trigger source is used, ADC14ENC must be toggled between each conversion. For the final project, we are using the ADC14SC bit as our trigger source so we don't need to toggle ADC14ENC.

###### Sequence-of-channels
In this mode, a sequence of channels is sampled and converted once. This is the mode necessary for the final project. The ADC results are written to the conversion memories starting with the ADCMEMx defined by the CSTARTADDx bits. The sequence stops after the measurement of the channel with a set ADC14EOS bit. To be clear, the ADC14EOS bit is in the ADC14MCTLx register. 

This means that if you wanted to sample a sequence of four channels (e.g. A1-A4 on pins 5.4-5.1) then you could configure your ADC14MCTLx registers to have the ADC14EOS bit be set only on ADC14MCTL3 and cleared for ADC14MCTL0-2. This would make the sequence of channels be stored to ADC14MCTL0-3 and it would stop the sequence on the fourth memory.

###### Repeat-Modes
These aren't useful for the final project application. Note that it is necessary to read the result after the completed conversion because the same memory sequence is used on every conversion and those memories will be overwritten by the next conversion.

#### Precision ADC Interrupts

The Precision ADC has the following interrupt sources: 
- ADC14IFG0 to ADC14IFG31
- ADC14OV: ADC14MEMx overflow
- ADC14TOV: Precision ADC conversion time overflow
- ADC14LOIFG, ADC14INIFG, and ADC14HIIFG for ADC14MEMx
- ADC14RDYIFG: Precision ADC local buffered reference ready

The important one in this list is the ADC14IFGx interrupt. These are set when their corresponding ADC14MEMx memory register is loaded with a conversion result. ==An interrupt request is generated if the corresponding ADC14IEx bit is set and the interrupt registers in the Arm Cortex-M4 and NVIC are configured properly.== 

Another notable one is the ADC14OV which is set when a conversion result is written to any ADC14MEMx before its previous conversion result was read. In addition to this one, the ADC14TOV interrupt is the "Precision ADC conversion time overflow." If this flag is set, it means that ==a sample-and-conversion was requested before the current conversion was completed==.


### Final Project Comments

It appears we can get higher resolution on the ADC readings for the FSRs if we use a lower reference voltage closer to the values that we actually expect (0-0.8V). This can be done by changing ADC14VRSEL and changing the reference voltages that these will use accordingly. We should implement this if we feel like ==we need better resolution on our values==, however, we really only care about the maximum so this is likely not that important.

The clock we are currently using runs at 24MHz but we could get up to 48MHz using SMCLK. This also is only really important if ==we need better resolution on our values==. Once again we're not that interested in this right now.

###### Using the Multiple Sample and Convert (ADC14MSC) Bit

> To configure the converter to perform successive conversions automatically and as quickly as possible, a multiple sample and convert function is available - [[msp432p4XX Technical Reference Manual.pdf#page=855&selection=17,0,18,49|msp432p4XX Technical Reference Manual, page 855]]

This seems pretty nice because it would increase the speed of successive conversions. ==If we aren't sampling the FSRs fast enough, we should do this==. 

#### Glossary of Terms

| Term | Definition                                                          |
| ---- | ------------------------------------------------------------------- |
| SAR  | Successive-Approximation Register as seen in [[Lessons#Lesson 5-5]] |

















![[Precision ADC.svg]]


## Drawing
```compressed-json
N4IgLgngDgpiBcIYA8DGBDANgSwCYCd0B3EAGhADcZ8BnbAewDsEAmcm+gV31TkQAswYKDXgB6MQHNsYfpwBGAOlT0AtmIBeNCtlQbs6RmPry6uA4wC0KDDgLFLUTJ2lH8MTDHQ0YNMWHRJMRZFAHZFAFYyJE9VGEYwGgQAbQBdcnQoKABlALA+UEl8PGzsDT5GTkxMch0YIgAhdFQAayKuRlwAYXpMenwEEABiADMx8ZAAX0mgA
```
%%