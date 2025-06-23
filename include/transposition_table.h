#pragma once
#include "Game.h"

#include <fstream>
#include <iostream>
#include <cstring> // for std::memset

// TODO maybe use History-Heuristic
namespace TT {

    constexpr ssize_t TT_SIZE = 1 << 24; // 16M entries
    constexpr ssize_t ZOBRIST_COLOR_OFFSET = 1 << 23;

    constexpr int NUM_TOWER_TYPES = 8;
    constexpr int NUM_PLAYERS = 2;
    constexpr int BOARD_SIZE = 49;

    constexpr uint64_t player_keys[NUM_PLAYERS] = {
        0x6a5d932b45ff2c83ULL, 0x7b85179ad5b077e0ULL
    };

    constexpr uint64_t zobrist_table[NUM_TOWER_TYPES * NUM_PLAYERS * BOARD_SIZE] = {
        0x1c80317fa3b1799dULL, 0xbdd640fb06671ad1ULL, 0x3eb13b9046685257ULL, 0x23b8c1e9392456deULL,
        0x1a3d1fa7bc8960a9ULL, 0xbd9c66b3ad3c2d6dULL, 0x8b9d2434e465e150ULL, 0x972a846916419f82ULL,
        0x0822e8f36c031199ULL, 0x17fc695a07a0ca6eULL, 0x3b8faa1837f8a88bULL, 0x9a1de644815ef6d1ULL,
        0x8fadc1a606cb0fb3ULL, 0xb74d0fb132e70629ULL, 0xb38a088ca65ed389ULL, 0x6b65a6a48b8148f6ULL,
        0x72ff5d2a386ecbe0ULL, 0x4737819096da1dacULL, 0xde8a774bcf36d58bULL, 0xc241330b01a9e71fULL,
        0x28df6ec4ce4a2bbdULL, 0x6c307511b2b9437aULL, 0x47229389571aa876ULL, 0x371ecd7b27cd8130ULL,
        0xc37459eef50bea63ULL, 0x1a2a73ed562b0f79ULL, 0x6142ea7d17be3111ULL, 0x5be6128e18c26797ULL,
        0x580d7b71d8f56413ULL, 0x43b7a3a69a8dca03ULL, 0x0b1f9163ce9ff57fULL, 0x759cde66bacfb3d0ULL,
        0x1ff49b7889463e85ULL, 0xec1b8ca1f91e1d4cULL, 0x142c3fe860e7a113ULL, 0x4b0dbb418d5288f1ULL,
        0xa0ee89aed453dd32ULL, 0xe2acf72f9e574f7aULL, 0x5c941cf0dc98d2c1ULL, 0x3139d32c93cd59bfULL,
        0x11ce5dd2b45ed1f0ULL, 0xa9488d990bbb2599ULL, 0xc5e7ce8a3a578a8eULL, 0xfc377a4c4a15544dULL,
        0xdaf61a26146d3f31ULL, 0xddd1dfb23b982ef8ULL, 0x614ff3d719db3ad0ULL, 0x7412b29347294739ULL,
        0xd58842dea2bc372fULL, 0x29a3b2e95d65a441ULL, 0x5af305535ec42e08ULL, 0xab9099a435a240aeULL,
        0xb3aa7efe4458a885ULL, 0xaefcfad8efc89849ULL, 0x12476f57a5e5a5abULL, 0xa28defe39bf00273ULL,
        0x88bd64072bcfbe01ULL, 0x3eabedcbbaa80dd4ULL, 0x7656af7229d4beefULL, 0x451b4cf36123fdf7ULL,
        0xece66fa2fd5166e6ULL, 0xb02b61c4a3d70628ULL, 0x3838b3268e944239ULL, 0x5304317faf42e12fULL,
        0xc4b032ccd7c524a5ULL, 0x0e51f30dc6a7ee39ULL, 0xd261a7ab3aa2e4f9ULL, 0xce177b4e0837b8a3ULL,
        0x66b2bc5b50c187fcULL, 0x10f1bc81448aaa9eULL, 0xe9c349e03602f8acULL, 0x9132b63ef16287e4ULL,
        0xb7c93acfe059a0eeULL, 0x366eb16f508ebad7ULL, 0x7fcd9eb1a7cad415ULL, 0xe27a984d654821d0ULL,
        0xa491f0b2ea1fca65ULL, 0x24933b83757750a9ULL, 0x23bed01d43cf2fdeULL, 0xbeb799193f22faf8ULL,
        0x89fa6a688fb5d27bULL, 0xbf3c4c06434308bcULL, 0x6dadd6c795a76d79ULL, 0x956269f0e5d7b875ULL,
        0x5cabcc97663f1c97ULL, 0xff50bde4382567b8ULL, 0x2369b584ff5e9ff0ULL, 0x7e570ddf827050a8ULL,
        0xc17af08a1745d6d8ULL, 0xdc713d960c0fd195ULL, 0x27209bdf1c11f735ULL, 0x28f49481a0a04dc4ULL,
        0xae340454cac5b68cULL, 0x98ae43346c12ace8ULL, 0x62801c4510435a10ULL, 0x988c24c961b1cd22ULL,
        0x77d21e02ff01cf99ULL, 0x405cacec877409a9ULL, 0x8da0365bf89897b9ULL, 0xf143262fdc5c0eedULL,
        0xae270da702f06b90ULL, 0x1d53434bb88139b9ULL, 0xe2817efdae849217ULL, 0xc03987108976e334ULL,
        0xc4c2e2e3444ea7c8ULL, 0x5715bd6fa4161293ULL, 0x4b22d3081c8eaee9ULL, 0x287d06ca6f4cc69aULL,
        0x00d4af5974273ca3ULL, 0xb8db0672f42d47ccULL, 0xb83cfe0be037e5edULL, 0xf8cda88b436d76e2ULL,
        0xc30ff46e8026695fULL, 0x81f76d1c2dbc2134ULL, 0x1b3dbd5ce9a1fa6fULL, 0xa013ac6ededa4e16ULL,
        0xd777a4774c66e0a8ULL, 0x81f631d4a39231a7ULL, 0x32ebd6899be578c7ULL, 0x5fb8d16c2720797dULL,
        0x295b4715c333e861ULL, 0xf4188f3f8a14be62ULL, 0xec24a3c5c754108fULL, 0xeb2263dd87c5421eULL,
        0x99546eb400257ad1ULL, 0x7d15438552fbe43bULL, 0x1ca35cfb04fc6d82ULL, 0x5cec4eb5edd96831ULL,
        0xfc3e058be0f3eab0ULL, 0xce88cb2dd4e80839ULL, 0x3d4cbf374eb93effULL, 0x3da9c2a90ed42f1aULL,
        0x913e4de2e0c53cb8ULL, 0x14296c07f26b4776ULL, 0xbb5e4bcf15ed6269ULL, 0xd0e6e6607c69dee1ULL,
        0xfa5d310011b7e948ULL, 0x885f6e66c2b6d2c5ULL, 0x2031d750c40db9b4ULL, 0xa8e56e0c20de435dULL,
        0xf264accc79ac1b1eULL, 0x2a45c2ab8cbfedb0ULL, 0x8715a10343dac043ULL, 0x9b49bd26df57c59aULL,
        0xf6e07cc06c52c49fULL, 0xedcd465e36386821ULL, 0xc1590f538a0f4efbULL, 0xb09b2a5cbadcc32aULL,
        0xb683d2e6337ea2dfULL, 0x66245bfa4fcca39aULL, 0xabf3ad39fec21bbeULL, 0x5f987c71a65e688eULL,
        0xe64d1bcb702753a1ULL, 0x7394988f847fd9b4ULL, 0x3f76be1d1efa2197ULL, 0x1064005c3985c3cfULL,
        0x05628059568cc69bULL, 0x8dcdcd03969b6662ULL, 0x96a402f23ae8cc93ULL, 0x01d7425638602ab6ULL,
        0xb535106e122c9a56ULL, 0x0f1259e0a18ff6b6ULL, 0x114125c63a9bedd4ULL, 0x080aadfbe7c99b26ULL,
        0x5496f63cdc1110c1ULL, 0x839fbc501223b513ULL, 0x474a493b3ceddf2dULL, 0x7c441fe7ab4220a7ULL,
        0x8a0b3c3336d8393aULL, 0xb92da22b21df306fULL, 0xe1e3db63ef7ddc76ULL, 0x93829b43922fe15aULL,
        0x3e3511287900f7f9ULL, 0x7914c120c8dcd19fULL, 0x683514f2ceb81f9dULL, 0x1825bc5430beb45fULL,
        0xa8b317fa18d0752bULL, 0x5ab33edf6e595ed3ULL, 0x693dffbc6c6fa611ULL, 0xdd2467ac778eedb3ULL,
        0x0dde29a6baa4b71aULL, 0xa748dbcfac619e63ULL, 0xa56c0941fbf24050ULL, 0x0f844fef1931e9eeULL,
        0xba6c34ab6712303aULL, 0xccf3a17156dc8907ULL, 0x1bf90e27dc96925eULL, 0x310c0c003fa7f104ULL,
        0x894a05e430b187efULL, 0x23e2fcb472d8567dULL, 0x2ef912766c006f61ULL, 0x766ecb15474ebc19ULL,
        0xdfde4fbf3ff350bfULL, 0x134c6c92ec5b227cULL, 0xceda8bbb71710434ULL, 0xdb20a56edc815fe7ULL,
        0x19108be58ce21ea3ULL, 0xa6f2f7b80cf35b58ULL, 0x8a63f881ffd0f9d5ULL, 0x03c72ba8d605e770ULL,
        0x17e011b7f8102383ULL, 0xc0e9ab30ed2662e9ULL, 0x3c835dc0d9441fa5ULL, 0x680ac07a2a935d62ULL,
        0x7b3a4e3e7c52fa17ULL, 0xdd59ba7136b82481ULL, 0xe7067ef466aa9385ULL, 0x2a25a8880f02bad0ULL,
        0x008d4127610461e3ULL, 0x63f2ae24fc3d3348ULL, 0xed3049cf43e458fcULL, 0xc8fe3ccdc8b8d9c6ULL,
        0x490617f2747b6dbaULL, 0xb253d2186c4a37eaULL, 0xbb026576f512c4c3ULL, 0xc88a618efed4057dULL,
        0xa97065e18e46d534ULL, 0x7c967f79b7e99acaULL, 0x309d258c27a0c3d7ULL, 0x37bb3eec4bf50b52ULL,
        0x0ef8c2d6f7fd5646ULL, 0xbc594585944528c0ULL, 0x0f9aea4b8acd4e10ULL, 0x504867babf7b539bULL,
        0x0cd620c20ea2622bULL, 0x7a0ecfea958ca9baULL, 0xeb5cf46780bacd64ULL, 0x87f7e1fbda4bd9caULL,
        0x0e8fa8e0284d82e5ULL, 0x82010c62f5f59b22ULL, 0xd9f195d014822f53ULL, 0x118a9d292f923996ULL,
        0x1165e21098543881ULL, 0xdca02eecacdabaccULL, 0x675dd5af3c365296ULL, 0xf10c718b1eb0e38aULL,
        0x91d63f78e3e9de99ULL, 0x94340a033f07f814ULL, 0x0a2c827e98326856ULL, 0x14fcdd549e8fc965ULL,
        0xa8499b926b5252e3ULL, 0x90b2b633956b8c0cULL, 0x50fd9d3f85d51695ULL, 0x42c18a62ef48e8d5ULL,
        0xab73295b344a54b8ULL, 0x506e5a9ab758588dULL, 0x43ff50113d1a85ddULL, 0x21813d25655238a6ULL,
        0xa53f8a28abf3e3fcULL, 0x750cab754ccc9bc2ULL, 0xedd4253b50f0fd0aULL, 0xef8c485bc07a30f2ULL,
        0x02627f7312922f83ULL, 0x9f044aed75523327ULL, 0x902059e4ff9ab5c2ULL, 0x19985f15ff002d4dULL,
        0x89a2688b12c136e0ULL, 0x8181a8cc369147ebULL, 0x21e8ac6843e42cafULL, 0x5958a499eeea163eULL,
        0x119c4ea3e1805081ULL, 0x3e896c64e117dac3ULL, 0x48f4ef125e9953d2ULL, 0x702cdd20286218b8ULL,
        0x8b10550cd5704f32ULL, 0x4d71c366b41b3143ULL, 0xfbddcf7c9c96e9ecULL, 0xce9e1a11fcbb4e59ULL,
        0x8768a84fa76afde6ULL, 0xaaf915310200b1f0ULL, 0x8dfa6a56d12dbc9aULL, 0xee87905e4ca415eaULL,
        0x1a84a51aa9d3d7c7ULL, 0xe0ccedc5f05db76eULL, 0x43b409ef2260e70fULL, 0xe3c436571d8cbbacULL,
        0xbe0f051b1b66b5a9ULL, 0x27cb6f2a8da01097ULL, 0x48212ddb45b89cd9ULL, 0x35ebd32d9ad620abULL,
        0x57c700aab7b56ea7ULL, 0xafffcfd2341ef40bULL, 0xda587e8aa25d6b29ULL, 0x81627cf1439472e6ULL,
        0x40497b717d106c60ULL, 0xe87d1c78e7c421c7ULL, 0x0d01280fd89a40c0ULL, 0xa260772317a0df49ULL,
        0xd450281c6c6f7633ULL, 0x0b49452d46d483f3ULL, 0x5563f61600e85eceULL, 0x217d65a0c56811cdULL,
        0xfad409e2a319dcb4ULL, 0x295d6fbf430f801dULL, 0x711c21c9bdc14f1fULL, 0xb4a69f3c8d3aed99ULL,
        0x8f9797b06d7ce3c9ULL, 0x1ca3c4480279b6a6ULL, 0xf1eedba313432e61ULL, 0xb0e6a969e21342b0ULL,
        0x26286bfbe767dceaULL, 0x093923de8babce3bULL, 0x5e84f058d5a804ebULL, 0x8d7248e2951f58d0ULL,
        0x6e06809725e97977ULL, 0x0ab54bde20a04502ULL, 0x5d59cd2a4eea04e7ULL, 0xeededb07e623a689ULL,
        0xf8e1daa7cbceabdeULL, 0x0a368ce7dc570131ULL, 0x5b9962c6e61fecc0ULL, 0xae9bec3635c7936cULL,
        0xaabc25fa3fe12e47ULL, 0x5a8aaeca1a50aec3ULL, 0x8f5486b7c7b5b2bcULL, 0xdfed2c43e256a6dcULL,
        0xf94d62046808593fULL, 0xbfddc3d99ee3ac2aULL, 0xecfedb992790cebdULL, 0x3c9ad14cee0caeb5ULL,
        0x2999b735dd56cc94ULL, 0xccc56569f9e8a369ULL, 0x2d534dd0cf8ebc5aULL, 0x698c206fe1a47e10ULL,
        0x2dea94930658663aULL, 0xecab3301bc8f7d29ULL, 0xc84a7b28550a1b46ULL, 0x696608aaee49f329ULL,
        0xab7f089acd5f4822ULL, 0xbc2cbb0ddd334cc7ULL, 0x3f87e362cf8d446aULL, 0x28c13091444d610bULL,
        0xb386f7a4c991603fULL, 0x61ee411a1bac27a7ULL, 0x09e9db0adf465290ULL, 0x787f2425dbccc477ULL,
        0x3317347038f16a81ULL, 0xeb1fa9f2d10bd1d0ULL, 0x598336e375d66ed4ULL, 0xd20eac174e20fd1aULL,
        0xdf0f06cbcb9bc326ULL, 0x391184973a43b2baULL, 0xa8f7ef5a060edf5bULL, 0x6601ddd03170f437ULL,
        0x475287aa5408f9acULL, 0x11c58ef0dd463c09ULL, 0xc5f8bc16f7860b50ULL, 0x59e4b6714774bc58ULL,
        0x8268690ba43825b5ULL, 0xadf4e62d6651529eULL, 0xd7fa2d8dfb2ca025ULL, 0x54c63cd889456f27ULL,
        0x0710d430f071d879ULL, 0xe08596db1d870966ULL, 0x42deffccf86c2ca2ULL, 0x94a1875d2db69edbULL,
        0xfbc9f87af668a617ULL, 0x09cb394243f59a85ULL, 0x98b8e4cc1bc044fcULL, 0x587ef3446f3f920cULL,
        0xc9535b63ba81edd9ULL, 0x6fb78271504d281fULL, 0xfbf6e16f9b3080d5ULL, 0x1d9af65982ec9f2dULL,
        0xe645f129629c2ae3ULL, 0x30a900ad939b462dULL, 0x0b5cea6a41357e8cULL, 0x6fa17735b572f3d0ULL,
        0x85197ff4006ed6e3ULL, 0xce777f00ecf27e76ULL, 0xafd5dea589d7fd6cULL, 0xf0b5156bb82c9074ULL,
        0xbcae8081bdf070aaULL, 0x3270e4faabae4f43ULL, 0x6e6981a35d3d9e56ULL, 0xf2e9702d11e9cdaaULL,
        0xebb7a385aa0b7b14ULL, 0x9f871ce75487fd4fULL, 0xa9d3c2e6505cc686ULL, 0x1fe771d6d9178793ULL,
        0xe6697833b841d0a0ULL, 0x81d2c7de4ce1eb90ULL, 0xaab97e494f2d4796ULL, 0x5380b904688c7015ULL,
        0xb27c40266703b636ULL, 0x8dedf9fb4bb00f20ULL, 0x311c6eb62095eef6ULL, 0xaa38d0a16ba25efeULL,
        0x610faa3ff0bbac67ULL, 0xbf85bf0ead64b56cULL, 0x2c8d0e44e71e43a6ULL, 0x91b0e1d99d9262afULL,
        0x67f48ad54d0b0d1aULL, 0xd56f03508c459ce2ULL, 0x4dcabfb7001a9a8bULL, 0x35ce884149732d6cULL,
        0xc9277d9b6e0d2648ULL, 0x9b4e2c249479e1e6ULL, 0x527eecfaa79ac9aaULL, 0x7118e36477097749ULL,
        0xacf5e81e71316269ULL, 0x82dc4c8e36b5229aULL, 0xcb323e357922bac2ULL, 0xf5b78cc7e6b3c944ULL,
        0xbc67f831cbc84759ULL, 0xa8aa71582b70e525ULL, 0x48a639d015b52908ULL, 0xa9f2533683f4a9a9ULL,
        0x9e87e04ca2086977ULL, 0x17e8392a55cee5dbULL, 0xf3b63fe1d1843324ULL, 0x3c20592fc04a96c4ULL,
        0x4f77a665ac3c5640ULL, 0xce7ae7f639820cffULL, 0x25b8fd4b32fa2de8ULL, 0x0bd4a9900640be0fULL,
        0xfbe33b243eae0032ULL, 0x9c7c737779a28903ULL, 0xc4bbb7a9d98868ddULL, 0x7496276412a4def0ULL,
        0xe2d9de5d6a18ce4cULL, 0x935f2b0aa1384ddcULL, 0xb7e5848131c681ecULL, 0x624c69b6b24445a7ULL,
        0x664fa6637e8f8095ULL, 0x25c73c443e75c3b4ULL, 0xb00805cca7f36ae9ULL, 0xe4855aa1016b6287ULL,
        0xdc45d539c03f3538ULL, 0xe2add909c521bf2dULL, 0xc7468f591b494e15ULL, 0x3805f9076cd66193ULL,
        0xcdda24ba2d06e8cfULL, 0xb227462cf53d4330ULL, 0x76ecbdd68498e113ULL, 0x8eb225790cdb1ca4ULL,
        0xeadf50853fcb7546ULL, 0x1f115b76d92c9227ULL, 0x222282e174daaebfULL, 0x76f2dbfecd29a36fULL,
        0x87f8424daae65fc1ULL, 0x8f15ba58fce68504ULL, 0x513a7052986f9025ULL, 0xc1581092f335cba3ULL,
        0x714c7df4e4347d51ULL, 0xd0a444329cd6c852ULL, 0xe45b712eb8225688ULL, 0x6d3ee1dc81392443ULL,
        0xe827a1b9d4a02e53ULL, 0x722764e68c41561bULL, 0x28be9288e5af6e39ULL, 0xdc8aee30be6033f7ULL,
        0x7337c59979844388ULL, 0xc074718e425a609fULL, 0xd701410d3f4b1a70ULL, 0x46fde062a33dc7afULL,
        0xc715b2b9c40c5d91ULL, 0x7c0e8cd88573e793ULL, 0x3d3f3799a07295e9ULL, 0x709b7d97464c04afULL,
        0xb6aae05b13d5f2f7ULL, 0x3c07c57449257af1ULL, 0x55fa1ab8458f1f19ULL, 0xe49d681d51d87c64ULL,
        0x14a0bccb8a476a87ULL, 0x269cd696236c7b87ULL, 0x620e99d33b33f3d8ULL, 0x271e3ee2b1a6b1f1ULL,
        0x36c59dacb4d7e28eULL, 0x6a34c85410714d51ULL, 0x54b4a48268586ebaULL, 0x7746d0ba8ae8905bULL,
        0x0ff0a55c6a702e2fULL, 0xd5385b0e34f3193cULL, 0x63b4c08b6b8e869fULL, 0xc51155ffe7a37e81ULL,
        0xf2311f1795863a76ULL, 0x05000bc6b20dcb6eULL, 0xe172b725db52ca58ULL, 0x9360715fc3fe0183ULL,
        0x7a1b58066160a6b4ULL, 0xf1578470018267c4ULL, 0x4c71e0fe5a0cdd7cULL, 0x63d62a39c0e3befdULL,
        0xe4429ebbda7b9095ULL, 0xd5f25073f41402b1ULL, 0x89c8d2ab6b44fa8dULL, 0xbc10fa52bf5d2fdfULL,
        0xccc429038bcf53a1ULL, 0xe5d6f6e69a6ec2f5ULL, 0x7cfc9b793875394cULL, 0x45df16b6382c043fULL,
        0x7c5308bf6f92f25eULL, 0x638c254c076e2bbaULL, 0xab3b4d37560c95eeULL, 0xcc530e36addc3e13ULL,
        0xb963f37f67814c1fULL, 0xd72b61082a405f12ULL, 0xeb67146a77a6e17cULL, 0xfb10987f20ac3703ULL,
        0x88bc539c9f4c3b79ULL, 0xe82c7d7b06e745f9ULL, 0x978648f864de82e6ULL, 0xa9ba5a27907bfe36ULL,
        0x157d94a106f028ffULL, 0x6db99102a48b3dbeULL, 0xdde9f86322bd3388ULL, 0x2e85cb217631de9dULL,
        0x42999aa40cdf742bULL, 0x53cd6268610cf373ULL, 0x74672cd9362f5e5cULL, 0x56666f9f53ac2ab9ULL,
        0xe1301617c2dff335ULL, 0x473bd358610e6a64ULL, 0xf3821cfdc083b73aULL, 0x6bebac31d4f8fd72ULL,
        0xd5bcb8d04094ddedULL, 0x7866076514f7ce8dULL, 0xbfc00dc804f64d86ULL, 0x0d557b618a175dfeULL,
        0xf3b1025bfff9f585ULL, 0x39669fa759970043ULL, 0x1190f938a66fd7f7ULL, 0xf510ab53c7fee39fULL,
        0x0a4e5b70a6d964a3ULL, 0x07f194f9c1156d6dULL, 0x3f4df561f319c125ULL, 0xd6d7b3b833094d35ULL,
        0x9f0fda8d05379ff6ULL, 0x3d1148022702878bULL, 0x793b4c3220500494ULL, 0x1d48a071ab61a7b1ULL,
        0xf2a0345990604f62ULL, 0x770c779837cc863bULL, 0x41992fdfb31022f0ULL, 0x5e6fea07c4536f1dULL,
        0x9b1bc8952af43ab7ULL, 0xf6b751f79b749245ULL, 0xb7e6427cbf780e3fULL, 0xc71d5e601d5206abULL,
        0x29ec8e49d1bdb8c0ULL, 0x4fa03f26f6f7f0ccULL, 0x9424aed51bac5c15ULL, 0xedcb8cb60692dc63ULL,
        0x93676a024fdc6e1bULL, 0xe87466d7ad66a1bdULL, 0x60141de9f54ad0a2ULL, 0xf1043785658b2523ULL,
        0x32c5bd89b70b3420ULL, 0x9793b9b413748146ULL, 0xd4a057a7b0cc1b3bULL, 0x3e2b6091a092f52aULL,
        0xb27b3d901a16342cULL, 0x4d3485c5c5c14eb4ULL, 0xaf2b99b4d9acd158ULL, 0xce3714af99b49350ULL,
        0xcbd58bf61efd76e9ULL, 0x90e0f4a0fbdd3933ULL, 0x0a8381bec85aca46ULL, 0x8861fe1858e25888ULL,
        0xa95976636daa2e68ULL, 0x11a726095eddbbbfULL, 0xa5c5650c8186a576ULL, 0x033d2bce575aed2cULL,
        0x6b88f83dd97dc9cdULL, 0x7d7ddbedd284476cULL, 0x6efb63b11b049863ULL, 0x5cb85aedf5f62c97ULL,
        0xe43e4288a2b5b498ULL, 0x75b17a55d4262982ULL, 0x272a6d8eb5122df8ULL, 0x2d174fc96f7c15eaULL,
        0x859131d2bbda0242ULL, 0xa6846099f7294951ULL, 0x9dac6e8345241ea6ULL, 0xeb6c1016cee624d0ULL,
        0xc64ee6e389c5b31aULL, 0x7701f7bb7bc67e1fULL, 0xd36357b66f81cf4fULL, 0x97ac6aa8bb2488a3ULL,
        0x52828d8044b591f7ULL, 0x3ed8c56cda09dfa0ULL, 0xef43613cd4aac9a3ULL, 0x4767d76c162f8a24ULL,
        0x7367c28de1b294deULL, 0xc01f36bf3e6dd58bULL, 0x91e1aa9676f72255ULL, 0xab0e664e9c3eb2d5ULL,
        0x561e16d16105716bULL, 0x7e8adee70758e201ULL, 0x533420e6d9d80b8dULL, 0x7cd0129d2e8d0e87ULL,
        0x5ad5cf06364d7c87ULL, 0x4223623bcc3ebddeULL, 0x4797b2c957207246ULL, 0x989d9d4ae15ca666ULL,
        0xe14eb70db380c73aULL, 0x8e48522346b98991ULL, 0x8441aefd0299436aULL, 0x30e912f2f2b43abfULL,
        0x3dc9829015eabb27ULL, 0x680bac63b856d035ULL, 0x8e2007247d137018ULL, 0x3d85de89c2171429ULL,
        0x79e13ceab0cbc61fULL, 0xb63b4dc3a559e463ULL, 0x72bb912d7da67785ULL, 0x046a0df5cafda613ULL,
        0x4b5305e517d2582eULL, 0x6786d50638ba8abcULL, 0x3e493f43b118f68dULL, 0xa9f948b24e6384bbULL,
        0x5e781fd794e0d3baULL, 0x8db0674679279973ULL, 0x58007c0287ea7ff5ULL, 0xff233d5f6cedd15dULL,
        0x8ce6424dbef59fe6ULL, 0x5a10412954aebd1bULL, 0x7428a656b3ee4d3bULL, 0x4e7ed827455ac762ULL,
        0x3b048a8b405bfdc9ULL, 0xb8a6171f1ee34dc4ULL, 0x50c7c006314d3441ULL, 0xbe2d740a1e9b23bcULL,
        0xf36cb62b892e6161ULL, 0xb0ae8f08c31edbbcULL, 0x3108d4482f65fafaULL, 0xbd1531c83764fbdaULL,
        0x46c8adfe7bf47042ULL, 0x96ef2ad6b97e6703ULL, 0xc29cfc0cfa02eaecULL, 0x98c7472a864e9a13ULL,
        0xfb02bebb48729a4dULL, 0xd52721e719bc143eULL, 0x4bd6cee631b1b099ULL, 0x5c62b3a23a3c563eULL,
        0x4d6168bd2defe193ULL, 0xb540b30e039f3a25ULL, 0x2067bdac88bd13d1ULL, 0x0ba6eab94639447bULL,
        0x0df56ac6f96b648aULL, 0x4ac9778d8da8eee4ULL, 0xf1afdb65b289f224ULL, 0xa34b6cf62053da42ULL,
        0xc0b6fce2de53790aULL, 0x1a432f0a7daa39f0ULL, 0x0323d342df6a8f93ULL, 0x48ca765192f5df7bULL,
        0x7a8d03aa782a65e0ULL, 0x5738811d70c2903fULL, 0xf72ada9b2f32751eULL, 0x40a26c600d270659ULL,
        0xdc99e04cf0e98b3bULL, 0x1d34d08e7a4c75d4ULL, 0x10ba58e3d2762bdcULL, 0x7de31a516694c343ULL,
        0x93b7a88612f70c97ULL, 0xafbb411aa1235a8cULL, 0x26d794d30db95301ULL, 0xcfa701cd2631d00bULL,
        0xf2f9e5fa90164161ULL, 0x15ce6a664dc82a1eULL, 0x3f897142fe716b14ULL, 0x8edddfcd1e52d770ULL,
        0x6a8a616fc3b290d0ULL, 0x989bc4da9b37a22bULL, 0x9e50aa42ca6dfda1ULL, 0xc693da1139c6a1caULL,
        0x6160745985c7504bULL, 0xe893be3d7354ea6fULL, 0x4c1f55ab715629eeULL, 0x96a9954fdc33e1f9ULL,
        0x6dc7cac7fd72b050ULL, 0x9191b3634e2d6645ULL, 0x0f6b40d09efba58bULL, 0xf5c9b0479c10c572ULL,
        0x19675f06bd767e35ULL, 0xc342bd2bf295456eULL, 0xa021c0ca3531968dULL, 0x43bfd9313605bf54ULL,
        0x14c8b3b4a911d192ULL, 0x3d67cde92834e4c0ULL, 0x8d4f5d272c7f0b79ULL, 0x2812859a1337739eULL,
        0x68949b8d00af5b3aULL, 0xb07aa066735435eaULL, 0x784c2f29980402a2ULL, 0x085b15fb4a8ff810ULL,
        0x49c13de73b4206c5ULL, 0x48603b32b4fb0eb9ULL, 0xdc0f2fcfb3f6fe0dULL, 0x1238d630743b65a2ULL,
        0x3bc1a987aff8754dULL, 0x43b9da13ec856f37ULL, 0xca8f3653c9af18f8ULL, 0x96fc734da003cd28ULL,
        0xcdccc33aa9434aa0ULL, 0x32a447b2ef04e57dULL, 0x1d61fac36cd5e859ULL, 0x398d1ca68b6870b5ULL,
        0x26242b40a5cb63a2ULL, 0x44007d5ae88da719ULL, 0x246998e8d39e198bULL, 0x0f44704f1247ea4eULL,
        0xcae9b4a72a79ea68ULL, 0x9854ce4e4ebfa5c3ULL, 0xd3016989bfbbb17fULL, 0xebd3461691b78d8eULL,
        0x706c5c5649e2623dULL, 0x77fc97031fd5a423ULL, 0x4dd8eb85b04d3376ULL, 0x670acc5cb321bf21ULL,
        0x45b1ed25f1533ae8ULL, 0x8a3c3b5e801ef1daULL, 0x7010f7197e695d0dULL, 0x9918ee461497d658ULL,
        0xe3b137fc0a3450fcULL, 0xbc0a6a5d6e996e3eULL, 0x9a8cfa3c5283aac7ULL, 0x069f14f140181c6eULL,
        0x3a9aca5e176132edULL, 0xac9ed156f63fce41ULL, 0xdc4ad56bd6016237ULL, 0x964db03f93403fadULL,
        0x054f92fff366bad4ULL, 0xc3c75611ffe3fa49ULL, 0xd248a9a7ac1aa554ULL, 0x9384ec2b44feacaeULL,
        0xc35b1c8c0a4c9f7fULL, 0x2cd94cbbc19ad58cULL, 0x84dad06a7872bdebULL, 0x7135f221a6c9537fULL,
        0x473544f9ea83bf00ULL, 0xff37d19c2e76128bULL, 0x6f96288295d82980ULL, 0xd0725b5ca2814044ULL,
        0xf81401027de1bdfeULL, 0x785299f4175ba98dULL, 0x6889803e5913f9d3ULL, 0x5230dfbd5553b2feULL,
        0x1ac70ec0ab8ddeb4ULL, 0x292bd156db946570ULL, 0x6961929e546e035aULL, 0x7ed70ed7b194990bULL,
        0xa99f131849c8a43fULL, 0x668409e3f1f8343eULL, 0xc2b01cfdd045dd1cULL, 0x0964fbbf8cd321b0ULL,
        0x168b1625746f7891ULL, 0x409d360250843242ULL, 0x1dad09b252c21221ULL, 0xc5c5b37af85e06a1ULL,
        0xdd6ac7b86778043bULL, 0xd32e6dcd83bc9478ULL, 0x004b6fabfcf56188ULL, 0xde8ede0ba85c6e4aULL,
        0x764414fd8ae769edULL, 0x0de051a669ca97d2ULL, 0x84b871bb300568d2ULL, 0x9f64eeed5c9d927dULL,
        0x7f9d3e64c1a6423bULL, 0x71299889a01ac992ULL, 0x0d366dfcc28ebd70ULL, 0x445dcc38341c6494ULL,
        0x218a15368c99a894ULL, 0x49bc473fed7bf656ULL, 0xe17f29e170286046ULL, 0x7c16128db2c08394ULL,
        0x0763fcd01f15c7b6ULL, 0xa14923c2f920264cULL, 0xcc9fd3349bdf0377ULL, 0xb5b453ca3d42993cULL,
        0x4f8d5238288b78b5ULL, 0x03802b708d03c91eULL, 0x687213f98d605936ULL, 0x3985fb6217dc8effULL,
        0xd7665cdafe049059ULL, 0x1d0bc9bde9b5c5cfULL, 0xf27292b6762172edULL, 0xa5d04d531e1242e3ULL,
        0x276aa6ced50755d9ULL, 0xeec259dc7f95897cULL, 0x4ab7706eb77350caULL, 0xb495db4e82456fb4ULL
    };

    enum class Flag : uint8_t {
        EXACT, ALPHA_CUTOFF, BETA_CUTOFF
    };

    // only save index for smaller form factor
    struct TT_Move {
        uint8_t from_sq = 0;
        uint8_t to_sq = 0;
        uint8_t move_distance = 0;

        TT_Move() = default;

        explicit TT_Move(const Move& move)
         : from_sq(std::countr_zero(move.from)), to_sq(std::countr_zero(move.to)), move_distance(move.move_distance) {}

        [[nodiscard]] Move convertToMove() const {
            const uint64_t from_pos = 1ULL << from_sq;
            const uint64_t to_pos = 1ULL << to_sq;
            return Move{from_pos, to_pos, move_distance};
        }
    };

    // 16 Bytes
    struct TTEntry {
        uint64_t key = 0;
        int score = 0;
        TT_Move bestMove{};
        int depth = 0;
        Flag type = Flag::EXACT;
    };

    inline TTEntry tt[TT_SIZE]; // Global table

    inline void store(const uint64_t key, const int score, const Move &bestMove, const int depth, const Flag type) {
        const uint64_t index = key & (TT_SIZE - 1);
        TTEntry& entry = tt[index];

        // Replace if deeper or new
        if (entry.key == 0 || depth >= entry.depth) {
            entry = TTEntry{key, score, TT_Move(bestMove), depth, type};
        }
    }

    inline uint64_t getKey(const Game& game) {
        uint64_t key = player_keys[game.active_player];

        for (int color = 0; color < 2; color++) {
            const uint64_t color_mask = (color == 0) ? game.bitBoards[C_B] : game.bitBoards[C_R];
            const int offset = (color == 0) ? 0 : ZOBRIST_COLOR_OFFSET;
            for (int tower_type = 0; tower_type <= T_G; tower_type++) {
                if (uint64_t tower = game.bitBoards[tower_type] & color_mask) {
                    while (tower) {
                        const int bit_index = std::countr_zero(tower);
                        key ^= zobrist_table[offset + tower_type * 49 + (convert_pos[bit_index] - 1)];
                        tower &= (tower - 1);
                    }
                }
            }
        }
        return key;
    }

    // first call removes old hashes, second call adds new hashes
    inline void flipHashForMove(const Game &game, uint64_t& old_key, const Move& move) {
         old_key ^= player_keys[game.active_player];

        const int offset_from = ((game.bitBoards[C_R] & move.from) == 0) ? 0 : ZOBRIST_COLOR_OFFSET;
        for (int tower_type = 0; tower_type <= T_G; tower_type++) {
            if (const uint64_t tower = game.bitBoards[tower_type] & move.from) {
                    const int bit_index = std::countr_zero(tower);
                    old_key ^= zobrist_table[offset_from + tower_type * 49 + (convert_pos[bit_index] - 1)];
                    break;
            }
        }
        const int offset_to = ((game.bitBoards[C_R] & move.to) == 0) ? 0 : ZOBRIST_COLOR_OFFSET;
        for (int tower_type = 0; tower_type <= T_G; tower_type++) {
            if (const uint64_t tower = game.bitBoards[tower_type] & move.to) {
                const int bit_index = std::countr_zero(tower);
                old_key ^= zobrist_table[offset_to + tower_type * 49 + (convert_pos[bit_index] - 1)];
                break;
            }
        }
    }


    inline bool probe(const uint64_t key, TTEntry& out) {
        const uint64_t index = key & (TT_SIZE - 1);
        const TTEntry& entry = tt[index];

        if (entry.key == key) {
            out = entry;
            return true;
        }
        return false;
    }

    inline bool loadFromFile() {
        std::ifstream in("../transposition_table/transposition_table.bin", std::ios::binary);
        if (!in) {
            std::cerr << "TT load failed: file not found. Initializing TT and creating file...\n";
            std::memset(tt, 0, sizeof(tt));

            // Create and write the initialized TT to the file
            std::ofstream out("../transposition_table/transposition_table.bin", std::ios::binary);
            if (!out) {
                std::cerr << "Failed to create TT file.\n";
                return false;
            }
            out.write(reinterpret_cast<const char*>(tt), sizeof(tt));
            out.close();
            return true;  // File created and written successfully
        }

        in.read(reinterpret_cast<char*>(tt), sizeof(tt));
        if (!in) {
            std::cerr << "TT load failed: read error. Initializing TT...\n";
            std::memset(tt, 0, sizeof(tt));
            return false;
        }

        std::cout << "Loading TT finished" << std::endl;

        in.close();
        return true;
    }

    inline bool saveToFile() {
        std::ofstream out("../transposition_table/transposition_table.bin", std::ios::binary);
        if (!out) {
            std::cerr << "TT save failed: could not open file.\n";
            return false;
        }

        out.write(reinterpret_cast<const char*>(tt), sizeof(tt));
        out.close();
        std::cout << "Saved TT finished" << std::endl;
        return out.good();
    }

}
