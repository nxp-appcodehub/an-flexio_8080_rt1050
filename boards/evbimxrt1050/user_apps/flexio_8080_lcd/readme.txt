~~~~~~~~
This example demonstrate how to use FlexIO to simulate 8080 interface, and drive
an 8080 interface LCD panel. The example makes use a LCD panel with the  HX8357 
driver device.

~~~~~~~~
Use wires to connect the pins between RT1050-EVB and the LCD panel, as shown
below. And check the RT1050-EVB, to make sure the signals are through on the 
board.

  ------------------------------------
    LCD      MCU         EVB
  ------------------------------------
    D0    FLEXIO_12     J60-10
    D1    FLEXIO_13     J60-8
    D2    FLEXIO_14     J60-6
    D3    FLEXIO_15     J60-4
    D4    FLEXIO_16     J74-3
    D5    FLEXIO_17     J74-5
    D6    FLEXIO_18     J74-7
    D7    FLEXIO_19     J74-9
    D8    FLEXIO_20     J74-11
    D9    FLEXIO_21     J74-13
    D10   FLEXIO_22     J74-15
    D11   FLEXIO_23     J74-17
    D12   FLEXIO_24     J74-18
    D13   FLEXIO_25     J74-16
    D14   FLEXIO_26     J74-14
    D15   FLEXIO_27     J74-12
    
    WR    FLEXIO_00     J60-3
    RD    FLEXIO_01     J60-5
    
    RS    GPIO2_02      J60-7
    CS    GPIO2_03      J60-9

    3V3     3V3         J60-1
    GND     GND         J60-2
  ------------------------------------