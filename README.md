# URS-FILIP-ADS7828

#### A/D konverter ads7828 i adc-joystick

```dts
/ {
    model = "My Custom Board";
    compatible = "mycompany,myboard";

    memory@80000000 {
        device_type = "memory";
        reg = <0x80000000 0x2000000>;
    };

    leds {
        compatible = "gpio-leds";

        led0: led_0 {
            label = "Green LED";
            gpios = <&gpio1 17 GPIO_ACTIVE_HIGH>;
        };
    };
};



Ovo je nastavak
