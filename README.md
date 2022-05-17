# Not-Carlsen
Successor of the Not-Magnus chess engine, now written in C(arlsen). Whereas Not-Magnus's goal was to learn the popular algorithims and techniques in modern chess engines, this engine will efficently implement them. Thus, the goal of this engine is simply to be strong while providing me an opportunity to solidify my C programming skills.

Special thanks to:
- github.com/nkarve for their magic bitboard generation
- Dominic Hofer and Peter Ellis Jone's legal move generation guides

------

## Current Features
- FEN board initialization
- Magic bitboard sliding move generation

------

## Devlog
5/17/22 v0.1.5
> Began legal move generator by implementing checkmasks (a bitboard which restricts the legal moves when in check).

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
