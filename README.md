# Mano maišos funkcija „HashFun“: veikimo principai ir analizė

## Funkcijos paaiškinimas pseudokodu

**HashFun funkcija paima įvesties tekstą ir maišo jo baitus su aštuonių 64 bitų vidinių kintamųjų masyvu, atlikdama daugkartinius bitų poslinkius, XOR bei aritmetines operacijas. Po dviejų maišymo etapų ji sujungia gautas tarpines reikšmes į keturis 64 bitų blokus ir grąžina 256 bitų (64 simbolių šešioliktainį) maišos rezultatą.**

- Pradinė aštuonių 64 bitų registrų inicializacija su konstantomis:

```cpp
FUNCTION HashFun(input_string):
    seed[0..7] ← {0x5FAF3C1BULL, 0x6E8D3B27ULL, 0xA1C5E97FULL, 0x4B7D2E95ULL, 0xF2A39C68ULL, 0x3E9B5A7CULL, 0x9D74C5A1ULL, 0x7C1A5F3EULL}
```

- Pagrindinis maišymas su kiekvienu įvesties simboliu:

```cpp
    FOR i FROM 0 TO input_string.length − 1:
        c ← byte value of input_string[i]
        ind ← i MOD 8
        seed[ind] ← seed[ind] XOR ( (seed[(ind + 1) MOD 8] << 7) OR (seed[(ind + 7) MOD 8] >> 3) )
        seed[ind] ← seed[ind] + (cByte * 131) + ( seed[(ind + 3) MOD 8] XOR seed[(ind + 5) MOD 8] )
    END FOR
```

- Papildomi 64 maišymo raundai be naujų duomenų:

```cpp
    FOR i FROM 0 TO 63:
        ind ← i MOD 8
        seed[ind] ← seed[ind] XOR ( (seed[(ind + 1) MOD 8] << (i * 7) MOD 61) OR (seed[(ind + 7) MOD 8] >> (i * 5) MOD 53) )
        seed[ind] ← seed[ind] + ( seed[(ind + 3) MOD 8] XOR seed[(ind + 5) MOD 8] ) + (0x9E3779B97F4A7C15ULL XOR (i * 0xA1C52E95ULL))
    END FOR
```

- Išvesties suspaudimas į keturis galutinius 64 bitų žodžius:

```cpp
    FOR i FROM 0 TO 3:
        out4[i] ← seed[i] XOR (seed[i + 4] << 1) XOR (seed[(i + 2) MOD 8] >> 1)
    END FOR
```

- Sujungimas į vieną 256 bitų (64 hex) eilutę:

```cpp
    hex_out ← format_as_16hex(out4[0]) || format_as_16hex(out4[1]) || format_as_16hex(out4[2]) || format_as_16hex(out4[3])
    RETURN hex_out
END FUNCTION
```

## Eksperimentinis tyrimas

Šiame projekte tiriama mano sukurta **HashFun**. Tikslas – įvertinti jos savybes pagal užduoties reikalavimus ir palyginti jas su standartine **SHA-256** funkcija.

---

## 1. Testinių failų paruošimas

Eksperimentams sukuriau kelis failus su skirtingu turiniu:

| Failo pavadinimas | Turinys                                                       |
|-------------------|---------------------------------------------------------------|
| `blank.txt`       | Tuščias failas                                                |
| `a.txt`           | Viena mažoji raidė a                                          |
| `b.txt`           | Viena mažoji raidė b                                          |
| `rnd100000.txt`   | 100 000 atsitiktinių simbolių (raidės, skaičiai, ženklai)     |
| `rnd1000000.txt`  | 1 000 000 atsitiktinių simbolių                               |
| `rndF100000.txt`  | 100 000 atsitiktinių simbolių                                 |
| `rndS100000.txt`  | Kaip rndF100000.txt, bet pakeistas 75 000-asis simbolis       |
| `10mb.txt`        | Didžiausias failas su daugiau nei 10 000 000 simbolių         |

Šie failai naudoti tiek su HashFun, tiek su SHA-256, kad būtų galima tiesiogiai palyginti rezultatus.

## 2. Išvedimo dydis

- **HashFun:** visada sugeneruoja 64 simbolių (256 bitų) heksadecimalinę eilutę.  
- **SHA-256:** taip pat visada grąžina 64 simbolių (256 bitų) heksadecimalinę eilutę.

| Failas          | HashFun rezultatas                                              | SHA-256 rezultatas                                                       |
|-----------------|-----------------------------------------------------------------|-----------------------------------------------------------------|
| `blank.txt`     |e1e10ec33852dafdad0a90ef0edf56b9f7c1425813bc42d0a67386e5c1269b78 | e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 |
| `a.txt`         |13fbc56937664ae5ce4503508cf94ba6b82eaa19b9bc3c5bb8a7950b485df478 |ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb|
| `b.txt`         |0cce237961797622b9053b0d5e7c7c56848bcec8da55ef82c46496b265eee872 |3e23e8160039594a33894f6564e1b1348bbd7a0088d42c4acb73eeaed59c009d|
| `rnd100000.txt` |bf6fb7e57c24cee30ce7d062523997b2dcef0b49b1218c91dea9fca4751126f2 |c253e0ba462db43db95286964de626fcf4e931f4f5c0434e840d57598b0fc7df|
| `rnd1000000.txt`|afe953bc81c149bb814f8da11f69edd37b52b149d43643c1d85c4b5974b9c521 |19be1f718f2145ce5e23690366f811db2793cb7c3214a76bc9c076c71247ef14|

Tai įrodo, kad abi funkcijos išlaiko pastovų išvedimo ilgį nepriklausomai nuo įvesties.

## 3. Deterministiškumas

Abi funkcijos yra deterministinės – ta pati įvestis visada duoda tą patį išvedimą.
Pavyzdys su `a.txt`, abu kartus gautas identiškas rezultatas:

- **HashFun:**

13fbc56937664ae5ce4503508cf94ba6b82eaa19b9bc3c5bb8a7950b485df478
13fbc56937664ae5ce4503508cf94ba6b82eaa19b9bc3c5bb8a7950b485df478

- **SHA-256:**

ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb
ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb

Tas pats galioja visiems kitiems failams (`blank.txt`, `b.txt`, `rnd100000.txt`, `rnd1000000.txt`, `rndF100000.txt`, `rndS100000.txt`).

## 4. Efektyvumas

Matavau hashavimo funkcijų veikimo laiką naudojant `konstitucija.txt` failą. Matavau tik pačios hashavimo funkcijos vykdymo laiką (be failo skaitymo) ir kiekvieną bandymą paleidau kelis kartus bei užrašiau vidutinę trukmę mikrosekundėmis.

| Įvedimo dydis (eilučių sk.) | HashFun vidutinis veikimo laikas (µs)| SHA-256 vidutinis veikimo laikas (µs)|
|-----------------------------|--------------------------------------|--------------------------------------|
| 1 eilutė                    | 17 µs                                | 2 800 µs                             |
| 10 eilučių                  | 18 µs                                | 2 670 µs                             |
| 100 eilučių                 | 40 µs                                | 2 800 µs                             |
| 789 eilučių (pilnas failas) | 270 µs                               | 2 850 µs                             |

Papildomas testas su 10 MB dydžio `10mb.txt`:

| Įvedimo dydis (eilučių sk.)                 | HashFun vidutinis veikimo laikas (µs)| SHA-256 vidutinis veikimo laikas (µs)|
|---------------------------------------------|--------------------------------------|--------------------------------------|
| apie 100 000 eilučių                        | 13 900  µs                           | 5 500 µs                             |
| apie 270 000 eilučių (pilnas failas)        | 36 000 µs                            | 8 000 µs                             |

Rezultatai rodo, kad mažiems failams HashFun yra daug greitesnė už SHA-256. Tačiau su dideliais failais SHA-256 tampa efektyvesnė, nes HashFun veikimo laikas sparčiai auga.

Žemiau rezultatai pateikti naudojant grafiką:
![HashFun veikimo laikas](grafikas.png)

## 5. Kolizijų paieška

Šiame etape tikrinau, ar mano sukurta HashFun ir standartinė SHA-256 funkcija sukuria skirtingus hash’us atsitiktinėms įvestims.

Iš pradžių papildžiau programos kodą, pridėdamas naujas funkcijas, kurios generuoja po 100 000 atsitiktinių string porų, kiekvieną porą hash'uoja ir tikrina ar nebuvo identiškų hash'ų.

Poros generuotos keturių skirtingų ilgių: 10, 100, 500 ir 1000 simbolių, kiekvienam ilgiui atlikau po 3 bandymus su HashFun ir SHA-256, neaptikta ne vienos kolizijos:

- **HashFun**

| Ilgis (simbolių) | Bandymas 1 | Bandymas 2 | Bandymas 3 | Vidurkis |
|------------------|-----------:|-----------:|-----------:|---------:|
| 10               | 0          | 0          | 0          | **0**    |
| 100              | 0          | 0          | 0          | **0**    |
| 500              | 0          | 0          | 0          | **0**    |
| 1000             | 0          | 0          | 0          | **0**    |

- **SHA-256**

| Ilgis (simbolių) | Bandymas 1 | Bandymas 2 | Bandymas 3 | Vidurkis |
|------------------|-----------:|-----------:|-----------:|---------:|
| 10               | 0          | 0          | 0          | **0**    |
| 100              | 0          | 0          | 0          | **0**    |
| 500              | 0          | 0          | 0          | **0**    |
| 1000             | 0          | 0          | 0          | **0**    |

Taigi, HashFun bandymuose neaptikta jokių kolizijų (nors teoriškai dideliuose duomenų kiekiuose jos bus aptiktos) ir SHA-256 taip pat neaptikta jokių kolizijų, kaip ir tikėtasi iš kriptografiškai saugios funkcijos, nes niekas pasaulyje kol kas nerado kolizijų SHA-256 algoritme.

## 6. Lavinos efektas

Lavinos efektas (avalanche effect) – tai testas, kuris parodo, kiek radikaliai pasikeičia rezultatas, jei įvestyje pakeičiame vieną simbolį.

Šiam testui sukuriau papildomą funkciją, kuri sugeneruoja 100 000 atsitiktinių porų, kurių kiekviena skiriasi tik vienu simboliu, hash'ina kiekvieną ir skaičiuoja skirtumus bitų bei hex simbolių lygmeniu.
Eksperimentas buvo atliktas su 100 simbolių ilgio poromis, kiekvienoje poroje skyrėsi tik vienas simbolis.

| Metrika   | HashFun Bitai (%) | SHA-256 Bitai (%) | HashFun Hex’ai (%)| SHA-256 Hex’ai (%)|
|-----------|-------------------|-------------------|-------------------|-------------------|
| Min       | 46.09 – 46.88     | 46.87 - 78.82     | 76.56 – 78.12     | 75 - 76.56        |
| Max       | 83.98 – 84.38     | 83.98 - 85.15     | 100               | 100               |
| Vidurkis  | 66.02             | 66.03             | 93.75             | 93.74             |

Pagal teoriją ir viešai prieinamus šaltinius, idealiu atveju vidutinis bitų pasikeitimas turėtų siekti apie 50 %. Atlikto eksperimento rezultatai parodė didesnę reikšmę abiem funkcijoms (~66 %), kas rodo ryškų lavinos efektą: vieno simbolio pakeitimas įvestyje lemia radikalius išėjimo skirtumus. Nors gautos reikšmės viršija teorinę 50 % ribą, tai tikėtina susiję su taikyta skaičiavimo metodika. Šiuo požiūriu tiek HashFun, tiek SHA-256 elgiasi panašiai.

## 7. Negrįžtamumo demonstracija (hiding)

Šiame teste patikrinama ar pridėjus atsitiktinę salt reikšmę prie įvesties (`input + salt`) gaunamas maišos rezultatas neleidžia praktiškai atkurti pradinio input ir ar skirtingi salt'ai duoda skirtingus hash'us.

| Įvestis  | Salt'as          | HashFun hash'as                                                  | SHA-256 hash'as                                                           |
|----------|------------------|------------------------------------------------------------------|------------------------------------------------------------------|
| password | wE3j7ABS2znvst7w | c15bd20e68a992b44ed54d2167657f0b9079c70b3e19292876c2af0558f3a117 | f781aef635925254a907b1627ce923fb53882c2e87d521e1c7c71788bed4d279 |
| password | CPGPqzjSYyPdUfXN | 9a9e27e0e058f9da509af2e060a8c97871b223d8260fd3f465c778027e38cf00 | c7876305f12ff3a1e41bc4ea545cfdc66bb74cde759952d6e76d5f8d1447dabd |
| password | J14CzwHdQROGpKf8 | 91ec2679370a14aca0168e7ac5f6d1646ad06414dbea806a6dcc5ead9a162a19 | 073c655744015665a650196dbc274fd69adc432677299bfa6d3f3c29878aab3e |
| 123      | 3ITciUEWkZJGDZph | 36461c597345f62e6179ab27b33fc0e6e98f821492287de0ef1a7426d0b22a31 | e316dba129ec115a283b0a2e89f42b684968da809a3cecd57ccda65f5fba9766 |
| 123      | eQxpzFxZxv8WFbKO | 135d180d86a50df2645e54cb33d6c2eefef4d339bc668e94fffc514fbfbc2c33 | e1a5e6286829b065abcfeb0b002c74b2c195baf9e009a65cb73ea387cb3d56c7 |
| 123      | UJglVSJiAYyP1OA7 | 90925ec3057386dea887303b925559bf4543c261d407b1a2eda4ddd15b6d338b | f78f089251d3b697094274aa6635bd6a88e1c7bf2b8ddce7065d09b798f55f62 |
| a        | XDuLthwhJk1xL9ep | a06fc92dc256eeae8b791f7fbb6ef39ece8208198ff100919090ae66a707ce7a | 4c1c5b2493ae3873b91cd4aba25dded396094de6983a8fdc0e8fc0372f872aa3 |
| a        | 9v0kmD6W5wxrr1We | 10ca5b2d9757d5ef303b0a193f106bca359cc18496edbee2fe9edfb7222e7c07 | 3dd8a6f8fd55c4c48788362bda6147d4f10fa80b090652876aaae40a8c298e7a |
| a        | pwEw0SpTl3Jl00XS | 277385e5f613ce39d2583f2ecbd5ce376dd33715f0b0630276694a1a4596f3f2 | 2739b829aab56d51ed5e447ac31eea78c9d96500e886f08b8d8283cc41f0b004 |

Eksperimento rezultatai rodo, kad tiek `HashFun`, tiek `SHA-256` elgiasi kaip hiding funkcijos: tas pats įvedimas su skirtingais salt'ais generuoja visiškai skirtingus hash'us. Be žinomo salt, atstatyti pradinį tekstą iš vieno hash'o praktiškai neįmanoma.

## 8. Išvados

### Palyginimas su SHA‑256

- Abi funkcijos grąžina 256 bitų (64 hex) rezultatus, nepriklausomai nuo įvesties.
- Deterministiškumas panašus – ta pati įvestis visuomet duoda tą patį hash'ą.
- Lavinos efektas: HashFun vidutiniškai ~66 % bitų pasikeitimas, SHA‑256 ~66 %, t. y. abi funkcijos reaguoja panašiai į vieno simbolio pakeitimą.
- Kolizijų testuose nei HashFun, nei SHA‑256 neaptikta.
- Efektyvumas: HashFun greitesnė mažoms įvestims, tačiau su dideliais failais SHA‑256 gali būti efektyvesnė dėl optimizuotų bibliotekų.
- Hiding savybė: abiejose funkcijose pridėjus atsitiktinį salt tas pats įvesties tekstas generuoja labai skirtingus hash'us.

### HashFun stipriosios pusės

- Deterministinė ir visada grąžina 256 bitų (64 hex) rezultatą.
- Vidutinis 66 % bitų pokytis rodo gerai veikianti lavinos efektą.
- 100 000 atsitiktinių porų įvairaus ilgio nepateikė nė vienos kolizijos.

### HashFun silpnosios pusės

- Neištirtas kriptografinis atsparumas prieš atvirkštinę paiešką ar pasirinktas kolizijas.
- Nors veikimas greitas, nebuvo atlikti testai su itin dideliais (>GB) failais.
- Nepatikrintos standartinės kriptografinės atakos (diferencinė analizė, linijinė analizė).
- Reikalingi papildomi formalūs testai dėl saugumo ir stiprumo.
