// MODULES / UTILS / PRINT.C

#include "print.h" // modules/utils/print.h
#include "../globals.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "../_libs/linenoise/linenoise.h"


/*
Entiers (Nombres entiers)
%d ou %i : Entier signé (int). Affiche les nombres négatifs avec un signe moins.
%u : Entier non signé (unsigned int). Affiche uniquement des nombres positifs (gère les grands nombres positifs si l'entier est signé mais contient une valeur binaire élevée).
%x ou %X : Entier en hexadécimal (base 16). x pour minuscules (a-f), X pour majuscules (A-F). Très utile pour voir les adresses ou les bits.
%o : Entier en octal (base 8).
%ld, %lu, %llx : Versions pour les types long ou long long.
Nombres à virgule (Flottants)
%f : Nombre décimal standard (float, double). Affiche 6 chiffres après la virgule par défaut.
%e ou %E : Notation scientifique (ex: 1.23e+02).
%g : Choisit automatiquement le format le plus court entre %f et %e.
%.2f : Permet de préciser la précision (ici, 2 chiffres après la virgule).
Caractères et Chaînes
%c : Caractère unique (char).
%s : Chaîne de caractères (char * ou tableau de char). S'arrête au caractère nul \0.
Adresses et Pointeurs
%p : Pointeur (adresse mémoire). Important : Il faut toujours caster le pointeur en (void *) avant de l'afficher (ex: printf("%p", (void*)ptr);).
Autres
%% : Affiche le caractère pourcentage % lui-même (car % est le caractère d'échappement).
*/

void _utils_printf(const char *err, const char* string, ...) {
  if (linenoise_state.ofd != 0) { linenoiseHide(&linenoise_state); }
  va_list args;
  va_start(args, string);
  if (err) {
    fprintf(stderr, "%s", TERMINAL_STYLE_RED);
    vfprintf(stderr, string, args); fflush(stderr);
    fprintf(stderr, "%s", TERMINAL_STYLE_RESET);
  } else {
    vprintf(string, args); fflush(stdout);
  }
  va_end(args);
  if (linenoise_state.ofd != 0) { linenoiseShow(&linenoise_state); }
}

void _utils_print_ln(const char* string, ...) {
  va_list args;
  va_start(args, string);
  vprintf(string, args);
  _utils_printf(NULL, "\n");
  va_end(args);
}


