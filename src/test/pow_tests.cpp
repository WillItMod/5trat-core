// Copyright (c) 2015-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chain.h>
#include <chainparams.h>
#include <consensus/amount.h>
#include <pow.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>
#include <util/chaintype.h>
#include <validation.h>

#include <boost/test/unit_test.hpp>

#include <vector>

BOOST_FIXTURE_TEST_SUITE(pow_tests, BasicTestingSetup)

/* Test calculation of next difficulty target with no constraints applying */
BOOST_AUTO_TEST_CASE(get_next_work)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    int64_t nLastRetargetTime = 1261130161; // Block #30240
    CBlockIndex pindexLast;
    pindexLast.nHeight = 32255;
    pindexLast.nTime = 1262152739;  // Block #32255
    pindexLast.nBits = 0x1d00ffff;

    // Here (and below): expected_nbits is calculated in
    // CalculateNextWorkRequired(); redoing the calculation here would be just
    // reimplementing the same code that is written in pow.cpp. Rather than
    // copy that code, we just hardcode the expected result.
    unsigned int expected_nbits = 0x1d00d86aU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, chainParams->GetConsensus()), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, expected_nbits));
}

/* Test the constraint on the upper bound for next work */
BOOST_AUTO_TEST_CASE(get_next_work_pow_limit)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    int64_t nLastRetargetTime = 1231006505; // Block #0
    CBlockIndex pindexLast;
    pindexLast.nHeight = 2015;
    pindexLast.nTime = 1233061996;  // Block #2015
    pindexLast.nBits = 0x1d00ffff;
    unsigned int expected_nbits = 0x1d00ffffU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, chainParams->GetConsensus()), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, expected_nbits));
}

/* Test the constraint on the lower bound for actual time taken */
BOOST_AUTO_TEST_CASE(get_next_work_lower_limit_actual)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    int64_t nLastRetargetTime = 1279008237; // Block #66528
    CBlockIndex pindexLast;
    pindexLast.nHeight = 68543;
    pindexLast.nTime = 1279297671;  // Block #68543
    pindexLast.nBits = 0x1c05a3f4;
    unsigned int expected_nbits = 0x1c0168fdU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, chainParams->GetConsensus()), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, expected_nbits));
    // Test that reducing nbits further would not be a PermittedDifficultyTransition.
    // NOTE: This check only applies to pre-fork (original DAA) blocks. Post-fork ASERT
    // doesn't have per-block change limits - difficulty is deterministic based on timestamps.
    unsigned int invalid_nbits = expected_nbits-1;
    if (!chainParams->GetConsensus().IsBCH2ForkActive(pindexLast.nHeight+1)) {
        BOOST_CHECK(!PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, invalid_nbits));
    }
}

/* Test the constraint on the upper bound for actual time taken */
BOOST_AUTO_TEST_CASE(get_next_work_upper_limit_actual)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    int64_t nLastRetargetTime = 1263163443; // NOTE: Not an actual block time
    CBlockIndex pindexLast;
    pindexLast.nHeight = 46367;
    pindexLast.nTime = 1269211443;  // Block #46367
    pindexLast.nBits = 0x1c387f6f;
    unsigned int expected_nbits = 0x1d00e1fdU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, chainParams->GetConsensus()), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, expected_nbits));
    // Test that increasing nbits further would not be a PermittedDifficultyTransition.
    unsigned int invalid_nbits = expected_nbits+1;
    BOOST_CHECK(!PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, invalid_nbits));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_negative_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    nBits = UintToArith256(consensus.powLimit).GetCompact(true);
    hash.SetHex("0x1");
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_overflow_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits{~0x00800000U};
    hash.SetHex("0x1");
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_too_easy_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    arith_uint256 nBits_arith = UintToArith256(consensus.powLimit);
    nBits_arith *= 2;
    nBits = nBits_arith.GetCompact();
    hash.SetHex("0x1");
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_biger_hash_than_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    arith_uint256 hash_arith = UintToArith256(consensus.powLimit);
    nBits = hash_arith.GetCompact();
    hash_arith *= 2; // hash > nBits
    hash = ArithToUint256(hash_arith);
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_zero_target)
{
    const auto consensus = CreateChainParams(*m_node.args, ChainType::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    arith_uint256 hash_arith{0};
    nBits = hash_arith.GetCompact();
    hash = ArithToUint256(hash_arith);
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(GetBlockProofEquivalentTime_test)
{
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    std::vector<CBlockIndex> blocks(10000);
    for (int i = 0; i < 10000; i++) {
        blocks[i].pprev = i ? &blocks[i - 1] : nullptr;
        blocks[i].nHeight = i;
        blocks[i].nTime = 1269211443 + i * chainParams->GetConsensus().nPowTargetSpacing;
        blocks[i].nBits = 0x207fffff; /* target 0x7fffff000... */
        blocks[i].nChainWork = i ? blocks[i - 1].nChainWork + GetBlockProof(blocks[i - 1]) : arith_uint256(0);
    }

    for (int j = 0; j < 1000; j++) {
        CBlockIndex *p1 = &blocks[InsecureRandRange(10000)];
        CBlockIndex *p2 = &blocks[InsecureRandRange(10000)];
        CBlockIndex *p3 = &blocks[InsecureRandRange(10000)];

        int64_t tdiff = GetBlockProofEquivalentTime(*p1, *p2, *p3, chainParams->GetConsensus());
        BOOST_CHECK_EQUAL(tdiff, p1->GetBlockTime() - p2->GetBlockTime());
    }
}

void sanity_check_chainparams(const ArgsManager& args, ChainType chain_type)
{
    const auto chainParams = CreateChainParams(args, chain_type);
    const auto consensus = chainParams->GetConsensus();

    // hash genesis is correct
    BOOST_CHECK_EQUAL(consensus.hashGenesisBlock, chainParams->GenesisBlock().GetHash());

    // target timespan is an even multiple of spacing
    BOOST_CHECK_EQUAL(consensus.nPowTargetTimespan % consensus.nPowTargetSpacing, 0);

    // genesis nBits is positive, doesn't overflow and is lower than powLimit
    arith_uint256 pow_compact;
    bool neg, over;
    pow_compact.SetCompact(chainParams->GenesisBlock().nBits, &neg, &over);
    BOOST_CHECK(!neg && pow_compact != 0);
    BOOST_CHECK(!over);
    // The private 5TRAT prototype uses an intentionally easy, hardcoded
    // genesis while block one is anchored at the production-style target.
    // Production launch replaces this exemption with an ASIC-mined genesis.
    if (chain_type != ChainType::MAIN && chain_type != ChainType::TESTNET) {
        BOOST_CHECK(UintToArith256(consensus.powLimit) >= pow_compact);
    }

    // check max target * 4*nPowTargetTimespan doesn't overflow -- see pow.cpp:CalculateNextWorkRequired()
    if (!consensus.fPowNoRetargeting) {
        arith_uint256 targ_max{UintToArith256(uint256S("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"))};
        targ_max /= consensus.nPowTargetTimespan*4;
        BOOST_CHECK(UintToArith256(consensus.powLimit) < targ_max);
    }
}

BOOST_AUTO_TEST_CASE(ChainParams_MAIN_sanity)
{
    sanity_check_chainparams(*m_node.args, ChainType::MAIN);
}

BOOST_AUTO_TEST_CASE(ChainParams_REGTEST_sanity)
{
    sanity_check_chainparams(*m_node.args, ChainType::REGTEST);
}

BOOST_AUTO_TEST_CASE(ChainParams_TESTNET_sanity)
{
    sanity_check_chainparams(*m_node.args, ChainType::TESTNET);
}

BOOST_AUTO_TEST_CASE(ChainParams_SIGNET_sanity)
{
    sanity_check_chainparams(*m_node.args, ChainType::SIGNET);
}

// ============================================================================
// BCH2 ASERT Difficulty Adjustment Tests
// ============================================================================
// These tests verify the CalculateASERT() function directly, which implements
// the ASERT (aserti3-2d) algorithm from Bitcoin Cash Node (BCHN).

BOOST_AUTO_TEST_CASE(asert_on_schedule)
{
    // When blocks are exactly on schedule (600 seconds apart), difficulty should stay the same
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();

    arith_uint256 refTarget;
    refTarget.SetCompact(0x1d00ffff);  // Standard initial difficulty

    const arith_uint256 powLimit = UintToArith256(consensus.powLimit);
    const int64_t nPowTargetSpacing = 600;  // 10 minutes
    const int64_t nHalfLife = 172800;       // 2 days

    // Test: 10 blocks, exactly 6000 seconds elapsed (on schedule)
    // timeDiff = 6000, heightDiff = 10
    // exponent = (6000 - 600 * (10 + 1)) / 172800 = (6000 - 6600) / 172800 = -600/172800 ≈ -0.00347
    // Result should be very close to refTarget (slightly lower due to -600 seconds)
    arith_uint256 result = CalculateASERT(refTarget, nPowTargetSpacing, 6000, 10, powLimit, nHalfLife);

    // The result should be within 1% of refTarget for on-schedule blocks
    arith_uint256 diff = (result > refTarget) ? result - refTarget : refTarget - result;
    arith_uint256 tolerance = refTarget / 100;  // 1%
    BOOST_CHECK(diff < tolerance);
}

BOOST_AUTO_TEST_CASE(asert_slow_blocks)
{
    // When blocks are slower than schedule, difficulty should decrease (target increases)
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();

    arith_uint256 refTarget;
    refTarget.SetCompact(0x1c0fffff);  // Some arbitrary difficulty

    const arith_uint256 powLimit = UintToArith256(consensus.powLimit);
    const int64_t nPowTargetSpacing = 600;
    const int64_t nHalfLife = 172800;

    // Test: 10 blocks but 12000 seconds elapsed (blocks taking 20 min each instead of 10 min)
    // Ideal time for 10 blocks = 6000 seconds, actual = 12000 seconds
    // Blocks are slow, so difficulty should decrease (target increases)
    arith_uint256 result = CalculateASERT(refTarget, nPowTargetSpacing, 12000, 10, powLimit, nHalfLife);

    BOOST_CHECK(result > refTarget);  // Target increased (difficulty decreased)
}

BOOST_AUTO_TEST_CASE(asert_fast_blocks)
{
    // When blocks are faster than schedule, difficulty should increase (target decreases)
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();

    arith_uint256 refTarget;
    refTarget.SetCompact(0x1c0fffff);

    const arith_uint256 powLimit = UintToArith256(consensus.powLimit);
    const int64_t nPowTargetSpacing = 600;
    const int64_t nHalfLife = 172800;

    // Test: 10 blocks but only 3000 seconds elapsed (blocks taking 5 min each instead of 10 min)
    // Blocks are fast, so difficulty should increase (target decreases)
    arith_uint256 result = CalculateASERT(refTarget, nPowTargetSpacing, 3000, 10, powLimit, nHalfLife);

    BOOST_CHECK(result < refTarget);  // Target decreased (difficulty increased)
}

BOOST_AUTO_TEST_CASE(asert_halflife)
{
    // After exactly half-life seconds of delay, difficulty should halve (target should double)
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();

    arith_uint256 refTarget;
    refTarget.SetCompact(0x1c00ffff);  // Use a target that won't overflow when doubled

    const arith_uint256 powLimit = UintToArith256(consensus.powLimit);
    const int64_t nPowTargetSpacing = 600;
    const int64_t nHalfLife = 172800;  // 2 days

    // Test: 1 block, but half-life + 600 seconds elapsed
    // timeDiff - spacing * (heightDiff + 1) = (nHalfLife + 600) - 600 * 2 = nHalfLife - 600
    // We want exponent = nHalfLife exactly, so:
    // timeDiff = nHalfLife + spacing * (heightDiff + 1) = 172800 + 600 * 2 = 174000 for heightDiff=1
    int64_t timeDiff = nHalfLife + nPowTargetSpacing * 2;  // 174000 for heightDiff=1
    arith_uint256 result = CalculateASERT(refTarget, nPowTargetSpacing, timeDiff, 1, powLimit, nHalfLife);

    // Target should approximately double (2x refTarget)
    arith_uint256 expectedTarget = refTarget * 2;
    arith_uint256 diff = (result > expectedTarget) ? result - expectedTarget : expectedTarget - result;
    arith_uint256 tolerance = expectedTarget / 100;  // 1% tolerance for fixed-point math approximation
    BOOST_CHECK(diff < tolerance);
}

BOOST_AUTO_TEST_CASE(asert_clamped_to_powlimit)
{
    // When difficulty would go below minimum (target > powLimit), clamp to powLimit
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();

    arith_uint256 refTarget;
    refTarget.SetCompact(0x1d00ffff);  // Near powLimit already

    const arith_uint256 powLimit = UintToArith256(consensus.powLimit);
    const int64_t nPowTargetSpacing = 600;
    const int64_t nHalfLife = 172800;

    // Test: Very slow blocks - 10x halflife delay
    // This would push target way above powLimit
    int64_t timeDiff = nHalfLife * 10 + nPowTargetSpacing * 2;
    arith_uint256 result = CalculateASERT(refTarget, nPowTargetSpacing, timeDiff, 1, powLimit, nHalfLife);

    // Result should be clamped to powLimit
    BOOST_CHECK(result == powLimit);
}

BOOST_AUTO_TEST_CASE(asert_never_zero)
{
    // Target should never be zero, even with extreme fast blocks
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();

    arith_uint256 refTarget;
    refTarget.SetCompact(0x1d00ffff);

    const arith_uint256 powLimit = UintToArith256(consensus.powLimit);
    const int64_t nPowTargetSpacing = 600;
    const int64_t nHalfLife = 172800;

    // Test: Extremely fast blocks - negative time drift
    // 100 blocks in just 1 second (impossible but tests edge case)
    arith_uint256 result = CalculateASERT(refTarget, nPowTargetSpacing, 1, 100, powLimit, nHalfLife);

    // Result should never be zero
    BOOST_CHECK(result >= arith_uint256(1));
}

BOOST_AUTO_TEST_CASE(asert_deterministic)
{
    // ASERT should be deterministic - same inputs always produce same output
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();

    arith_uint256 refTarget;
    refTarget.SetCompact(0x1c0fffff);

    const arith_uint256 powLimit = UintToArith256(consensus.powLimit);
    const int64_t nPowTargetSpacing = 600;
    const int64_t nHalfLife = 172800;

    // Calculate same result multiple times
    arith_uint256 result1 = CalculateASERT(refTarget, nPowTargetSpacing, 10000, 15, powLimit, nHalfLife);
    arith_uint256 result2 = CalculateASERT(refTarget, nPowTargetSpacing, 10000, 15, powLimit, nHalfLife);
    arith_uint256 result3 = CalculateASERT(refTarget, nPowTargetSpacing, 10000, 15, powLimit, nHalfLife);

    BOOST_CHECK(result1 == result2);
    BOOST_CHECK(result2 == result3);
}

BOOST_AUTO_TEST_CASE(asert_height_zero)
{
    // Test at height diff 0 (anchor block itself)
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();

    arith_uint256 refTarget;
    refTarget.SetCompact(0x1c0fffff);

    const arith_uint256 powLimit = UintToArith256(consensus.powLimit);
    const int64_t nPowTargetSpacing = 600;
    const int64_t nHalfLife = 172800;

    // At height diff 0, timeDiff should be 0 for on-schedule
    // Formula: exponent = (timeDiff - spacing * (0 + 1)) / halflife = (0 - 600) / 172800
    // This is a small negative exponent, so target should decrease slightly
    arith_uint256 result = CalculateASERT(refTarget, nPowTargetSpacing, 0, 0, powLimit, nHalfLife);

    // Should be slightly less than refTarget (more difficulty)
    BOOST_CHECK(result < refTarget);
    // But should be close (within 1%)
    arith_uint256 diff = refTarget - result;
    arith_uint256 tolerance = refTarget / 100;
    BOOST_CHECK(diff < tolerance);
}

BOOST_AUTO_TEST_CASE(asert_symmetric_adjustment)
{
    // ASERT should produce sensible adjustments in both directions
    const auto chainParams = CreateChainParams(*m_node.args, ChainType::MAIN);
    const auto& consensus = chainParams->GetConsensus();

    arith_uint256 refTarget;
    refTarget.SetCompact(0x1c00ffff);

    const arith_uint256 powLimit = UintToArith256(consensus.powLimit);
    const int64_t nPowTargetSpacing = 600;
    const int64_t nHalfLife = 172800;

    // First: slow blocks (target increases/difficulty decreases)
    // 10 blocks in 12000 seconds (should take 6600 ideally for 11 intervals)
    arith_uint256 slowResult = CalculateASERT(refTarget, nPowTargetSpacing, 12000, 10, powLimit, nHalfLife);

    // Then: fast blocks (target decreases/difficulty increases)
    // 10 blocks in 3000 seconds (should take 6600 ideally)
    arith_uint256 fastResult = CalculateASERT(refTarget, nPowTargetSpacing, 3000, 10, powLimit, nHalfLife);

    // Verify direction of adjustments:
    // Slow blocks -> target increases (difficulty decreases)
    // Fast blocks -> target decreases (difficulty increases)
    BOOST_CHECK(slowResult > refTarget);
    BOOST_CHECK(fastResult < refTarget);

    // Verify adjustments are reasonable (within 2x for these moderate deviations)
    BOOST_CHECK(slowResult < refTarget * 2);
    BOOST_CHECK(fastResult > refTarget / 2);

    // Verify that adjustment magnitudes are non-trivial but not extreme
    // slowResult should be noticeably higher than refTarget
    BOOST_CHECK(slowResult > refTarget + refTarget / 100);  // At least 1% higher
    // fastResult should be noticeably lower than refTarget
    BOOST_CHECK(fastResult < refTarget - refTarget / 100);  // At least 1% lower
}

BOOST_AUTO_TEST_CASE(fivetrat_nested_proof_tiers)
{
    constexpr uint32_t blue_bits{0x1a00ccf5};
    arith_uint256 blue_target;
    blue_target.SetCompact(blue_bits);

    BOOST_CHECK(ClassifyProofTier(ArithToUint256(blue_target), blue_bits) == ProofTier::BLUE);
    BOOST_CHECK(ClassifyProofTier(ArithToUint256(blue_target / 4), blue_bits) == ProofTier::PINK);
    BOOST_CHECK(ClassifyProofTier(ArithToUint256(blue_target / 12), blue_bits) == ProofTier::GOLD);

    BOOST_CHECK_EQUAL(GetProofTierTarget(blue_bits, ProofTier::BLUE), ArithToUint256(blue_target));
    BOOST_CHECK_EQUAL(GetProofTierTarget(blue_bits, ProofTier::PINK), ArithToUint256(blue_target / 4));
    BOOST_CHECK_EQUAL(GetProofTierTarget(blue_bits, ProofTier::GOLD), ArithToUint256(blue_target / 12));
}

BOOST_AUTO_TEST_CASE(fivetrat_production_consensus_parameters)
{
    const auto params = CreateChainParams(ArgsManager{}, ChainType::MAIN);
    const auto& consensus = params->GetConsensus();

    BOOST_CHECK_EQUAL(params->GetDefaultPort(), 57555);
    BOOST_CHECK_EQUAL(consensus.nPowTargetSpacing, 15 * 60);
    BOOST_CHECK_EQUAL(consensus.nPowTargetUpgradeHeight, 80);
    BOOST_CHECK_EQUAL(consensus.GetPowTargetSpacing(79), 15 * 60);
    BOOST_CHECK_EQUAL(consensus.GetPowTargetSpacing(80), 5 * 60);
    BOOST_CHECK_EQUAL(consensus.nASERTHalfLife, Consensus::Params::ASERT_HALFLIFE_30_MINUTES);
    BOOST_CHECK_EQUAL(consensus.GetASERTHalfLife(79), 30 * 60);
    BOOST_CHECK_EQUAL(consensus.GetASERTHalfLife(80), 15 * 60);
    BOOST_CHECK_EQUAL(consensus.nASERTHalfLifeTransitionHeight, Consensus::NEVER_ACTIVE_HEIGHT);
    BOOST_CHECK_EQUAL(consensus.nASERTAnchorEpochLength, 144);
    BOOST_CHECK_EQUAL(consensus.nASERTStallResetSeconds, 6 * 60 * 60);
    BOOST_CHECK_EQUAL(consensus.nASERTMaxStallEasingHalflives, 2);
    BOOST_CHECK(consensus.fASERTAnchorAtFirstBlockTime);
    BOOST_CHECK_EQUAL(consensus.nSubsidyHalvingInterval, 420000);
    BOOST_CHECK_EQUAL(consensus.nInitialSubsidy, 5 * COIN);
    BOOST_CHECK_EQUAL(consensus.nJackpotActivationHeight, 280);
    BOOST_CHECK_EQUAL(GetBlockSubsidy(1, consensus), 5 * COIN);
    BOOST_CHECK_EQUAL(GetBlockSubsidy(420000, consensus), 250000000);
    BOOST_REQUIRE(consensus.asertAnchorParams.has_value());
    BOOST_CHECK_EQUAL(consensus.asertAnchorParams->nHeight, 1);
    BOOST_CHECK_EQUAL(consensus.asertAnchorParams->nBits, 0x1a00ccf5U);
    BOOST_CHECK_EQUAL(UintToArith256(consensus.GetPowLimit(79)).GetCompact(), 0x1a00ccf5U);
    BOOST_CHECK_EQUAL(UintToArith256(consensus.GetPowLimit(80)).GetCompact(), 0x1a0266dfU);
    BOOST_CHECK_EQUAL(params->GenesisBlock().GetHash().GetHex(), "af4973599946fbe8c350eae4ff51ba9fbe3fc00fa07e8413b869874ee1be8310");
    BOOST_CHECK_EQUAL(params->GenesisBlock().hashMerkleRoot.GetHex(), "f18430f89ae896d596d5dba54f5303ddff124532015bdb07150ba3f9f4763335");
    BOOST_CHECK(CheckProofOfWork(params->GenesisBlock().GetHash(), params->GenesisBlock().nBits, consensus));

    CBlockIndex genesis;
    genesis.nHeight = 0;
    genesis.nTime = params->GenesisBlock().nTime;
    genesis.nBits = params->GenesisBlock().nBits;

    CBlockIndex delayed_block_one;
    delayed_block_one.pprev = &genesis;
    delayed_block_one.nHeight = 1;
    delayed_block_one.nTime = genesis.nTime + 8 * 60 * 60;
    delayed_block_one.nBits = consensus.asertAnchorParams->nBits;

    CBlockHeader block_two;
    block_two.nTime = delayed_block_one.nTime + consensus.nPowTargetSpacing;
    BOOST_CHECK_EQUAL(GetNextWorkRequired(&delayed_block_one, &block_two, consensus), 0x1a00ccf5U);
}

BOOST_AUTO_TEST_CASE(fivetrat_five_minute_upgrade_boundary)
{
    const auto params = CreateChainParams(ArgsManager{}, ChainType::MAIN);
    const auto& consensus = params->GetConsensus();
    const int activation = consensus.nPowTargetUpgradeHeight;

    std::vector<CBlockIndex> chain(activation + 2);
    chain[0].nHeight = 0;
    chain[0].nTime = params->GenesisBlock().nTime;
    chain[0].nBits = params->GenesisBlock().nBits;

    for (int height = 1; height <= activation + 1; ++height) {
        CBlockHeader candidate;
        candidate.nTime = chain[height - 1].nTime +
            consensus.GetPowTargetSpacing(height);
        chain[height].pprev = &chain[height - 1];
        chain[height].nHeight = height;
        chain[height].nTime = candidate.nTime;
        chain[height].nBits =
            GetNextWorkRequired(&chain[height - 1], &candidate, consensus);
    }

    // No cliff: the activation block inherits the final legacy target.
    BOOST_CHECK_EQUAL(chain[activation].nBits, chain[activation - 1].nBits);
    // A block arriving on the new five-minute schedule keeps that target.
    BOOST_CHECK_EQUAL(chain[activation + 1].nBits, chain[activation].nBits);

    const uint32_t legacy_floor =
        UintToArith256(consensus.GetPowLimit(activation - 1)).GetCompact();
    const uint32_t upgraded_floor =
        UintToArith256(consensus.GetPowLimit(activation)).GetCompact();
    BOOST_CHECK(!PermittedDifficultyTransition(
        consensus, activation - 1, legacy_floor, upgraded_floor));
    BOOST_CHECK(PermittedDifficultyTransition(
        consensus, activation, legacy_floor, upgraded_floor));

    // The reward schedule is deliberately unchanged by the timing upgrade.
    BOOST_CHECK_EQUAL(GetBlockSubsidy(activation - 1, consensus), 5 * COIN);
    BOOST_CHECK_EQUAL(GetBlockSubsidy(activation, consensus), 5 * COIN);
    BOOST_CHECK_EQUAL(GetBlockSubsidy(420000, consensus), 250000000);

    // Even if the final legacy block is the first returning block after an
    // outage, the activation block is an exact target handoff. Bounded stall
    // recovery may begin with later blocks, never by creating an activation
    // cliff.
    chain[activation - 1].nTime += 7 * 60 * 60;
    CBlockHeader delayed_activation;
    delayed_activation.nTime = chain[activation - 1].nTime +
        consensus.GetPowTargetSpacing(activation);
    BOOST_CHECK_EQUAL(
        GetNextWorkRequired(
            &chain[activation - 1], &delayed_activation, consensus),
        chain[activation - 1].nBits);
}

BOOST_AUTO_TEST_CASE(fivetrat_stall_recovery_is_bounded)
{
    const auto params = CreateChainParams(ArgsManager{}, ChainType::MAIN);
    const auto& consensus = params->GetConsensus();

    std::vector<CBlockIndex> chain(12);
    chain[0].nHeight = 0;
    chain[0].nTime = params->GenesisBlock().nTime;
    chain[0].nBits = params->GenesisBlock().nBits;
    chain[1].pprev = &chain[0];
    chain[1].nHeight = 1;
    chain[1].nTime = chain[0].nTime + consensus.nPowTargetSpacing;
    chain[1].nBits = consensus.asertAnchorParams->nBits;

    // Establish a legitimately high target using a run of rapid blocks.
    for (int height = 2; height <= 9; ++height) {
        CBlockHeader candidate;
        candidate.nTime = chain[height - 1].nTime + 1;
        chain[height].pprev = &chain[height - 1];
        chain[height].nHeight = height;
        chain[height].nTime = candidate.nTime;
        chain[height].nBits = GetNextWorkRequired(&chain[height - 1], &candidate, consensus);
    }

    // The first block after an outage is still required at the difficulty
    // already in force; a candidate timestamp cannot buy an easier target.
    CBlockHeader returning_block;
    returning_block.nTime = chain[9].nTime + 7 * 60 * 60;
    CBlockHeader ordinary_candidate;
    ordinary_candidate.nTime = chain[9].nTime + consensus.nPowTargetSpacing;
    const uint32_t returning_bits = GetNextWorkRequired(&chain[9], &returning_block, consensus);
    BOOST_CHECK_EQUAL(
        returning_bits,
        GetNextWorkRequired(&chain[9], &ordinary_candidate, consensus));

    chain[10].pprev = &chain[9];
    chain[10].nHeight = 10;
    chain[10].nTime = returning_block.nTime;
    chain[10].nBits = returning_bits;

    CBlockHeader following_block;
    following_block.nTime = chain[10].nTime + consensus.nPowTargetSpacing;
    arith_uint256 recovered_target;
    recovered_target.SetCompact(
        GetNextWorkRequired(&chain[10], &following_block, consensus));
    arith_uint256 high_difficulty_target;
    high_difficulty_target.SetCompact(returning_bits);

    // Two configured half-lives permit approximately 4x easing, never an
    // unbounded catch-up target and never beyond the permanent launch floor.
    BOOST_CHECK(recovered_target >= high_difficulty_target * 3);
    BOOST_CHECK(recovered_target <= high_difficulty_target * 5);
    BOOST_CHECK(recovered_target <= UintToArith256(consensus.powLimit));
}

BOOST_AUTO_TEST_CASE(fivetrat_fast_blocks_raise_difficulty_quickly)
{
    const auto params = CreateChainParams(ArgsManager{}, ChainType::MAIN);
    const auto& consensus = params->GetConsensus();

    std::vector<CBlockIndex> chain(14);
    chain[0].nHeight = 0;
    chain[0].nTime = params->GenesisBlock().nTime;
    chain[0].nBits = params->GenesisBlock().nBits;
    chain[1].pprev = &chain[0];
    chain[1].nHeight = 1;
    chain[1].nTime = chain[0].nTime + consensus.nPowTargetSpacing;
    chain[1].nBits = consensus.asertAnchorParams->nBits;

    for (int height = 2; height < 14; ++height) {
        CBlockHeader candidate;
        candidate.nTime = chain[height - 1].nTime + 1;
        chain[height].pprev = &chain[height - 1];
        chain[height].nHeight = height;
        chain[height].nTime = candidate.nTime;
        chain[height].nBits = GetNextWorkRequired(&chain[height - 1], &candidate, consensus);
    }

    arith_uint256 launch_target;
    launch_target.SetCompact(consensus.asertAnchorParams->nBits);
    arith_uint256 final_target;
    final_target.SetCompact(chain.back().nBits);
    BOOST_CHECK(final_target < launch_target / 32);
}

BOOST_AUTO_TEST_SUITE_END()
