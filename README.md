# war

This program simulates [the card game
War](https://en.wikipedia.org/wiki/War_(card_game)), where two
players flip cards and the higher card takes both.  Ties trigger a
"war" in which each player commits a configurable number of
additional cards.  The simulation emits per-round state as CSV, and
the Makefile sweeps war sizes 1 through 6 across 1000 seeds each
for Monte Carlo analysis.

The implementation is plain C99 with hand-rolled data structures
and the PCG32 generator for reproducible randomness.  Decks are
flexible array member structs with stack-like semantics, cards are
bare face values since suits are irrelevant, and each player
maintains separate play and discard piles that reshuffle on demand.
Shuffling is Fisher-Yates, war resolution uses heapsort to compare
committed cards by rank, and all decks are malloc'd once at startup
and reused throughout.
