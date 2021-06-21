#include <stdio.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>

/*
 * Projekt zaliczeniowy z programowania strukturalnego
 *
 * SPIS TRE¦CI
 * 0 - Ustawienia domy¶lne / sta³e
 * 1 - Definicja enumów i ich typów
 * 2 - Definicje struktur i ich typów
 *      2.0 zwi±zane z danymi obliczeñ
 *      2.1 zwi±zane z menu g³ównym / ustawieniami
 * 3 - Prototypy funkcji
 * 4 - Main()
 * 5 - Definicje funkcji
 * Uwagi:
 * W programie u¿yto:
 * Implementacji algorytmu biegunowego Marsaglii na podstawie
 * https://phoxis.org/2013/05/04/generating-random-numbers-from-normal-distribution-in-c/
 * wykorzystane w funkcjach generateNoiseMonoSum() i generateNoisePolySum()
 *
 * Je¶li chodzi o u¿ycie kodu, ca³o¶æ opiera siê na wybraniu odpowiedniej funkcji, który to wybór zostaje zapisany
 * w strukturze export_settings. Reszta programu (seria funkcji select.*) sprawdza tê warto¶æ (i warto¶ci struktury
 * menu_flags) i dokonuje wyboru odpowiednich funkcji dla okre¶lonych kroków podawania danych
 * (funkcji addParams.*, addNoise.*, addTime.*, funcCalc.*, generate.*)
 *
 * Wyj±tkiem jest funkcja selectExport() która zajmuje siê eksportowaniem obliczonych danych do podanych plików.
 *
 * Uwaga zwi±zana z CMake:
 * Pliki s± eksportowane do tego samego folderu w którym jest skompilowany program, czyli ./cmake-build-debug
 */

/*###########################################
 * 0 - Ustawienia domy¶lne / sta³e
 *###########################################*/
#define DEFAULT_FLOAT_PRECISION 5 // 1 - 15

/*###########################################
 * 1. Definicja enumów i ich typów
 *###########################################*/
enum flags {
    FLAG_PROCESS_TYPE,
    FLAG_PROCESS_PARAM,
    FLAG_NOISE_PARAM,
    FLAG_TIME_RANGE,
    FLAG_GENERATED,
    FLAG_EXPORTED,
};

enum func_types {
    FUNC_LINEAR,
    FUNC_EXPONENTIAL,
    FUNC_POWER,
    FUNC_LOGARITHMIC,
    FUNC_POLYNOMIAL,
    FUNC_NONE
};

typedef enum {
    CON_INPUT_TOO_BIG,
    CON_INPUT_TOO_SMALL,
    CON_ERR_CHARACTER,
    CON_SUCCESS
} conversion_errno;

/*###########################################
 * 2. Definicje struktur i ich typów
 * 2.0 zwi±zane z danymi obliczeñ
 * 2.1 zwi±zane z menu g³ównym / ustawieniami
 *###########################################*/

/*################### 2.0 ###################*/
typedef struct {
    float frequency;
    float time;
    float time_start_point;
    float a; float b;
    float std_deviation;
    float expected_value;
    int array_size;
    float *values; // wska¼nik na array
    float *noise_values;
} mono_struct;

typedef struct {
    float frequency;
    float time;
    float time_start_point;
    float a_6; float a_5; float a_4; float a_3; float a_2; float a_1; float a_0;
    float std_deviation;
    float expected_value;
    int array_size;
    float *values;
    float *noise_values;
} poly_struct;

/*################### 2.1 ###################*/
struct { // struktura z polami bitowymi informuj±cymi czy dane pole zosta³o wype³nione
    bool process_type : 1;
    bool process_param : 1;
    bool noise_param : 1;
    bool time_range : 1;
    bool generated : 1;
    bool exported : 1;
    bool debug_mode : 1;
} menu_flags;
//  unsigned char s± u¿yte dla oszczêdno¶ci pamiêci wzglêdem int. 1 bajt (dla obu zmiennych) vs 8
struct { // zawiera informacje potrzebne do stworzenia i eksportu danych
    unsigned char float_precision : 4; // 0-15
    unsigned char selected_function : 3; // 0-7
    mono_struct* mono_data;
    poly_struct* poly_data;
} export_settings;
/*###########################################
 * 3. Prototypy funkcji
 * Definicje funkcji s± rozmieszczone w tej samej kolejno¶ci co prototypy
 *###########################################*/
// funkcje pomocnicze programu
void debugFlags();
void debugModePrint();
void debugPrint(float *array, int array_size);
void setFlag(int flag, bool value);
void clearData();

void settings();
float setPrecision(float number, int precision); // na sytuacje gdy 0.1 + 0.2 powinno daæ 0.3

conversion_errno str2int(int* out, char* str_in, bool positive_only); // konwersja wprowadzonego inputu na cyfrê
conversion_errno str2float(float* out, char* str_in, bool positive_only);

int smartInputInt(bool positive_only); // funkcja zwracaj±ca prawid³ow± liczbê od u¿ytkownika
float smartInputFloat(bool positive_only); // z mo¿liwo¶ci± uzyskania tylko dodatnich liczb

// funkcje g³ówne
bool selectProcessType(); // zwraca informacjê czy co¶ zosta³o wybrane czy nie
void selectProcessParams();
void selectNoiseParams();
void selectTimeRange();
void selectGenerate();
void selectExport();

void addParamsLinear(mono_struct* new_linear); // f(t) = a * t + b
void addParamsExpo(mono_struct* new_expo); // f(t) = a^t * b
void addParamsPower(mono_struct* new_power); // f(t) = t^a * b
void addParamsLog(mono_struct* new_log); // f(t) = log_a(t)
void addParamsPoly(poly_struct* new_poly); // f(t) = a_n * x^n + ... + a_1 * x + a_0

void addNoiseMono(mono_struct* new_mono);
void addNoisePoly(poly_struct* new_poly);

void addTimeMono(mono_struct* new_mono);
void addTimePoly(poly_struct* new_poly);

void funcCalcLinear(mono_struct* new_linear);
void funcCalcExpo(mono_struct* new_expo);
void funcCalcPower(mono_struct* new_power);
void funcCalcLog(mono_struct* new_log);
void funcCalcPoly(poly_struct* new_poly);

void generateNoiseMonoSum(mono_struct* new_mono); // c - warto¶æ losowa
void generateNoisePolySum(poly_struct *new_poly); // y = f(t) + c

void generateLinear(mono_struct* new_linear);
void generateExpo(mono_struct* new_expo);
void generatePower(mono_struct* new_power);
void generateLog(mono_struct* new_log);
void generatePoly(poly_struct* new_poly);

/*###########################################
 * 4. Main()
 *###########################################*/
int main() {
    setlocale(LC_ALL, "");
    // menu g³ówne jest budowane z tych elementów, na podstawie stanu flag
    const char *menu_intro = "Symulacja Systemu Pomiarowego\n";
    const char *menu_choice= "Wybierz cyfre by wybrac dana opcje\n";
    const char *menu_info =  "Pole po cyfrze informuje czy dane pole zostalo wypelnione\n"
                       "W ustawieniach mozna wybrac dokladnosc wynikow\n";
    const char *menu_separator = "==============================\n"; // 33 =
    const char *menu_to_select_without[] = {
                        "[1][ ] - Charakterystyka procesu\n",
                        "[2][ ] - Parametry procesu\n",
                        "[3][ ] - Parametry zaklocen\n",
                        "[4][ ] - Horyzont czasowy\n",
                        "[5][ ] - Generuj wartosci\n",
                        "[6][ ] - Eksportuj do arkuszy\n",
                        "[7] - Ustawienia\n",
                        "[8] - Wyjdz\n"
    };
    const char *menu_to_select_with[]= {
                        "[1][*] - Charakterystyka procesu\n",
                        "[2][*] - Parametry procesu\n",
                        "[3][*] - Parametry zaklocen\n",
                        "[4][*] - Horyzont czasowy\n",
                        "[5][*] - Generuj wartosci\n",
                        "[6][*] - Eksportuj do arkuszy\n"
    };
    const char *menu_debug =  "Wlaczono tryb podgladu, beda wyswietlane dodatkowe opcje\n";

    // Pocz±tkowe ustawienia
    export_settings.float_precision = DEFAULT_FLOAT_PRECISION;
    export_settings.selected_function = FUNC_NONE;

    bool menu_displayed = false;
    bool additions_displayed = false; // dla sprawdzenia czy potrzebne (estetycznie) jest dodanie kolejnego separatora
    srand(time(NULL));
    int choice = 0; // !choice - pierwszy przebieg
    while (choice != 8) {
        if ( choice > 0 && choice < 9 || !menu_displayed) {
            // If dla unikniêcia spamowania menu g³ównym przy podaniu nieprawid³owej opcji
            printf("%s%s", menu_intro, menu_separator);
            if (menu_flags.debug_mode) {
                printf("%s", menu_debug);
                debugModePrint();
                additions_displayed = true;
            }
            if (!choice) { printf("%s", menu_info); additions_displayed = true; }
            if (additions_displayed) { printf("%s", menu_separator); additions_displayed = false;}
            printf("%s%s%s%s%s%s%s%s%s%s",
                menu_flags.process_type ? menu_to_select_with[0] : menu_to_select_without[0], // w zale¿no¶ci czy flaga jest prawdziwa czy nie
                menu_flags.process_param ? menu_to_select_with[1] : menu_to_select_without[1], // dobrana jest odpowiednia opcja
                menu_flags.noise_param ? menu_to_select_with[2] : menu_to_select_without[2],
                menu_flags.time_range ? menu_to_select_with[3] : menu_to_select_without[3],
                menu_flags.generated ? menu_to_select_with[4] : menu_to_select_without[4],
                menu_flags.exported ? menu_to_select_with[5] : menu_to_select_without[5],
                menu_to_select_without[6],
                menu_to_select_without[7],
                menu_separator,
                menu_choice);

            menu_displayed = true;
        }
        choice = smartInputInt(true);
        switch (choice) {
            case 1:
                if (selectProcessType()) { // Je¶li dokonano wyboru...
                    if (menu_flags.process_type || menu_flags.exported) { // ...po raz kolejny lub po exportowaniu danych...
                        clearData(); // ...wyczy¶æ dotychczasowe dane
                    }
                    switch (export_settings.selected_function) {
                        case FUNC_LINEAR:
                        case FUNC_EXPONENTIAL:
                        case FUNC_POWER:
                        case FUNC_LOGARITHMIC:{
                            static mono_struct new_mono;
                            export_settings.mono_data = &new_mono;
                        } break;
                        case FUNC_POLYNOMIAL: {
                            static poly_struct new_poly;
                            export_settings.poly_data = &new_poly;
                        } break;
                    }
                    setFlag(FLAG_PROCESS_TYPE, true);
                    menu_displayed = false;
                } break;
            case 2:
                if (menu_flags.process_type) {
                    selectProcessParams();
                    setFlag(FLAG_PROCESS_PARAM, true);
                    menu_displayed = false;
                } else printf("> Najpierw wybierz typ procesu! <\n");
                break;
            case 3:
                if (menu_flags.process_type) {
                    selectNoiseParams();
                    setFlag(FLAG_NOISE_PARAM, true);
                    menu_displayed = false;
                } else printf("> Najpierw wybierz typ procesu! <\n");
                break;
            case 4:
                if (menu_flags.process_type) {
                    selectTimeRange();
                    setFlag(FLAG_TIME_RANGE, true);
                    menu_displayed = false;
                } else printf("> Najpierw wybierz typ procesu! <\n");
                break;
            case 5:
                if (menu_flags.process_type && menu_flags.process_param
                && menu_flags.noise_param && menu_flags.time_range) {
                    selectGenerate();
                    setFlag(FLAG_GENERATED, true);
                    menu_displayed = false;
                } else printf("> Najpierw uzupelnij pozostale pola! <\n");
                break;
            case 6:
                if (menu_flags.generated) {
                    selectExport();
                    setFlag(FLAG_EXPORTED, true);
                    menu_displayed = false;
                } else printf("> Najpierw wygeneruj dane! <\n");
                break;
            case 7:
                settings(); menu_displayed = false; break;
            case 8:
                printf("Do widzenia!\n"); break;
            default:
                printf("Wpisz liczbe od 1 do 7.\n"); break;
        }
    }
    clearData();
    return 0;
}

/*###########################################
 * 5. Definicje funkcji
 *###########################################*/
void debugFlags() {
    printf("Rozmiar menu_flags (bajty): %d\n"
           "==============================\n"
           "process_type: %d | process_param: %d\n"
           "noise_param: %d | time_range: %d\n"
           "generated: %d | exported: %d\n"
           "debug_mode: %d\n",
           sizeof (menu_flags), menu_flags.process_type, menu_flags.process_param, menu_flags.noise_param,
           menu_flags.time_range, menu_flags.generated, menu_flags.exported, menu_flags.debug_mode);

    printf("Rozmiar export_settings (bajty): %d\n"
           "==============================\n"
           "float_precision: %d\nselected_function: %d\n"
           "0 - Liniowa, 1 - Wykladnicza, 2 - Potegowa, 3 - Logarytmiczna, 4 - Wielomianowa, 5 - nic\n",
           sizeof(export_settings), export_settings.float_precision, export_settings.selected_function);
}

void debugModePrint() {
    if (menu_flags.process_type) {
        switch (export_settings.selected_function) {
            case FUNC_LINEAR: printf("Charakterystyka procesu - Liniowa\n"); break;
            case FUNC_EXPONENTIAL: printf("Charakterystyka procesu - Wykladnicza\n"); break;
            case FUNC_POWER: printf("Charakterystyka procesu - Potegowa\n"); break;
            case FUNC_LOGARITHMIC: printf("Charakterystyka procesu - Logarytmiczna\n"); break;
            case FUNC_POLYNOMIAL: printf("Charakterystyka procesu - Wielomianowa\n"); break;
        }
        switch (export_settings.selected_function) {
            case FUNC_LINEAR:
            case FUNC_EXPONENTIAL:
            case FUNC_POWER:
            case FUNC_LOGARITHMIC:
                if (menu_flags.process_param)
                    printf("Parametr a = %f | Parametr b = %f\n", export_settings.mono_data->a, export_settings.mono_data->b);
                if (menu_flags.noise_param)
                    printf("Wartosc oczekiwana = %f | odchylenie standardowe = %f\n",
                           export_settings.mono_data->expected_value, export_settings.mono_data->std_deviation);
                if (menu_flags.time_range)
                    printf("Okres symulacji = %f | Czestotliwosc = %f | punkt startowy = %f\n",
                           export_settings.mono_data->time, export_settings.mono_data->frequency, export_settings.mono_data->time_start_point);
                break;
            case FUNC_POLYNOMIAL:
                if (menu_flags.process_param)
                    printf("Wartosc 0 oznacza, ze dany parametr jest pominiety, a liczba (np. 6 w a_6) oznacza jego stopien.\n"
                           "a_6 = %f | a_5 = %f | a_4 = %f | a_3 = %f\na_2 = %f | a_1 = %f | a_0 = %f\n",
                           export_settings.poly_data->a_6, export_settings.poly_data->a_5, export_settings.poly_data->a_4,
                           export_settings.poly_data->a_3, export_settings.poly_data->a_2, export_settings.poly_data->a_1,
                           export_settings.poly_data->a_0);
                if (menu_flags.noise_param)
                    printf("Wartosc oczekiwana = %f | odchylenie standardowe = %f\n",
                           export_settings.poly_data->expected_value, export_settings.poly_data->std_deviation);
                if (menu_flags.time_range)
                    printf("Okres symulacji = %f | Czestotliwosc = %f | punkt startowy = %f\n",
                           export_settings.poly_data->time, export_settings.poly_data->frequency, export_settings.poly_data->time_start_point);
                break;
        }
    }
}

void debugPrint(float *array, int array_size) {
    printf("[ ");
    for (int i = 0; i < array_size; i++) {
        printf("%f | ", array[i]);
    }
    printf("]\n");
}

void setFlag(int flag, bool value) {
    switch (flag) {
        case FLAG_PROCESS_TYPE: menu_flags.process_type = value; break;
        case FLAG_PROCESS_PARAM: menu_flags.process_param = value; break;
        case FLAG_NOISE_PARAM: menu_flags.noise_param = value; break;
        case FLAG_TIME_RANGE: menu_flags.time_range = value; break;
        case FLAG_GENERATED: menu_flags.generated = value; break;
        case FLAG_EXPORTED: menu_flags.exported = value; break;
        default: // Nigdy nie powinno byæ wykonywane
            printf("BLAD PROGRAMISTY: Niedozwolona wartosc flagi: %d\n", (int) flag);
            exit(1);
    }
}

void clearData() {
    menu_flags.process_type = false;
    menu_flags.process_param = false;
    menu_flags.noise_param = false;
    menu_flags.time_range = false;
    menu_flags.generated = false;
    menu_flags.exported = false;
    if (export_settings.mono_data != NULL) {
        if (export_settings.mono_data->values != NULL) free(export_settings.mono_data->values);
        free(export_settings.mono_data);
    }
    if (export_settings.poly_data != NULL) {
        if (export_settings.poly_data->values != NULL) free(export_settings.poly_data->values);
        free(export_settings.poly_data);
    }
}

void settings() {
    int choice = 0;
    bool menu_displayed = false;
    while (choice != 4) {
        if (choice > 0 && choice < 5 || !menu_displayed ) {
            printf("Tutaj mozesz wybrac ustawienia eksportu danych\n"
                   "[1] - Dokladnosc - Liczba cyfr po przecinku\n"
                   "[2] - Tryb podgladu\n"
                   "[3] - Wyswietl wartosci flag i rozmiary struktur (debug)\n"
                   "[4] - Wroc do menu glownego\n");
            menu_displayed = true;
        }
        choice = smartInputInt(true);
        switch (choice) {
            case 1: {
                printf("Obecnie: %d miejsc po przecinku\nNowy:\n", export_settings.float_precision);
                int precision = smartInputInt(true);
                if (precision > 0 && precision < 15) {
                export_settings.float_precision = precision;
                } else printf("Podaj liczbê od 0 do 15!\n");
                break; }
            case 2:
                printf("Obecnie: %s | Czy chcesz to zmienic?\n[0 - nie / pozostale - tak]:\n",
                       menu_flags.debug_mode ? "Wlaczony" : "Wylaczony");
                if (smartInputInt(false) != 0) menu_flags.debug_mode = !menu_flags.debug_mode;
                break;
            case 3:
                debugFlags(); break;
            case 4:
                break;
            default:
                printf("Wpisz liczbê od 1 do 4\n");
        }
    }
}

float setPrecision(float number, int precision)
{ // setPrecision(1.23456789, 5) -> 1.2345600...
    return floor(pow(10, precision) * number) / pow(10, precision);
}

conversion_errno str2int(int* out, char* str_in, bool positive_only) {
    /* out - integer w którym zapisuje siê otrzyman± liczbê
       str_in - ci±g znaków musi mieæ warto¶æ, nie mo¿e zaczynaæ siê od whitespace i zawieraæ liter
       -----------------------------------------------------------------------
       remainder - wska¼nik na pozosta³o¶æ po procesie przekszta³cania stringu w liczbê,
                je¶li komórka na któr± wskazuje ma warto¶æ 0 - str_in zawiera tylko liczbê
       number - warto¶æ zwrotna strtol() która bêdzie przypisana na dany adres o ile nie by³o b³êdów
    */
    char* remainder;
    long number = strtol(str_in, &remainder, 10); // konwersja dla systemu dziesi±tnego
    if (*remainder != 0) return CON_ERR_CHARACTER; // je¶li pojawi³a siê litera
    // sprawdzenie czy nie wprowadzono zbyt du¿ej b±d¼ za ma³ej liczby.
    if (number >= INT_MAX) return CON_INPUT_TOO_BIG;
    if (positive_only) {
        if (number <= 0) return CON_INPUT_TOO_SMALL;
    } else {
        if (number <= -INT_MAX) return CON_INPUT_TOO_SMALL;
    }
    *out = number;
    return CON_SUCCESS;
}

conversion_errno str2float(float* out, char* str_in, bool positive_only) {
    char* remainder; // analogicznie do powy¿szego
    double number = strtod(str_in, &remainder);
    if (*remainder != 0) return CON_ERR_CHARACTER;
    if (number >= FLT_MAX) return CON_INPUT_TOO_BIG;
    if (positive_only) {
        if (number <= 0) return CON_INPUT_TOO_SMALL;
    } else {
        if (number <= -FLT_MAX) return CON_INPUT_TOO_SMALL;
    }
    *out = number;
    return CON_SUCCESS;
}

int smartInputInt(bool positive_only) {
    int number; // warto¶æ zwrotna
    char input[16]; // miejsce na wprowadzone dane, 15 cyfr + '\0'
    int correct_input = false;
    while (correct_input != true) {
        fgets(input, 16, stdin);
        input[strcspn(input, "\n")] = '\0'; //podmienia znaleziony newline na koniec stringu
        conversion_errno status = str2int(&number, input, positive_only);
        switch (status) {
            case CON_INPUT_TOO_BIG:
                printf("Prosze wprowadzic mniejsza liczbe!\n"); break;
            case CON_INPUT_TOO_SMALL:
                printf("Prosze wprowadzic wieksza liczbe!\n"); break;
            case CON_ERR_CHARACTER:
                printf("Mozna wprowadzic tylko liczbe!\n"); break;
            case CON_SUCCESS:
                correct_input = true; break;
        }
    }
    return number;
}

float smartInputFloat(bool positive_only) {
    float number; // analogicznie do powy¿szego
    char input[16];
    float correct_input = false;
    while (correct_input != true) {
        fgets(input, 16, stdin);
        input[strcspn(input, "\n")] = '\0';
        conversion_errno status = str2float(&number, input, positive_only);
        switch (status) {
            case CON_INPUT_TOO_BIG:
                printf("Prosze wprowadzic mniejsza liczbe!\n"); break;
            case CON_INPUT_TOO_SMALL:
                printf("Prosze wprowadzic wieksza liczbe!\n"); break;
            case CON_ERR_CHARACTER:
                printf("Mozna wprowadzic tylko liczbe!\n"); break;
            case CON_SUCCESS:
                correct_input = true; break;
        }
    }
    return number;
}

bool selectProcessType() {
    int choice = 0;
    bool menu_displayed = false;
    bool anything_selected = false;
    while (choice != 6 && !anything_selected) {
        if (choice > 0 && choice < 7 || !menu_displayed) {
            printf("Wybierz charakterystyke procesu:\n"
                   "[1] - Liniowa\n"
                   "[2] - Wykladnicza\n"
                   "[3] - Potegowa\n"
                   "[4] - Logarytmiczna\n"
                   "[5] - Wielomianowa\n"
                   "[6] - Wroc do menu glownego\n");
            if (menu_flags.process_type) printf("Uwaga! Ponowny wybor kasuje poprzednio wprowadzone dane!\n");
            menu_displayed = true;
        }
        choice = smartInputInt(true);
        switch (choice) {
            case 1:
                export_settings.selected_function = FUNC_LINEAR;
                anything_selected = true; break;
            case 2:
                export_settings.selected_function = FUNC_EXPONENTIAL;
                anything_selected = true; break;
            case 3:
                export_settings.selected_function = FUNC_POWER;
                anything_selected = true; break;
            case 4:
                export_settings.selected_function = FUNC_LOGARITHMIC;
                anything_selected = true; break;
            case 5:
                export_settings.selected_function = FUNC_POLYNOMIAL;
                anything_selected = true; break;
            case 6: // po prostu przerywa pêtle
                break;
            default:
                printf("Wpisz liczbe od 1 do 6.\n"); break;
        }
    }
    return anything_selected;
}

void selectProcessParams() {
    switch (export_settings.selected_function) {
        case FUNC_LINEAR: addParamsLinear(export_settings.mono_data); break;
        case FUNC_EXPONENTIAL: addParamsExpo(export_settings.mono_data); break;
        case FUNC_POWER: addParamsPower(export_settings.mono_data); break;
        case FUNC_LOGARITHMIC: addParamsLog(export_settings.mono_data); break;
        case FUNC_POLYNOMIAL: addParamsPoly(export_settings.poly_data); break;
    }
}

void selectNoiseParams() {
    switch (export_settings.selected_function) {
        case FUNC_LINEAR: // cztery pierwsze u¿ywaj± tego samego kodu
        case FUNC_EXPONENTIAL: // wiêc mo¿na pomin±æ jego powtarzanie
        case FUNC_POWER:
        case FUNC_LOGARITHMIC: addNoiseMono(export_settings.mono_data); break;
        case FUNC_POLYNOMIAL: addNoisePoly(export_settings.poly_data); break;
    }
}

void selectTimeRange() {
    switch (export_settings.selected_function) {
        case FUNC_LINEAR: // jak wy¿ej
        case FUNC_EXPONENTIAL:
        case FUNC_POWER:
        case FUNC_LOGARITHMIC: addTimeMono(export_settings.mono_data); break;
        case FUNC_POLYNOMIAL: addTimePoly(export_settings.poly_data); break;
    }
}
void selectGenerate() {
    switch (export_settings.selected_function) {
        case FUNC_LINEAR: generateLinear(export_settings.mono_data); break;
        case FUNC_EXPONENTIAL: generateExpo(export_settings.mono_data); break;
        case FUNC_POWER: generatePower(export_settings.mono_data); break;
        case FUNC_LOGARITHMIC: generateLog(export_settings.mono_data); break;
        case FUNC_POLYNOMIAL: generatePoly(export_settings.poly_data); break;
    }
}

void selectExport() {
    FILE *base_data, *noised_data;
    char name_csv[64];
    char *file_names_info[] = {
            "Podaj nazwe dla pliku bez zaklocen:",
            "Podaj nazwe dla pliku z zakloceniami:"
    };
    char *file_error[] = {
            "eksportowanie anulowane!",
            "eksportowano tylko dane bez zaklocen!"
    };
    bool is_noiseless_exported = false;
    while (1) {
        printf("%s\n", file_names_info[is_noiseless_exported]);
        // nazwa (59) + ".csv" + '\0' = 64
        fgets(name_csv, 59, stdin); // zwraca text zakoñczony '\n'
        name_csv[strcspn(name_csv, "\n")] = '\0';
        strcat(name_csv, ".csv");
        base_data = fopen(name_csv, "w");
        if (base_data != NULL) {
            if (!is_noiseless_exported) {
                if (export_settings.mono_data != NULL) {
                    fprintf(base_data, "czas;wartosc\n");
                    for (int index = 0; index < export_settings.mono_data->array_size; index++) {
                        fprintf(base_data, "%g;%g\n", setPrecision(export_settings.mono_data->time_start_point
                                                                   + (index / export_settings.mono_data->frequency), export_settings.float_precision),
                                export_settings.mono_data->values[index]);
                    }
                    is_noiseless_exported = true;
                    fclose(base_data);
                } else if (export_settings.poly_data != NULL) {
                    fprintf(base_data, "czas;wartosc\n");
                    for (int index = 0; index < export_settings.poly_data->array_size; index++) {
                        fprintf(base_data, "%g;%g\n", setPrecision(export_settings.poly_data->time_start_point
                                                                   + (index / export_settings.poly_data->frequency), export_settings.float_precision),
                                export_settings.poly_data->values[index]);
                    }
                    is_noiseless_exported = true;
                    fclose(base_data);
                } else {
                    printf("Blad! Nie znaleziono danych do eksportu (bez zaklocen). Sprobuj ponownie wprowadzic dane\n");
                    return;
                }
            } else {
                if (export_settings.mono_data != NULL) {
                    fprintf(base_data, "czas;wartosc\n");
                    for (int index = 0; index < export_settings.mono_data->array_size; index++) {
                        fprintf(base_data, "%g;%g\n", setPrecision(export_settings.mono_data->time_start_point
                                                                   + (index / export_settings.mono_data->frequency), export_settings.float_precision),
                                export_settings.mono_data->noise_values[index]);
                    }
                    fclose(base_data);
                    break;
                } else if (export_settings.poly_data != NULL) {
                    fprintf(base_data, "czas;wartosc\n");
                    for (int index = 0; index < export_settings.poly_data->array_size; index++) {
                        fprintf(base_data, "%g;%g\n", setPrecision(export_settings.poly_data->time_start_point
                                                                   + (index / export_settings.poly_data->frequency), export_settings.float_precision),
                                export_settings.poly_data->noise_values[index]);
                    }
                    fclose(base_data);
                    break;
                } else {
                    printf("Blad! Nie znaleziono danych do eksportu (z zakloceniami). Sprobuj ponownie wprowadzic dane\n");
                    return;
                }
            }
        } else printf("Nie mozna otworzyc podanego pliku, %s\n", file_error[is_noiseless_exported]);
    }
}

void addParamsLinear(mono_struct *new_linear) {
    if (new_linear != NULL) {
        printf("f(t) = a * t + b\nPodaj wartosc parametru a:\n");
        new_linear->a = smartInputFloat(false);
        printf("Podaj wartosc parametru b:\n");
        new_linear->b = smartInputFloat(false);
    } else printf("Blad! Wskaznik NULL dla funkcji dodawania p. procesow\n");
}

void addParamsExpo(mono_struct* new_expo) {
    if (new_expo != NULL) {
        printf("f(t) = a^t * b\nPodaj wartosc parametru a:\n");
        new_expo->a = smartInputFloat(false);
        printf("Podaj wartosc parametru b:\n");
        new_expo->b = smartInputFloat(false);
    } else printf("Blad! Wskaznik NULL dla funkcji dodawania p. procesow\n");
}

void addParamsPower(mono_struct* new_power) {
    if (new_power != NULL) {
        printf("f(t) = t^a * b\nPodaj wartosc parametru a:\n");
        new_power->a = smartInputFloat(false);
        printf("Podaj wartosc parametru b:\n");
        new_power->b = smartInputFloat(false);
    } else printf("Blad! Wskaznik NULL dla funkcji dodawania p. procesow\n");
}

void addParamsLog(mono_struct* new_log) {
    if (new_log != NULL) {
        printf("f(t) = log_a(t)\nPodaj wartosc parametru a:\n");
        new_log->a = smartInputFloat(false);
    } else printf("Blad! Wskaznik NULL dla funkcji dodawania p. procesow\n");
}

void addParamsPoly(poly_struct* new_poly) {
    if (new_poly != NULL) {
        printf("f(t) = a_n * x^n + ... + a_1 * x + a_0\n"
               "Podaj liczbe parametrow n (n = stopien wielomianu - 1, od 1 do 7):\n");
        int poly_order = 8;
        bool highest_order_declared = false;
        float value = 0;
        while (poly_order > 7) {
            poly_order = smartInputInt(true);
            switch (poly_order) {
                /* UWAGA! Tutaj umy¶lnie nie ma break'ów.
                 * Po definicji wy¿szych stopniem wyrazów wielomianu i tak trzeba podaæ te mniejsze
                 * wiêc bez break'ów switch skoczy do najwy¿szego stopniem wyrazu i poda kolejne
                 * w ten sposób mo¿na upro¶ciæ kod przy zachowaniu funkcjonalno¶ci*/
                case 7:
                    printf("Podaj wartosc parametru a_6 (oprocz 0):\n");
                    value = 0;
                    while(!value) {
                        value = smartInputFloat(false);
                        if (value) {
                            new_poly->a_6 = value;
                        } else printf("Nie moze byc rowna 0!\n");
                    }
                    highest_order_declared = true;
                case 6:
                    if (highest_order_declared) {
                        printf("Podaj wartosc parametru a_5 lub wpisz 0 by go pominac:\n");
                        new_poly->a_5 = smartInputFloat(false);
                    } else {
                        highest_order_declared = true;
                        new_poly->a_6 = 0.0f;
                        printf("Podaj wartosc parametru a_5 (oprocz 0):\n");
                        value = 0;
                        while(!value) {
                            value = smartInputFloat(false);
                            if (value) {
                                new_poly->a_5 = value;
                            } else printf("Nie moze byc rowna 0!\n");
                        }
                    }
                case 5:
                    if (highest_order_declared) {
                        printf("Podaj wartosc parametru a_4 lub wpisz 0 by go pominac:\n");
                        new_poly->a_4 = smartInputFloat(false);
                    } else {
                        highest_order_declared = true;
                        new_poly->a_6 = 0.0f; new_poly->a_5 = 0.0f;
                        printf("Podaj wartosc parametru a_4 (oprocz 0):\n");
                        value = 0;
                        while(!value) {
                            value = smartInputFloat(false);
                            if (value) {
                                new_poly->a_4 = value;
                            } else printf("Nie moze byc rowna 0!\n");
                        }
                    }
                case 4:
                    if (highest_order_declared) {
                        printf("Podaj wartosc parametru a_3 lub wpisz 0 by go pominac:\n");
                        new_poly->a_3 = smartInputFloat(false);
                    } else {
                        highest_order_declared = true;
                        new_poly->a_6 = 0.0f; new_poly->a_5 = 0.0f; new_poly->a_4 = 0.0f;
                        printf("Podaj wartosc parametru a_3 (oprocz 0):\n");
                        value = 0;
                        while(!value) {
                            value = smartInputFloat(false);
                            if (value) {
                                new_poly->a_3 = value;
                            } else printf("Nie moze byc rowna 0!\n");
                        }
                    }
                case 3:
                    if (highest_order_declared) {
                        printf("Podaj wartosc parametru a_2 lub wpisz 0 by go pominac:\n");
                        new_poly->a_2 = smartInputFloat(false);
                    } else {
                        highest_order_declared = true;
                        new_poly->a_6 = 0.0f; new_poly->a_5 = 0.0f; new_poly->a_4 = 0.0f;
                        new_poly->a_3 = 0.0f;
                        printf("Podaj wartosc parametru a_2 (oprocz 0):\n");
                        value = 0;
                        while(!value) {
                            value = smartInputFloat(false);
                            if (value) {
                                new_poly->a_2 = value;
                            } else printf("Nie moze byc rowna 0!\n");
                        }
                    }
                case 2:
                    if (highest_order_declared) {
                        printf("Podaj wartosc parametru a_1 lub wpisz 0 by go pominac:\n");
                        new_poly->a_1 = smartInputFloat(false);
                    } else {
                        highest_order_declared = true;
                        new_poly->a_6 = 0.0f; new_poly->a_5 = 0.0f; new_poly->a_4 = 0.0f;
                        new_poly->a_3 = 0.0f; new_poly->a_2 = 0.0f;
                        printf("Podaj wartosc parametru a_1 (oprocz 0):\n");
                        value = 0;
                        while(!value) {
                            value = smartInputFloat(false);
                            if (value) {
                                new_poly->a_1 = value;
                            } else printf("Nie moze byc rowna 0!\n");
                        }
                    }
                case 1:
                    if (highest_order_declared) {
                        printf("Podaj wartosc parametru a_0 lub wpisz 0 by go pominac:\n");
                        new_poly->a_0 = smartInputFloat(false);
                    } else {
                        highest_order_declared = true;
                        new_poly->a_6 = 0.0f; new_poly->a_5 = 0.0f; new_poly->a_4 = 0.0f;
                        new_poly->a_3 = 0.0f; new_poly->a_2 = 0.0f; new_poly->a_1 = 0.0f;
                        printf("Podaj wartosc parametru a_0 (oprocz 0):\n");
                        value = 0;
                        while(!value) {
                            value = smartInputFloat(false);
                            if (value) {
                                new_poly->a_0 = value;
                            } else printf("Nie moze byc rowna 0!\n");
                        }
                    }
                    break;
                default:
                    printf("Podaj wartosc z zakresu od 1 do 7!\n"); break;
            }
        }

    } else printf("Blad! Wskaznik NULL dla funkcji dodawania p. procesow\n");
}

void addNoiseMono(mono_struct* new_mono) {
    if (new_mono != NULL) {
        printf("Podaj wartosc oczekiwana:\n");
        new_mono->expected_value = smartInputFloat(false);
        printf("Podaj odchylenie standardowe:\n");
        new_mono->std_deviation = smartInputFloat(false);
    } else printf("Blad! Wskaznik NULL dla funkcji dodawania p. zaklocen\n");
}

void addNoisePoly(poly_struct* new_poly) {
    if (new_poly != NULL) {
        printf("Podaj wartosc oczekiwana:\n");
        new_poly->expected_value = smartInputFloat(false);
        printf("Podaj odchylenie standardowe:\n");
        new_poly->std_deviation = smartInputFloat(false);
    } else printf("Blad! Wskaznik NULL dla funkcji dodawania p. zaklocen\n");
}

void addTimeMono(mono_struct* new_mono) {
    if (new_mono != NULL) {
        printf("Podaj okres symulacji:\n");
        new_mono->time = smartInputFloat(true);
        printf("Podaj czestotliwosc (w Hz):\n");
        new_mono->frequency = smartInputFloat(true);
        new_mono->array_size = (int) ceilf(new_mono->time * new_mono->frequency);
        printf("Podaj punkt startowy sumulacji (w sekundach, 0+):\n");
        float start_point = -1.0f;
        while (start_point < 0) {
            start_point = smartInputFloat(false);
            if (start_point >= 0) new_mono->time_start_point = start_point;
            else printf("Punkt musi byc liczba nieujemna!\n");
        }
    } else printf("Blad! Wskaznik NULL dla funkcji dodawania p. czasu\n");
}

void addTimePoly(poly_struct* new_poly) {
    if (new_poly != NULL) {
        printf("Podaj okres symulacji:\n");
        new_poly->time = smartInputFloat(true);
        printf("Podaj czestotliwosc (w Hz):\n");
        new_poly->frequency = smartInputFloat(true);
        new_poly->array_size = (int) ceilf(new_poly->time * new_poly->frequency);
        printf("Podaj punkt startowy sumulacji (w sekundach, 0+):\n");
        float start_point = -1.0f;
        while (start_point < 0) {
            start_point = smartInputFloat(false);
            if (start_point >= 0) new_poly->time_start_point = start_point;
            else printf("Punkt musi byc liczba nieujemna!\n");
        }

    } else printf("Blad! Wskaznik NULL dla funkcji dodawania p. czasu\n");

}

void funcCalcLinear(mono_struct* new_linear) {
    new_linear->values = (float*) malloc(new_linear->array_size * sizeof(float));
    for (int index = 0; index < new_linear->array_size; index++) {
        new_linear->values[index] = setPrecision(new_linear->a * (new_linear->time_start_point + (index
                                                                                                  / new_linear->frequency)) + new_linear->b, export_settings.float_precision);
    }
}

void funcCalcExpo(mono_struct* new_expo) {
    new_expo->values = (float*) malloc(new_expo->array_size * sizeof(float));
    for (int index = 0; index < new_expo->array_size; index++) {
        new_expo->values[index] = setPrecision(powf(new_expo->a, (new_expo->time_start_point + (index
                                                                                                / new_expo->frequency))) * new_expo->b, export_settings.float_precision);
    }
}

void funcCalcPower(mono_struct* new_power) {
    new_power->values = (float*) malloc(new_power->array_size * sizeof(float));
    for (int index = 0; index < new_power->array_size; index++) {
        new_power->values[index] = setPrecision(powf((new_power->time_start_point + (index
                                                                                     / new_power->frequency)), new_power->a) * new_power->b, export_settings.float_precision);
    }
}

void funcCalcLog(mono_struct* new_log) {
    new_log->values = (float*) malloc(new_log->array_size * sizeof(float));
    for (int index = 0; index < new_log->array_size; index++) {
        new_log->values[index] = setPrecision(logf(new_log->time_start_point + (index / new_log->frequency))
                                              / logf(new_log->a), export_settings.float_precision);
    }
}

void funcCalcPoly(poly_struct* new_poly) {
    new_poly->values = (float*) malloc(new_poly->array_size * sizeof(float));
    for (int index = 0; index < new_poly->array_size; index++) {
        new_poly->values[index] = setPrecision(
                (new_poly->a_6 ? new_poly->a_6 * powf(new_poly->time_start_point + (index / new_poly->frequency), 6) : 0.0f)
                + (new_poly->a_5 ? new_poly->a_5 * powf(new_poly->time_start_point + (index / new_poly->frequency), 5) : 0.0f)
                + (new_poly->a_4 ? new_poly->a_4 * powf(new_poly->time_start_point + (index / new_poly->frequency), 4) : 0.0f)
                + (new_poly->a_3 ? new_poly->a_3 * powf(new_poly->time_start_point + (index / new_poly->frequency), 3) : 0.0f)
                + (new_poly->a_2 ? new_poly->a_2 * powf(new_poly->time_start_point + (index / new_poly->frequency), 2) : 0.0f)
                + (new_poly->a_1 ? new_poly->a_1 * (new_poly->time_start_point + (index / new_poly->frequency)) : 0.0f)
                + new_poly->a_0, export_settings.float_precision);
    }
}

void generateNoiseMonoSum(mono_struct* new_mono) {
    double y1, y2, W, mult, x1, x2;
    bool has_value = false;
    float* noise_values = (float*) malloc(new_mono->array_size * sizeof(float));
    new_mono->noise_values = noise_values;
    for (int index = 0; index < new_mono->array_size; index++) {
        if (has_value) { // je¶li ma warto¶æ z poprzedniego przebiegu wykorzystuje j±
            has_value = !has_value;
            new_mono->noise_values[index] = setPrecision(new_mono->values[index] + (new_mono->expected_value
                                            + new_mono->std_deviation * (float) x2), export_settings.float_precision);
            continue; // powy¿ej jest zsumowanie warto¶ci bazowej i losowej/szumu
        }
        do { // a je¶li nie, to j± generuje
            y1 = -1 + ((double) rand() / RAND_MAX) * 2;
            y2 = -1 + ((double) rand() / RAND_MAX) * 2;
            W = pow(y1, 2) + pow(y2, 2);
        } while (W >= 1 || W == 0);
        mult = sqrt((-2 * log(W)) / W);
        x1 = y1 * mult;
        x2 = y2 * mult;
        has_value = !has_value;
        new_mono->noise_values[index] = setPrecision(new_mono->values[index] + (new_mono->expected_value
                                            + new_mono->std_deviation * (float) x1), export_settings.float_precision);
    }
}

void generateNoisePolySum(poly_struct *new_poly) { // dzia³anie analogicznie jak wy¿ej
    double y1, y2, W, mult, x1, x2;
    bool has_value = false;
    float* noise_values = (float*) malloc(new_poly->array_size * sizeof(float));
    new_poly->noise_values = noise_values;
    for (int index = 0; index < new_poly->array_size; index++) {
        if (has_value) {
            has_value = !has_value;
            new_poly->noise_values[index] = setPrecision(new_poly->values[index] + (new_poly->expected_value
                                            + new_poly->std_deviation * (float) x2), export_settings.float_precision);
            continue;
        }
        do {
            y1 = -1 + ((double) rand() / RAND_MAX) * 2;
            y2 = -1 + ((double) rand() / RAND_MAX) * 2;
            W = pow(y1, 2) + pow(y2, 2);
        } while (W >= 1 || W == 0);
        mult = sqrt((-2 * log(W)) / W);
        x1 = y1 * mult;
        x2 = y2 * mult;
        has_value = !has_value;
        new_poly->noise_values[index] = setPrecision(new_poly->values[index] + (new_poly->expected_value
                                            + new_poly->std_deviation * (float) x1), export_settings.float_precision);
    }
}

void generateLinear(mono_struct* new_linear) {
    if (new_linear != NULL) {
        funcCalcLinear(new_linear);
        if (menu_flags.debug_mode) debugPrint(new_linear->values, new_linear->array_size);
        generateNoiseMonoSum(new_linear);
        if (menu_flags.debug_mode) debugPrint(new_linear->noise_values, new_linear->array_size);
    } else printf("Blad! Wskaznik NULL przekazany funkcji eksportu\n");
}

void generateExpo(mono_struct* new_expo) {
    if (new_expo != NULL) {
        funcCalcExpo(new_expo);
        if (menu_flags.debug_mode) debugPrint(new_expo->values, new_expo->array_size);
        generateNoiseMonoSum(new_expo);
        if (menu_flags.debug_mode) debugPrint(new_expo->noise_values, new_expo->array_size);
    } else printf("Blad! Wskaznik NULL dla funkcji eksportu\n");
}

void generatePower(mono_struct* new_power) {
    if (new_power != NULL) {
        funcCalcPower(new_power);
        if (menu_flags.debug_mode) debugPrint(new_power->values, new_power->array_size);
        generateNoiseMonoSum(new_power);
        if (menu_flags.debug_mode) debugPrint(new_power->noise_values, new_power->array_size);
    } else printf("Blad! Wskaznik NULL dla funkcji eksportu\n");
}

void generateLog(mono_struct* new_log) {
    if (new_log != NULL) {
        funcCalcLog(new_log);
        if (menu_flags.debug_mode) debugPrint(new_log->values, new_log->array_size);
        generateNoiseMonoSum(new_log);
        if (menu_flags.debug_mode) debugPrint(new_log->noise_values, new_log->array_size);
    } else printf("Blad! Wskaznik NULL przekazany funkcji eksportu\n");
}

void generatePoly(poly_struct *new_poly) {
    if (new_poly != NULL) {
        funcCalcPoly(new_poly);
        if (menu_flags.debug_mode) debugPrint(new_poly->values, new_poly->array_size);
        generateNoisePolySum(new_poly);
        if (menu_flags.debug_mode) debugPrint(new_poly->noise_values, new_poly->array_size);
    } else printf("Blad! Wskaznik NULL przekazany funkcji eksportu\n");
}
