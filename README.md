# URS-FILIP-ADS7828

## A/D konverter ads7828 i adc-joystick

#### Opis zadatka

U zadatku je bilo potrebno integrisati a/d konvertor ads7828, koji komunicara pomoću I2C protokola i adc-joystick, tako da se sa dzojstikom mogu vršiti određene akcije i da bude integrisan kao input device.

#### Implementacija

Da bi se uopste započela izrada zadatka, bilo je potrebno pravilno povezati komponente sa De1-SoC pločom. Povezivanje je izvršeno na sledeći način
1. Pin za napajanje na konvertoru i uzemljenje, su spojeni na pinove na ploči, koji predstavljaju uzemljenje i napajanje (vidjeti specifikaciju ploče)
2. Isto ovo ponoviti i za dzojstik
3. SCL i SDA pinove konvertera, spojiti na eksportovane pinove I2C2 magistrale
4. X i Y ose dzojstika spojiti na kanale 0 i 1 na A/D konvertoru

Nakon uspjšnog spajanja, bilo je potrebno obezbjediti I2C2 magistralu u dts fajlu. 
Pozicionirati se u sledeći folder

**linux-socfpga -> arch -> arm -> boot -> dts**
U ovom folderu u nekom od dostupnih tekst editora otvoriti fajl **socfpga_cyclone5_de1_soc.dts** i napraviti sledeće izmjene

```dts
/ &i2c0 {
	status = "okay";
	clock-frequency = <100000>;

	accel1: accelerometer@53 {
		compatible = "adi,adxl345";
		reg = <0x53>;

		interrupt-parent = <&portc>;
		interrupts = <3 2>;
	};
};

&i2c1 {
    status = "okay";
    clock-frequency = <100000>;
};

&i2c2 {
	status = "okay";
}
```

Ovakvim izledom dts fajla smo omogućili I2C2 magistralu.
Da bi sve funkcionisalo, potrebno je prekompajlirati ovaj dts fajl komandom
```
make dtbs
```

I dobijeni .dtb fajl, koji se nalazi u istom folderu kao i nas .dts fajl prekopirati na FAT32 particiju kartice.

Ako bi ubacili karticu u slot na ploči i pokrenuli komandu 
```
i2cdetect -ry 2
```

Vidijeli bi da je na adresi 53 detektovan naš A/D konvertor.

> **Note**: Obavezno dodati i2c1 čvor, jer u slučaju da njega ne dodamo, I2C2 magistrala neće biti dostupna.

Nakon modifikacije dts fajla, potrebno je uključiti drajvere za naše komponente

Pozicionirati se u folder linux-socfpga i pokrenuti komandu

```
make menuconfig
```

Ukljuciti sledecu opciju **Device drivers -> Hardware Monitoring support -> Texas Instruments ADS7828 and compatible**

Ovim smo uključili drajver za A/D konvertor, još nam preostaje da uključimo drajver za dzojstik.

**Device drivers -> Input device support -> Joysticks/Gampades -> Simple joystick connected over ADC**

Sačuvamo izvršene izmjene i pokrenemo komandu

```
make
```

Sledeće što je potrebno uraditi jeste sačuvati trenutnu konfiguraciju kao predefinisanu za našu ploču, u tu svrhu ćemo izvršiti sledeće komande

```
make savedefconfig
mv defconfig arch/arm/configs/de1_soc_defconfig
```

Sledeći korak je da prekopiramo relevantne fajlove u naš buildroot.
```
cp <path-to-linux-src>/arch/arm/configs/de1_soc_defconfig board/terasic/de1soc_cyclone5/
cp <path-to-linux-src>/arch/arm/boot/dts/socfpga_cyclone5_de1_soc.dts board/terasic/de1soc_cyclone5/
```

Nakon ovog kopiranje potrebno je izvršiti komandu

```
make
```

Sada je potrebno da opišemo naš uređaj u i2c2 čvoru našeg .dts fajla. Modifikovati i2c2 čvor tako da ima sledeći izgled

```dts
&i2c2 {
	status = "okay";
 
	adc: adc@48 {
            compatible = "ti,ads7828";
            reg = <0x48>;
            ti,differential-input;
			vref-supply = <&regulator_3_3v>;
			ti,enable-channels = <0 1>;	
			#io-channel-cells = <1>;

			joystick: adc-joystick {
					compatible = "adc-joystick";
					io-channels = <&adc 0>,
								<&adc 1>;
					#address-cells = <1>;
					#size-cells = <0>;

					axis@0 {
							reg = <0>;
							linux,code = <ABS_X>;
							abs-range = <0 3300>;
							abs-fuzz = <4>;
							abs-flat = <200>;
					};
					axis@1 {
							reg = <1>;
							linux,code = <ABS_Y>;
							abs-range = <0 3300>;
							abs-fuzz = <4>;
							abs-flat = <200>;
					};
            		};
		};
};
```


