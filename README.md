# Eksperimentinis tyrimas

Šiame projekte kuriama ir tiriama mano sukurta **C++ maišos (hash) funkcija**.  
Tikslas – įvertinti jos savybes pagal užduoties reikalavimus.

---

## 1. Testinių failų paruošimas

Eksperimentams sukuriau kelis failus su skirtingu turiniu:

| Failo pavadinimas | Turinys / paskirtis                                           |
|-------------------|---------------------------------------------------------------|
| `blank.txt`       | Tuščias failas                                                |
| `a.txt`           | Viena mažoji raidė **a**                                      |
| `b.txt`           | Viena mažoji raidė **b**                                      |
| `rnd100000.txt`   | 100 000 atsitiktinių simbolių (raidės, skaičiai, ženklai)     |
| `rnd1000000.txt`  | 1 000 000 atsitiktinių simbolių                               |
| `rndF100000.txt`  | 100 000 atsitiktinių simbolių                                 |
| `rndS100000.txt`  | Kaip rndF100000.txt, bet pakeistas 75 000-asis simbolis       |

## 2.  Išvedimo dydis

Paleidau savo "HashFun" tuščiam failui, failui su a, failui su b ir failams su 100 000 bei 1 000 000 atsitiktinių simbolių. Kiekvienu atveju gauta 64 hex'ų eilutė. Tai atitinka 256 bitų ilgį ir įrodo, kad išvedimo dydis nepriklauso nuo įvesties.

| Failas          | Hash rezultatas (pavyzdys)                                      |
|-----------------|-----------------------------------------------------------------|
| `blank.txt`     |e1e10ec33852dafdad0a90ef0edf56b9f7c1425813bc42d0a67386e5c1269b78 |
| `a.txt`         |13fbc56937664ae5ce4503508cf94ba6b82eaa19b9bc3c5bb8a7950b485df478 |
| `b.txt`         |0cce237961797622b9053b0d5e7c7c56848bcec8da55ef82c46496b265eee872 |
| `rnd100000.txt` |bf6fb7e57c24cee30ce7d062523997b2dcef0b49b1218c91dea9fca4751126f2 |
| `rnd1000000.txt`|afe953bc81c149bb814f8da11f69edd37b52b149d43643c1d85c4b5974b9c521 |

## 3. Deterministiškumas

Kiekvieną testinį failą hash’inau du kartus – rezultatai visada sutapo, todėl funkcija yra deterministinė.

Pavyzdys su `a.txt`:

13fbc56937664ae5ce4503508cf94ba6b82eaa19b9bc3c5bb8a7950b485df478
13fbc56937664ae5ce4503508cf94ba6b82eaa19b9bc3c5bb8a7950b485df478

Tas pats galioja visiems kitiems failams (`blank.txt`, `b.txt`, `rnd100000.txt`, `rnd1000000.txt`, `rndF100000.txt`, `rndS100000.txt`).

## 4. Efektyvumas

Hash funkcijos greitis buvo matuojamas naudojant `konstitucija.txt` failą. Matavau tik pačios hash funkcijos vykdymo laiką (be failo skaitymo) ir kiekvieną bandymą paleidau kelis kartus bei užrašiau vidutinę trukmę mikrosekundėmis.

| Įvedimo dydis (eilučių sk.) | Vidutinis veikimo laikas (µs)|
|-----------------------------|------------------------------|
| 1 eilutė                    | 2 µs                         |
| 10 eilučių                  | 3 µs                         |
| 100 eilučių                 | 23 µs                        |
| 789 eilučių (pilnas failas) | 260 µs                       |

Žemiau rezultatai pateikti naudojant grafiką:
![Hash funkcijos veikimo laikas](grafikas.png)

## 5. Kolizijų paieška

Šiame etape patikrinau, ar mano sukurta hash funkcija sukuria skirtingus hash’us atsitiktinėms įvestims.

Iš pradžių papildžiau programos kodą, pridėdamas naujas funkcijas, kurios kartu generuoja po 100 000 atsitiktinių string porų, kiekvieną porą hash'uoja ir tikrina ar nebuvo identiškų hash'ų.

Poros generuotos keturių skirtingų ilgių: 10, 100, 500 ir 1000 simbolių, kiekvienam ilgiui atlikau po 3 bandymus, neaptikta ne vienos kolizijos:

| Ilgis (simbolių) | Bandymas 1 | Bandymas 2 | Bandymas 3 | Vidurkis |
|------------------|-----------:|-----------:|-----------:|---------:|
| 10               | 0          | 0          | 0          | **0**    |
| 100              | 0          | 0          | 0          | **0**    |
| 500              | 0          | 0          | 0          | **0**    |
| 1000             | 0          | 0          | 0          | **0**    |

## 6. 