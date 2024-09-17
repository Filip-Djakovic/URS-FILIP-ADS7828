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

> **Note**: Please ensure you have the correct permissions before proceeding.
