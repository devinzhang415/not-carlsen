# Not-Carlsen
Successor of the Not-Magnus chess engine, now written in C(arlsen). Whereas Not-Magnus's goal was to learn the popular algorithims and techniques in modern chess engines, this engine will efficently implement them. Thus, the goal of this engine is simply to be strong while providing me an opportunity to solidify my C programming skills.

Special thanks to:
- Github.com/nkarve for their magic bitboard generation

------

## Current Features
- FEN board initialization
- Magic bitboard sliding move generation
- Pseudolegal move generator (6.5 million nps)

------

## Devlog
6/21/22 v0.3
> Added threefold repetition and 50-move rule detection.

6/15/22 v0.2.6
> Added zobrist hash to the position.

6/10/22 v0.2.5
> Continuing to squash perft bugs:
>
> Duplicate moves caused by an improper last move flag in the pseudo-legal move list due to a reliance on default uninitialized values.
>
> Missing castling moves for black as the move legality was checking for the existence of white rooks instead of black.
>
> Undefined behavior due to oversight where promotion moves that were also captures were not being properly considered captures.
>
> Undefined behavior due to knight promotions mistakingly being marked 'K' instead of 'N'.
>
> Several positions still have errors, but default position has no errors up to depth 7 (3.2 billion nodes). First speed test clocks in at ~6.5 million nodes per second.

6/8/22 v0.2.4
> Fixed major perft bugs by using a non-static pseudo-legal move array. In recurisve perft calls, said array was erroneously being changed mid-call such that the array would be different after a recursive call and keep the changes after the sub-function finished executing.
> 
> Perft errors reduced to ~10%.

6/7/22 v0.2.3
> Fixed bug with move generation where the victim piece in captures was incorrectly tagged due to it being checked after the attacker square was updated.
>
> Continuing to run into error where all "perft 1" numbers match the unit tests, but deeper depths give results larger than what they should be.

6/6/22 v0.2.2
> Started perft testing with the move generator. This determines the accuracy of the move generation as well as the speed at different depths.
>
> Added function to determine if king is in check.

6/5/22 v0.2.1
> Changed make/unmake to make/copy for move pushing and popping.

6/2/22 v0.2
> Implemented a pseudolegal move generator and move stack.
>
> Changed pawn move generator from setwise to individual.

5/9/22 v0.1.4
> Added pawn move (single push, double push, capture, and en passant) wrapper functions.

4/30/22 v0.1.3
> Added wrapper functions for the move generation of all pieces besides pawns.
>
> Board structure now includes a "mailbox" representation of the board to easily determine what piece is on a given square.

4/27/22 v0.1.2
> Implemented sliding move generation using magic bitboards. Credit to github.com/nkarve.

3/27/22 v0.1.1
> Defined bitboard constants for all squares, files, and ranks.
>
> Defined bit attack maps for knights, bishops, and rooks from all squares.

3/26/22 v0.1
> Implemented complete board initialization.

3/22/22 v0.0.1
> Began writing board representation "class".
> 
> Given a FEN string, function will pull information regarding turn, castling rights, en passant squares, halfmove clock, and fullmove number. Bitboard parsing not yet implemented.
