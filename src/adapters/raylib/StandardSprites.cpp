#include "adapters/raylib/StandardSprites.hpp"

#include "adapters/raylib/PixelSprite.hpp"
#include "domain/StandardPieces.hpp"

namespace chess::adapters::raylib {

namespace {

using domain::StandardPieces;

PixelSprite pawnSprite() {
    return PixelSprite({
        "................",
        "................",
        "................",
        "......oooo......",
        ".....offfso.....",
        ".....offfso.....",
        ".....offfso.....",
        "......offo......",
        "....offffsso....",
        "....oooooooo....",
        "......ofso......",
        ".....offfso.....",
        "....offffsso....",
        "...offfffssso...",
        "..oooooooooooo..",
        "................",
    });
}

PixelSprite rookSprite() {
    return PixelSprite({
        "................",
        "..ooo.oooo.ooo..",
        "..ofoooffoooso..",
        "..offffffffsso..",
        "..oooooooooooo..",
        "...offffffsso...",
        "....offffsso....",
        "....offffsso....",
        "....offffsso....",
        "....offffsso....",
        "....offffsso....",
        "...offfffssso...",
        "..oooooooooooo..",
        "..offffffffsso..",
        "..oooooooooooo..",
        "................",
    });
}

PixelSprite knightSprite() {
    return PixelSprite({
        "................",
        "......oo.o......",
        ".....offoso.....",
        "....offffsso....",
        "...offoffffso...",
        "..offfffffffso..",
        ".offfffffffffso.",
        ".offffoooffffso.",
        "..ooo..offfffso.",
        "......offffffso.",
        ".....offffffso..",
        "....offffffsso..",
        "..offffffffsso..",
        "..oooooooooooo..",
        "..offffffffsso..",
        "..oooooooooooo..",
    });
}

PixelSprite bishopSprite() {
    return PixelSprite({
        "................",
        ".......oo.......",
        "......offo......",
        ".....offfso.....",
        "....offfosso....",
        "....offoffso....",
        "....offffsso....",
        ".....offfso.....",
        "....oooooooo....",
        ".....offfso.....",
        ".....offfso.....",
        "....offffsso....",
        "...offfffssso...",
        "..offffffsssso..",
        "..oooooooooooo..",
        "................",
    });
}

PixelSprite queenSprite() {
    return PixelSprite({
        "................",
        ".o.....oo.....o.",
        ".oo...offo...oo.",
        ".ofo..offo..oso.",
        ".offooffffoosso.",
        ".offffffffffsso.",
        "..offffffffsso..",
        "...offffffsso...",
        "...oooooooooo...",
        "....offffsso....",
        "....offffsso....",
        "...offfffssso...",
        "..offffffsssso..",
        "..oooooooooooo..",
        "..offffffffsso..",
        "..oooooooooooo..",
    });
}

PixelSprite kingSprite() {
    return PixelSprite({
        "......oooo......",
        "....ooofsooo....",
        "....offffsso....",
        "....ooofsooo....",
        "......ofso......",
        "...oooofsoooo...",
        "..offffffffsso..",
        ".offffffffffsso.",
        ".offffffffffsso.",
        "..offffffffsso..",
        "...offffffsso...",
        "...oooooooooo...",
        "...offffffsso...",
        "..offffffffsso..",
        "..oooooooooooo..",
        "................",
    });
}

}

void StandardSprites::registerAll(SpriteCatalog& catalog) {
    catalog.add(StandardPieces::kPawn, pawnSprite());
    catalog.add(StandardPieces::kRook, rookSprite());
    catalog.add(StandardPieces::kKnight, knightSprite());
    catalog.add(StandardPieces::kBishop, bishopSprite());
    catalog.add(StandardPieces::kQueen, queenSprite());
    catalog.add(StandardPieces::kKing, kingSprite());
}

}
