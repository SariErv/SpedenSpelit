# Speden Spelit
Oulun Ammattikorkeakoulun TVT24KMO-luokka. Ryhmän 32 tietotekniikan sovellusprojekti, jossa toteutamme Speden Spelit-pelin. Käytämme pelin toteutuksessa Arduino Uno-mikrokontrolleria sekä elektroniikan komponentteja.

# Projektin esittely

Projekti hyödyntää Arduino Uno -mikrokontrolleria, painikkeita ja LED-valoja, jotka yhdessä muodostavat pelijärjestelmän. Pelissä on neljä painiketta, joita käytetään pelaamisen aikana, ja viides painike, jolla peli käynnistetään. Pelaajien on tarkoitus painaa oikeaa painiketta nopeasti tiettyjen LED-indikaatioiden mukaan. Alla kuvattu pelin kulku vuokaaviona.

![image](https://github.com/user-attachments/assets/e7a67299-080e-41c5-8c8d-a0dfac94a6ee)

# Projektissa käytettävät komponentit
- 	Arduino Uno
- 	2x 7-segmenttinäyttöä
- 	2x 74HC595 serial-to-parallel muuntimia
- 	4x lediä
- 	4x painiketta

Painikkeet ja LED-valot on kytketty seuraaviin Arduino-pinneihin:

LEDit: Liitettynä A1, A2, A3, ja A4
Painikkeet: Liitettynä D10, D11, D12, ja D13

# Kytkentä
![image](https://github.com/user-attachments/assets/87d2d699-db9b-42f2-a33c-7433e69b6a8c)

# Koodi

Arduino-koodissa on hyödynnetty keskeytyksiä, painikkeiden debounce-logiikkaa, ja LED-valojen ohjausta, jotta pelistä saadaan mahdollisimman sulava ja käyttäjäystävällinen.


# Tekijät
![Profiilikuva](https://github.com/Janitamaaaria)
![Profiilikuva](https://github.com/mikkohaapea)
![Profiilikuva](https://github.com/jennysara)
![Profiilikuva](https://github.com/SariErv)
