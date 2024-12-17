add_executable(${ProjectName}
    main.c
    onewire.c
    buzzer.c
    Servo.c
    keypad.c
    GPS.c
    utils.c
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    FreeRTOS-Kernel-Heap4 
    hardware_pwm    )

pico_add_extra_outputs(${ProjectName})
pico_enable_stdio_usb(${ProjectName} 1)
pico_enable_stdio_uart(${ProjectName} 0)
