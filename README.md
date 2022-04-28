# Not-Carlsen
Successor of the Not-Magnus chess engine, now written in C(arlsen). Whereas Not-Magnus's goal was to learn the popular algorithims and techniques in modern chess engines, this engine will efficently implement them. Thus, the goal of this engine is simply to be strong while providing me an opportunity to solidify my C programming skills.

------

## Current Features
- FEN board initialization
- Magic bitboard sliding move generation

------

## Devlog
4/27/22 v0.1.2
> Implemented sliding move generation using magic bitboards.

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
