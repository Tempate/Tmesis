#include <assert.h>

#include "headers/board.h"
#include "headers/magic.h"

static uint64_t maskConfiguration(uint64_t mask, int configuration);

static void generateBishMagics(void);
static void generateRookMagics(void);

static uint64_t diagonalMoves(const int sqr, const uint64_t obstacles);
static uint64_t straightMoves(const int sqr, const uint64_t obstacles);

static uint64_t rayAttacks(const int sqr, const uint64_t obstacles, const int dir, int bitScan(uint64_t));

static uint64_t findMagic(const int sqr, const int piece);

static inline uint64_t randomU64() { return (((uint64_t) rand()) << 33) | rand(); }


const uint64_t diagonalMasks[64] = {0x40201008040200, 0x402010080400, 0x4020100a00, 0x40221400, 0x2442800, 0x204085000, 0x20408102000, 0x2040810204000, 0x20100804020000, 0x40201008040000, 0x4020100a0000, 0x4022140000, 0x244280000, 0x20408500000, 0x2040810200000, 0x4081020400000, 0x10080402000200, 0x20100804000400, 0x4020100a000a00, 0x402214001400, 0x24428002800, 0x2040850005000, 0x4081020002000, 0x8102040004000, 0x8040200020400, 0x10080400040800, 0x20100a000a1000, 0x40221400142200, 0x2442800284400, 0x4085000500800, 0x8102000201000, 0x10204000402000, 0x4020002040800, 0x8040004081000, 0x100a000a102000, 0x22140014224000, 0x44280028440200, 0x8500050080400, 0x10200020100800, 0x20400040201000, 0x2000204081000, 0x4000408102000, 0xa000a10204000, 0x14001422400000, 0x28002844020000, 0x50005008040200, 0x20002010080400, 0x40004020100800, 0x20408102000, 0x40810204000, 0xa1020400000, 0x142240000000, 0x284402000000, 0x500804020000, 0x201008040200, 0x402010080400, 0x2040810204000, 0x4081020400000, 0xa102040000000, 0x14224000000000, 0x28440200000000, 0x50080402000000, 0x20100804020000, 0x40201008040200};
const uint64_t straightMasks[64] = {0x101010101017e, 0x202020202027c, 0x404040404047a, 0x8080808080876, 0x1010101010106e, 0x2020202020205e, 0x4040404040403e, 0x8080808080807e, 0x1010101017e00, 0x2020202027c00, 0x4040404047a00, 0x8080808087600, 0x10101010106e00, 0x20202020205e00, 0x40404040403e00, 0x80808080807e00, 0x10101017e0100, 0x20202027c0200, 0x40404047a0400, 0x8080808760800, 0x101010106e1000, 0x202020205e2000, 0x404040403e4000, 0x808080807e8000, 0x101017e010100, 0x202027c020200, 0x404047a040400, 0x8080876080800, 0x1010106e101000, 0x2020205e202000, 0x4040403e404000, 0x8080807e808000, 0x1017e01010100, 0x2027c02020200, 0x4047a04040400, 0x8087608080800, 0x10106e10101000, 0x20205e20202000, 0x40403e40404000, 0x80807e80808000, 0x17e0101010100, 0x27c0202020200, 0x47a0404040400, 0x8760808080800, 0x106e1010101000, 0x205e2020202000, 0x403e4040404000, 0x807e8080808000, 0x7e010101010100, 0x7c020202020200, 0x7a040404040400, 0x76080808080800, 0x6e101010101000, 0x5e202020202000, 0x3e404040404000, 0x7e808080808000, 0x7e01010101010100, 0x7c02020202020200, 0x7a04040404040400, 0x7608080808080800, 0x6e10101010101000, 0x5e20202020202000, 0x3e40404040404000, 0x7e80808080808000};

// These keys have been pre-generated by the code.
uint64_t bishMagic[64] = {0x40a004010410300, 0x400405002202140c, 0x4204540022000040, 0x60a082042004242, 0x200424440410a701, 0x3608030c03400010, 0x1412204402010, 0x424050020104, 0x4400644022080, 0x4000480025140820, 0x22009010404104a0, 0x1004082024200060, 0x452c0061000002, 0x104408014808020, 0x8645008020580, 0x20490210410c10, 0x1484804010140, 0x8004610810108022, 0x20008c1218001104, 0x183400808042003, 0x8002080010840011, 0x90014008108a8400, 0x41400240884022, 0x2068080011044204, 0x2212030500240, 0x4401040008164050, 0x908080001000860, 0x20080423004048, 0x101001011004000, 0x432008000102021, 0x4209000101000, 0x2500085000280504, 0x80101044200a0080, 0x5480082000030404, 0x1000100840034804, 0x221040400880120, 0x10802040008d010, 0x10019008322020, 0x50830a01043802, 0x804008a408011100, 0x4800100b008440, 0x4000840836200400, 0x2100202e801000, 0x1101400630930800, 0x1200c002121c00c0, 0x20c8021000b01168, 0x20602081a060008, 0x400140081580c108, 0x210080208031000, 0x401148020020c0, 0x8801008240830, 0x4010000202022050, 0x2080084044090008, 0x4006001044108, 0x98010808004000, 0x209080064005001, 0x202002023040080, 0xa11088600900c200, 0xa014100a0808, 0x841040000c41030, 0x40000020002882, 0x200480650114082, 0x2101204880f80, 0x1104040048001c80};
uint64_t rookMagic[64] = {0x80002040008010, 0x40200040001000, 0x401000400204060, 0x50042640502800, 0x1a00020008200104, 0x20009200050400, 0x1001201002401468, 0x450000804021000a, 0x2004004400820102, 0x4200104020004a10, 0x4000440010a0001c, 0x1044800800049000, 0x104418070a04, 0x2800824004082006, 0x110100044081e001, 0x1921000200040, 0x100404800110080, 0x840000402200800, 0x10000801200434, 0x44000810024008, 0x1842808102029, 0x8104004000818040, 0x12004904200, 0x90022001c01008, 0x40065a201000c004, 0x1040980004012004, 0x1200080020042000, 0x40820008100450, 0x400460a008004200, 0x105032008008001, 0x2064005001020, 0x6400904200010094, 0x308001010048001, 0x904604000080902, 0x1080c04002040d, 0x1020400205100004, 0x1418140418008040, 0x42018045000103, 0x401c10004401101, 0x4001100800a00042, 0x8050014009001, 0x8140010030010080, 0x800480104c801400, 0xc100901000a, 0xc00020004004001, 0x8c000200010300, 0x4840048000411001, 0x9088002848810002, 0x2420108006400012, 0x224003000400092, 0xd410402800200128, 0x180040042002018, 0x1010300800600350, 0x82008040440208, 0x4080810250420400, 0x2a0010000204080, 0x406008420104302, 0x1414182080c001, 0x802401005082101, 0xa0040402489a020e, 0x800211002000106, 0x22010d8422084809, 0x42000210008720d4, 0x8041002412};

uint64_t bishMagicMoves[64][512];
uint64_t rookMagicMoves[64][4096];


uint64_t bishopAttacks(const int sqr, const uint64_t occupied) {
	const uint64_t possibleBlockers = occupied & diagonalMasks[sqr];
	const int index = (possibleBlockers * bishMagic[sqr]) >> BISH_SHIFT;

	return bishMagicMoves[sqr][index];
}

uint64_t xrayBishopAttacks(const int sqr, const uint64_t occupied, const uint64_t myPieces) {
	const uint64_t attacks = bishopAttacks(sqr, occupied);
	const uint64_t blockers = myPieces & attacks;

	return attacks ^ bishopAttacks(sqr, occupied ^ blockers);
}

uint64_t rookAttacks(const int sqr, const uint64_t occupied) {
	const uint64_t possibleBlockers = occupied & straightMasks[sqr];
	const int index = (possibleBlockers * rookMagic[sqr]) >> ROOK_SHIFT;

	return rookMagicMoves[sqr][index];
}

uint64_t xrayRookAttacks(const int sqr, const uint64_t occupied, const uint64_t myPieces) {
	const uint64_t attacks = rookAttacks(sqr, occupied);
	const uint64_t blockers = myPieces & attacks;

	return attacks ^ rookAttacks(sqr, occupied ^ blockers);
}

void initMagics(void) {
    generateBishMagics();
    generateRookMagics();
}

static void generateBishMagics(void) {
	static const int n = 1 << MAX_BISH_BLOCKERS;

    for (int sqr = 0; sqr < 64; ++sqr) {
        const uint64_t mask = diagonalMasks[sqr];

        for (int conf = 0; conf < n; ++conf) {
            const uint64_t state = maskConfiguration(mask, conf);
            const int index = (state * bishMagic[sqr]) >> BISH_SHIFT;

            bishMagicMoves[sqr][index] = diagonalMoves(sqr, state);
        }
    }
}

static void generateRookMagics(void) {
	static const int n = 1 << MAX_ROOK_BLOCKERS;

    for (int sqr = 0; sqr < 64; ++sqr) {
        const uint64_t mask = straightMasks[sqr];

        for (int conf = 0; conf < n; ++conf) {
            const uint64_t state = maskConfiguration(mask, conf);
            const int index = (state * rookMagic[sqr]) >> ROOK_SHIFT;

            rookMagicMoves[sqr][index] = straightMoves(sqr, state);
        }
    }
}

/*
 * This function is used to produce all possible bitboards from a mask.
 * There are 2^bits possible configurations. They are sent as a number
 * which is then converted into a bitboard.
 */
static uint64_t maskConfiguration(uint64_t mask, int configuration) {
    uint64_t result = 0;

    do {
    	if (configuration & 1)
    		result |= lsbBB(mask);

    	configuration >>= 1;
    } while (unsetLSB(mask));

    return result;
}

static uint64_t diagonalMoves(const int sqr, const uint64_t obstacles) {
	return  rayAttacks(sqr, obstacles, NOEA, bitScanForward) |
			rayAttacks(sqr, obstacles, NOWE, bitScanForward) |
			rayAttacks(sqr, obstacles, SOEA, bitScanReverse) |
			rayAttacks(sqr, obstacles, SOWE, bitScanReverse);
}

static uint64_t straightMoves(const int sqr, const uint64_t obstacles) {
	return  rayAttacks(sqr, obstacles, NORT, bitScanForward) |
			rayAttacks(sqr, obstacles, EAST, bitScanForward) |
			rayAttacks(sqr, obstacles, SOUT, bitScanReverse) |
			rayAttacks(sqr, obstacles, WEST, bitScanReverse);
}

static uint64_t rayAttacks(const int sqr, const uint64_t obstacles, const int dir, int bitScan(uint64_t)) {
	// Lookup table for all eight ray-directions: NORT, NOEA, EAST, SOEA, SOUT, SOWE, WEST, NOWE
	static const uint64_t rayLookup[8][64] = {
		{0x101010101010100, 0x202020202020200, 0x404040404040400, 0x808080808080800, 0x1010101010101000, 0x2020202020202000, 0x4040404040404000, 0x8080808080808000, 0x101010101010000, 0x202020202020000, 0x404040404040000, 0x808080808080000, 0x1010101010100000, 0x2020202020200000, 0x4040404040400000, 0x8080808080800000, 0x101010101000000, 0x202020202000000, 0x404040404000000, 0x808080808000000, 0x1010101010000000, 0x2020202020000000, 0x4040404040000000, 0x8080808080000000, 0x101010100000000, 0x202020200000000, 0x404040400000000, 0x808080800000000, 0x1010101000000000, 0x2020202000000000, 0x4040404000000000, 0x8080808000000000, 0x101010000000000, 0x202020000000000, 0x404040000000000, 0x808080000000000, 0x1010100000000000, 0x2020200000000000, 0x4040400000000000, 0x8080800000000000, 0x101000000000000, 0x202000000000000, 0x404000000000000, 0x808000000000000, 0x1010000000000000, 0x2020000000000000, 0x4040000000000000, 0x8080000000000000, 0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x8040201008040200, 0x80402010080400, 0x804020100800, 0x8040201000, 0x80402000, 0x804000, 0x8000, 0x0, 0x4020100804020000, 0x8040201008040000, 0x80402010080000, 0x804020100000, 0x8040200000, 0x80400000, 0x800000, 0x0, 0x2010080402000000, 0x4020100804000000, 0x8040201008000000, 0x80402010000000, 0x804020000000, 0x8040000000, 0x80000000, 0x0, 0x1008040200000000, 0x2010080400000000, 0x4020100800000000, 0x8040201000000000, 0x80402000000000, 0x804000000000, 0x8000000000, 0x0, 0x804020000000000, 0x1008040000000000, 0x2010080000000000, 0x4020100000000000, 0x8040200000000000, 0x80400000000000, 0x800000000000, 0x0, 0x402000000000000, 0x804000000000000, 0x1008000000000000, 0x2010000000000000, 0x4020000000000000, 0x8040000000000000, 0x80000000000000, 0x0, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x0, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000, 0x0, 0xfe0000, 0xfc0000, 0xf80000, 0xf00000, 0xe00000, 0xc00000, 0x800000, 0x0, 0xfe000000, 0xfc000000, 0xf8000000, 0xf0000000, 0xe0000000, 0xc0000000, 0x80000000, 0x0, 0xfe00000000, 0xfc00000000, 0xf800000000, 0xf000000000, 0xe000000000, 0xc000000000, 0x8000000000, 0x0, 0xfe0000000000, 0xfc0000000000, 0xf80000000000, 0xf00000000000, 0xe00000000000, 0xc00000000000, 0x800000000000, 0x0, 0xfe000000000000, 0xfc000000000000, 0xf8000000000000, 0xf0000000000000, 0xe0000000000000, 0xc0000000000000, 0x80000000000000, 0x0, 0xfe00000000000000, 0xfc00000000000000, 0xf800000000000000, 0xf000000000000000, 0xe000000000000000, 0xc000000000000000, 0x8000000000000000, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x0, 0x204, 0x408, 0x810, 0x1020, 0x2040, 0x4080, 0x8000, 0x0, 0x20408, 0x40810, 0x81020, 0x102040, 0x204080, 0x408000, 0x800000, 0x0, 0x2040810, 0x4081020, 0x8102040, 0x10204080, 0x20408000, 0x40800000, 0x80000000, 0x0, 0x204081020, 0x408102040, 0x810204080, 0x1020408000, 0x2040800000, 0x4080000000, 0x8000000000, 0x0, 0x20408102040, 0x40810204080, 0x81020408000, 0x102040800000, 0x204080000000, 0x408000000000, 0x800000000000, 0x0, 0x2040810204080, 0x4081020408000, 0x8102040800000, 0x10204080000000, 0x20408000000000, 0x40800000000000, 0x80000000000000, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x101, 0x202, 0x404, 0x808, 0x1010, 0x2020, 0x4040, 0x8080, 0x10101, 0x20202, 0x40404, 0x80808, 0x101010, 0x202020, 0x404040, 0x808080, 0x1010101, 0x2020202, 0x4040404, 0x8080808, 0x10101010, 0x20202020, 0x40404040, 0x80808080, 0x101010101, 0x202020202, 0x404040404, 0x808080808, 0x1010101010, 0x2020202020, 0x4040404040, 0x8080808080, 0x10101010101, 0x20202020202, 0x40404040404, 0x80808080808, 0x101010101010, 0x202020202020, 0x404040404040, 0x808080808080, 0x1010101010101, 0x2020202020202, 0x4040404040404, 0x8080808080808, 0x10101010101010, 0x20202020202020, 0x40404040404040, 0x80808080808080},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x0, 0x100, 0x201, 0x402, 0x804, 0x1008, 0x2010, 0x4020, 0x0, 0x10000, 0x20100, 0x40201, 0x80402, 0x100804, 0x201008, 0x402010, 0x0, 0x1000000, 0x2010000, 0x4020100, 0x8040201, 0x10080402, 0x20100804, 0x40201008, 0x0, 0x100000000, 0x201000000, 0x402010000, 0x804020100, 0x1008040201, 0x2010080402, 0x4020100804, 0x0, 0x10000000000, 0x20100000000, 0x40201000000, 0x80402010000, 0x100804020100, 0x201008040201, 0x402010080402, 0x0, 0x1000000000000, 0x2010000000000, 0x4020100000000, 0x8040201000000, 0x10080402010000, 0x20100804020100, 0x40201008040201},
		{0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0x0, 0x100, 0x300, 0x700, 0xf00, 0x1f00, 0x3f00, 0x7f00, 0x0, 0x10000, 0x30000, 0x70000, 0xf0000, 0x1f0000, 0x3f0000, 0x7f0000, 0x0, 0x1000000, 0x3000000, 0x7000000, 0xf000000, 0x1f000000, 0x3f000000, 0x7f000000, 0x0, 0x100000000, 0x300000000, 0x700000000, 0xf00000000, 0x1f00000000, 0x3f00000000, 0x7f00000000, 0x0, 0x10000000000, 0x30000000000, 0x70000000000, 0xf0000000000, 0x1f0000000000, 0x3f0000000000, 0x7f0000000000, 0x0, 0x1000000000000, 0x3000000000000, 0x7000000000000, 0xf000000000000, 0x1f000000000000, 0x3f000000000000, 0x7f000000000000, 0x0, 0x100000000000000, 0x300000000000000, 0x700000000000000, 0xf00000000000000, 0x1f00000000000000, 0x3f00000000000000, 0x7f00000000000000},
		{0x0, 0x100, 0x10200, 0x1020400, 0x102040800, 0x10204081000, 0x1020408102000, 0x102040810204000, 0x0, 0x10000, 0x1020000, 0x102040000, 0x10204080000, 0x1020408100000, 0x102040810200000, 0x204081020400000, 0x0, 0x1000000, 0x102000000, 0x10204000000, 0x1020408000000, 0x102040810000000, 0x204081020000000, 0x408102040000000, 0x0, 0x100000000, 0x10200000000, 0x1020400000000, 0x102040800000000, 0x204081000000000, 0x408102000000000, 0x810204000000000, 0x0, 0x10000000000, 0x1020000000000, 0x102040000000000, 0x204080000000000, 0x408100000000000, 0x810200000000000, 0x1020400000000000, 0x0, 0x1000000000000, 0x102000000000000, 0x204000000000000, 0x408000000000000, 0x810000000000000, 0x1020000000000000, 0x2040000000000000, 0x0, 0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
	};

	// Calculates all possible moves for the direction
	uint64_t moves = rayLookup[dir][sqr];

	// Calculates all the bits that are being blocked
	const uint64_t blockers = moves & obstacles;

	if (blockers) {
		// Finds the bit thats blocking the rest
		const int blocker = bitScan(blockers);
		// Removes all the blocked bits and removes the blocker if its of the same color
		moves ^= rayLookup[dir][blocker];
	}

	return moves;
}



/*
 * This code is used for the generation of the magical keys.
 * As they have already been computed, there's no need to regenerate them.
 * This code WON'T BE RUN on a normal execution.
 */

void generateMagics(void) {
    srand(11);

    for (int i = 0; i < 64; ++i) {
        rookMagic[i] = findMagic(i, 0);
        bishMagic[i] = findMagic(i, 1);
    }

    initMagics();
}

/* Generate the magic for a given sqr and piece type
 * This algorithm is NOT guaranteed to find a solution since it works by trial and error
 */
static uint64_t findMagic(const int sqr, const int piece) {
	const uint64_t mask = (piece == BISHOP) ? diagonalMasks[sqr] : straightMasks[sqr];

    // Number of possible bbs under the given mask.
    const int n = 1 << popCount(mask);

    uint64_t attacks[4096];

    for (int conf = 0; conf < n; ++conf)
    	attacks[conf] = maskConfiguration(mask, conf);

    for (int k = 0; k < 0xfffff; ++k) {

    	// Magics with fewer bits set are better
        const uint64_t magic = randomU64() & randomU64() & randomU64();

        // It wont work if the mapped digits on the top row are less than 6
        if (popCount((mask * magic) & 0xFF00000000000000) < 6)
        	continue;

        // Initialize the collision array
        uint64_t collisions[4096];

        for (int i = 0; i < 4096; ++i)
        	collisions[i] = 0;

        /*
         * Go through all the possible states and see if it produces a collision,
         * if it doesn't it is a valid magic for the given square.
         */
        int collision = 0;

        for (int conf = 0; conf < n; ++conf) {
        	const int index = (attacks[conf] * magic) >> (64 - n);

        	if (collisions[index] == 0) {
        		collisions[index] = 1;
        	} else {
        		collision = 1;
        		break;
        	}
        }

        if (!collision)
            return magic;
    }

    fprintf(stdout, "[-] Error. The loop's limit for finding a magic number has been reached.\n");
    fflush(stdout);

    return 0;
}
