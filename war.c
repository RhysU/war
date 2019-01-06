#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcg_basic.h"

// For War only the face value is of interest hence one scalar
typedef uint16_t card_t;

// Common playing card conventions
static const size_t FACES = 13;
static const size_t SUITS = 4;

// Output a card value as something humans might recognize
void card_putchar(card_t c) {
    putchar("234567890JQKA"[c]);
}

// Convention is that that card[len-1] is the top
typedef struct deck_t {
    size_t len;
    card_t card[0];
} deck_t;

// Malloc a new deck which the caller must free
deck_t* deck_new() {
    deck_t *t = malloc(sizeof(deck_t) + SUITS * FACES * sizeof(card_t));
    t->len = SUITS * FACES;
    for (size_t i = 0; i < SUITS; ++i) {
        for (size_t j = 0; j < FACES; ++j) {
            t->card[i + j*SUITS] = j;
        }
    }
    return t;
}

// Make the deck have length zero
deck_t* deck_clear(deck_t *d) {
    d->len = 0;
    return d;
}

// Print a deck onto stdout
void deck_putchar(deck_t *d) {
    for (size_t i = d->len; i --> 0;) {
        card_putchar(d->card[i]);
    }
}

// Shuffle a deck with Fisher--Yates
void deck_shuffle(pcg32_random_t *rng, deck_t *d) {
    size_t n = d->len;
    while ( n > 1 ) {
        size_t k = pcg32_boundedrand_r(rng, n--);
        card_t t = d->card[n];
        d->card[n] = d->card[k];
        d->card[k] = t;
    }
}

// Split src into decks A and B, allowing src to be deck A.
void deck_split(deck_t *src, size_t where, deck_t *a, deck_t *b) {
    assert(where <= src->len);
    size_t n = src->len; // As src may alias a
    a->len = where;
    b->len = n - a->len;
    memmove(a->card, src->card, a->len * sizeof(card_t));
    memmove(b->card, &src->card[a->len], b->len * sizeof(card_t));
}

// Draw next card from the (p)lay pile into *out returning true.
// Otherwise shuffle the (d)iscard pile into (p)lay then draw.
// A NULL (d)iscard pile indicates to only use the (p)lay pile.
// Return false whenever no more cards available leaving out undefined.
bool deck_next(pcg32_random_t *rng, deck_t *p, deck_t *d, card_t *out) {
    if (p->len) {
        *out = p->card[--p->len];
        return true;
    }

    if (d && d->len) {
        deck_shuffle(rng, d);
        deck_split(d, d->len, p, d);
        *out = p->card[--p->len];
        return true;
    }

    return false;
}


// Add a card to the top of a deck
void deck_add(deck_t *d, card_t top) {
    assert(d->len <= FACES * SUITS);
    d->card[d->len++] = top;
}


// Sort a deck such that higher cards will be drawn first
// Adapted from http://www.codecodex.com/wiki/Heapsort
void deck_sort(deck_t *d) {
    card_t * const arr = d->card, t;
    size_t n = d->len, i = n / 2;

    for (;;) {
        if (i > 0) {
            t = arr[--i];
        } else {
            if (n-- <= 1) return;  // Allows zero-length input!
            t = arr[n];
            arr[n] = arr[0];
        }

        size_t parent = i;
        size_t child = i*2 + 1;

        while (child < n) {
            if (child + 1 < n  &&  arr[child + 1] > arr[child]) {
                ++child;
            }
            if (arr[child] > t) {
                arr[parent] = arr[child];
                parent = child;
                child = parent*2 + 1;
            } else {
                break;
            }
        }
        arr[parent] = t;
    }
}


// Return negative if (a) wins, 0 if a tie, positive if (b) wins.
// Provided decks are permuted but all cards are retained.
int war_winner(deck_t *a, deck_t *b) {
    deck_sort(a);
    deck_sort(b);

    size_t na = a->len, nb = b->len;

    while (na > 0 && nb > 0) {
        card_t ca = a->card[na - 1], cb = b->card[nb - 1];
        if (ca >= cb) {
            --nb;
        }
        if (cb >= ca) {
            --na;
        }
    }

    return nb - na;
}

int main(int argc, char **argv) {
    // Simple positional parsing
    const int warcards = argc > 1 ? atoi(argv[1]) : 4;
    const int pcg32seq = argc > 2 ? atoi(argv[2]) : 0;
    assert(warcards > 0);

    // Seed the random number generator
    pcg32_random_t r[1];
    pcg32_srandom_r(r, 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL + pcg32seq);

    // State consists of two (p)layers and two (d)iscards.
    // A shuffled initial deck is divided between the players.
    deck_t *p1 = deck_new(), *p2 = deck_new();
    deck_shuffle(r, p1);
    deck_split(p1, p1->len / 2, p1, p2);

    // Discard piles are initially empty
    deck_t *d1 = deck_clear(deck_new());
    deck_t *d2 = deck_clear(deck_new());

    // War piles are initially empty
    deck_t *w1 = deck_clear(deck_new());
    deck_t *w2 = deck_clear(deck_new());

    // Main loop
    card_t c1, c2;
    while (deck_next(r, p1, d1, &c1) && deck_next(r, p2, d2, &c2)) {

        if (c1 > c2) {  // Player 1 wins

            deck_add(d1, c1);
            deck_add(d1, c2);

        } else if (c2 > c1) {  // Player 2 wins

            deck_add(d2, c1);
            deck_add(d2, c2);

        } else {  // Tie

            // War involves drawing some number of cards...
            card_t t;
            for (size_t i = 0; i < warcards && deck_next(r, p1, d1, &t); ++i) {
                deck_add(w1, t);
            }
            for (size_t i = 0; i < warcards && deck_next(r, p2, d2, &t); ++i) {
                deck_add(w2, t);
            }

            // ...then dividing any spoils per its outcome:
            int result = war_winner(w1, w2);
            if (result < 0) {  // Player 1 wins

                deck_add(d1, c1);
                deck_add(d1, c2);
                while (deck_next(r, w1, NULL, &t)) deck_add(d1, t);
                while (deck_next(r, w2, NULL, &t)) deck_add(d1, t);

            } else if (result > 0) {  // Player 2 wins

                deck_add(d2, c1);
                deck_add(d2, c2);
                while (deck_next(r, w1, NULL, &t)) deck_add(d2, t);
                while (deck_next(r, w2, NULL, &t)) deck_add(d2, t);

            } else { // Tie (wildly unlikely)

                deck_add(d1, c1);
                while (deck_next(r, w1, NULL, &t)) deck_add(d1, t);
                deck_add(d2, c2);
                while (deck_next(r, w2, NULL, &t)) deck_add(d2, t);
            }
        }

        deck_putchar(p1);
        putchar(',');
        deck_putchar(p2);
        putchar(',');
        deck_putchar(d1);
        putchar(',');
        deck_putchar(d2);
        putchar('\n');
    }

    free(p1);
    free(p2);
    free(d1);
    free(d2);
    free(w1);
    free(w2);
}
