#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <limits>

using namespace std;

// Määritellään vakioita
const int MAX_HUONEET = 300; // Maksimimäärä huoneita
const int MIN_HUONEET = 40;  // Minimimäärä huoneita
const int MAX_VAR_NUMERO = 99999;
const int MIN_VAR_NUMERO = 10000;

// Rakenteet
struct Varaus {
    int varausnumero;
    string varaaja;
    int huonenumero;
    int oiden_maara;
    double loppuhinta;
    bool kaytossa;
};

// Funktiot
void naytaValikko();
void naytaOtsikko();
void teeVaraus(Varaus varaukset[], bool huoneet[], int huoneMaara, int yhdenHengenHuoneet, int& varauksienLkm);
void haeVaraus(Varaus varaukset[], int varausMaara);
void naytaSaatavillaOlevatHuoneet(bool huoneet[], int yhdenHengenHuoneet, int huoneidenMaara);
bool onHuoneVapaa(bool huoneet[], int huonenumero);
double laskeHinta(int huonetyyppi, int oiden_maara);
double arvoAlennus(double alkuperainen);
void alustaHuoneet(bool huoneet[], int huoneMaara);
void tyhjennaSyotevirta();
void perutaVaraus(Varaus varaukset[], bool huoneet[], int& varauksienLkm);
bool tarkistaNumero(int& syote);

int main() {
    srand(time(0)); // Satunnaisuuden alustaminen

    // Arvotaan huoneiden määrä ja jaetaan yhden/kahden hengen huoneet
    int huoneidenMaara = (rand() % ((MAX_HUONEET - MIN_HUONEET) / 2 + 1)) * 2 + MIN_HUONEET;
    int yhdenHengenHuoneet = huoneidenMaara / 2;

    bool huoneet[MAX_HUONEET] = { false };    // Taulukko, jossa seurataan huoneiden varaustilaa
    Varaus varaukset[MAX_HUONEET] = {};     // Taulukko varauksille
    int varauksienLkm = 0;                  // Varausten määrä

    alustaHuoneet(huoneet, huoneidenMaara);

    int valinta;

    do {
        naytaOtsikko();
        naytaValikko();
        cin >> valinta;

        switch (valinta) {
        case 1:
            teeVaraus(varaukset, huoneet, huoneidenMaara, yhdenHengenHuoneet, varauksienLkm);
            break;
        case 2:
            haeVaraus(varaukset, varauksienLkm);
            break;
        case 3:
            perutaVaraus(varaukset, huoneet, varauksienLkm);
            break;
        case 4:
            cout << "Ohjelma suljetaan.\n";
            break;
        default:
            cout << "Virheellinen valinta, yrita uudelleen.\n";
        }
    } while (valinta != 4);

    return 0;
}

// ASCII-taideotsikko
void naytaOtsikko() {
    cout << "*********************************************\n";
    cout << "*                                           *\n";
    cout << "*     HOTELLIHUONEEN VARAUSOHJELMA          *\n";
    cout << "*                                           *\n";
    cout << "*********************************************\n\n";
}

// Näyttää ohjelman valikon
void naytaValikko() {
    cout << "Valitse seuraavista toiminnoista:\n";
    cout << "1. Tee uusi varaus\n";
    cout << "2. Hae varausta\n";
    cout << "3. Peruuta varaus\n";
    cout << "4. Lopeta ohjelma\n";
    cout << "Valitse toiminto: ";
}

// Tekee uuden varauksen
void teeVaraus(Varaus varaukset[], bool huoneet[], int huoneMaara, int yhdenHengenHuoneet, int& varauksienLkm) {
    int huonetyyppi, huonenumero, oiden_maara;
    string varaaja;

    cout << "\nSaatavilla olevat huoneet:\n";
    naytaSaatavillaOlevatHuoneet(huoneet, yhdenHengenHuoneet, huoneMaara);

    cout << "Valitse huonetyyppi (1 = 1 hengen huone, 2 = 2 hengen huone): ";
    cin >> huonetyyppi;

    if (cin.fail() || (huonetyyppi < 1 || huonetyyppi > 2)) {
        cout << "Virheellinen huonetyyppi! Yrita uudelleen.\n";
        tyhjennaSyotevirta();
        return;
    }

    int minHuone = (huonetyyppi == 1) ? 1 : yhdenHengenHuoneet + 1;
    int maxHuone = (huonetyyppi == 1) ? yhdenHengenHuoneet : huoneMaara;

    cout << "Anna haluamasi huonenumero (" << minHuone << "-" << maxHuone << "): ";
    cin >> huonenumero;

    if (cin.fail() || huonenumero < minHuone || huonenumero > maxHuone || !onHuoneVapaa(huoneet, huonenumero)) {
        cout << "Huonetta ei voi varata. Se on joko varattu tai numero on virheellinen.\n";
        tyhjennaSyotevirta();
        return;
    }

    cout << "Anna varaajan nimi: ";
    cin.ignore();
    getline(cin, varaaja);

    cout << "Anna oiden maara: ";
    cin >> oiden_maara;

    if (cin.fail() || oiden_maara <= 0) {
        cout << "Virheellinen oiden maara! Yrita uudelleen.\n";
        tyhjennaSyotevirta();
        return;
    }

    double alkuperainenHinta = laskeHinta(huonetyyppi, oiden_maara);
    double loppuhinta = arvoAlennus(alkuperainenHinta);

    // Luodaan varaus
    int varausnumero = rand() % (MAX_VAR_NUMERO - MIN_VAR_NUMERO + 1) + MIN_VAR_NUMERO;
    varaukset[varauksienLkm] = { varausnumero, varaaja, huonenumero, oiden_maara, loppuhinta, true };
    huoneet[huonenumero - 1] = true;
    varauksienLkm++;

    cout << "Varaus onnistui! Varausnumero: " << varausnumero << ", Loppuhinta: " << fixed << setprecision(2) << loppuhinta << " euroa\n";
}

// Hakee varausta varausnumeron tai nimen perusteella
void haeVaraus(Varaus varaukset[], int varausMaara) {
    int hakutapa;
    bool virheellinenSyote = false;

    do {
        cout << "Haluatko hakea (1) varausnumerolla vai (2) nimella? ";
        cin >> hakutapa;

        if (cin.fail() || (hakutapa != 1 && hakutapa != 2)) {
            cout << "Virheellinen valinta! Yrita uudelleen.\n";
            tyhjennaSyotevirta();
            virheellinenSyote = true;
        }
        else {
            virheellinenSyote = false; // Jos syöte on kelvollinen, jatketaan normaalisti
        }
    } while (virheellinenSyote);

    if (hakutapa == 1) {
        int varausnumero;
        cout << "Anna varausnumero: ";
        cin >> varausnumero;

        bool loydetty = false;
        for (int i = 0; i < varausMaara; i++) {
            if (varaukset[i].kaytossa && varaukset[i].varausnumero == varausnumero) {
                cout << "Varaaja: " << varaukset[i].varaaja << ", Huonenumero: " << varaukset[i].huonenumero
                    << ", Oiden maara: " << varaukset[i].oiden_maara << ", Hinta: " << varaukset[i].loppuhinta << " euroa\n";
                loydetty = true;
                break;
            }
        }
        if (!loydetty) {
            cout << "Varausta ei loydy.\n";
        }
    }
    else if (hakutapa == 2) {
        string varaaja;
        cout << "Anna varaajan nimi: ";
        cin.ignore();
        getline(cin, varaaja);

        bool loydetty = false;
        for (int i = 0; i < varausMaara; i++) {
            if (varaukset[i].kaytossa && varaukset[i].varaaja == varaaja) {
                cout << "Varausnumero: " << varaukset[i].varausnumero << ", Huonenumero: " << varaukset[i].huonenumero
                    << ", Oiden maara: " << varaukset[i].oiden_maara << ", Hinta: " << varaukset[i].loppuhinta << " euroa\n";
                loydetty = true;
                break;
            }
        }
        if (!loydetty) {
            cout << "Varausta ei loydy.\n";
        }
    }
}

// Näyttää vapaat huoneet
void naytaSaatavillaOlevatHuoneet(bool huoneet[], int yhdenHengenHuoneet, int huoneidenMaara) {
    cout << "Yhden hengen huoneet: ";
    for (int i = 0; i < yhdenHengenHuoneet; i++) {
        if (!huoneet[i]) cout << i + 1 << " ";
    }
    cout << "\n\nKahden hengen huoneet: ";
    for (int i = yhdenHengenHuoneet; i < huoneidenMaara; i++) {
        if (!huoneet[i]) cout << i + 1 << " ";
    }
    cout << endl << endl;
}

// Tarkistaa, onko huone vapaa
bool onHuoneVapaa(bool huoneet[], int huonenumero) {
    return !huoneet[huonenumero - 1];
}

// Laskee huoneen hinnan
double laskeHinta(int huonetyyppi, int oiden_maara) {
    double perOikaHinta = (huonetyyppi == 1) ? 100.0 : 150.0;
    return perOikaHinta * oiden_maara;
}

// Arpoo alennuksen
double arvoAlennus(double alkuperainen) {
    int alennus = rand() % 3; // 0, 1, 2 (0%, 10%, 20%)
    double alennusprosentti = alennus * 0.1;
    return alkuperainen * (1 - alennusprosentti);
}

// Alustaa huoneet vapaiksi
void alustaHuoneet(bool huoneet[], int huoneMaara) {
    for (int i = 0; i < huoneMaara; i++) {
        huoneet[i] = false;
    }
}

// Tyhjentää syötevirran virhetilanteessa
void tyhjennaSyotevirta() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Peruuta varaus
void perutaVaraus(Varaus varaukset[], bool huoneet[], int& varauksienLkm) {
    int varausnumero;
    cout << "Anna peruutettavan varauksen varausnumero: ";

    while (!tarkistaNumero(varausnumero)) {
        cout << "Virheellinen syote. Anna validi varausnumero: ";
    }

    bool loydetty = false;
    for (int i = 0; i < varauksienLkm; i++) {
        if (varaukset[i].kaytossa && varaukset[i].varausnumero == varausnumero) {
            huoneet[varaukset[i].huonenumero - 1] = false; // Huone tulee jälleen vapaaksi
            varaukset[i].kaytossa = false; // Varaus peruutetaan
            cout << "Varaus peruutettu. Huone " << varaukset[i].huonenumero << " on jälleen vapaa.\n";
            loydetty = true;
            break;
        }
    }
    if (!loydetty) {
        cout << "Varausta ei loydy.\n";
    }
}

// Tarkistaa, että syöte on validi numero
bool tarkistaNumero(int& syote) {
    if (cin >> syote) {
        return true;
    }
    else {
        tyhjennaSyotevirta();
        return false;
    }
}