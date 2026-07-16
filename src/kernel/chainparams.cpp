// Copyright (c) 2009-2025 Satoshi Nakamoto
// Copyright (c) 2009-2024 The Bitcoin Core developers
// Copyright (c) 2024-2025 The BitcoinII developers
// Copyright (c) 2025 The Bitcoin Cash II developers
// Forked from Bitcoin Core version 0.27.0
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "15 July 2026 - 5tratum Coin private genesis: proof before promotion";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

static CBlock Create5tratGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "16 July 2026 - 5tratum Coin 15-minute relaunch: no catch-up issuance";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 420000;
        consensus.nInitialSubsidy = 5 * COIN;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;

        // CSV (CheckSequenceVerify)
        consensus.CSVHeight = 1;

        // Segwit softfork (disabled after BCH2 fork via IsSegwitActive())
        consensus.SegwitHeight = Consensus::NEVER_ACTIVE_HEIGHT;

        // =========================================================================
        // 5TRAT is a fresh chain. BCH transaction rules are active for every
        // spendable block after genesis.
        // =========================================================================
        consensus.BCH2ForkHeight = 0;

        // All BCH upgrades activate immediately after fork
        // (BCH2 starts as fully-upgraded BCH)
        consensus.uahfHeight = 0;
        consensus.daaHeight = 0;
        consensus.magneticAnomalyHeight = 0;
        consensus.gravitonHeight = 0;
        consensus.phononHeight = 0;
        consensus.axionHeight = 0;
        consensus.upgrade8Height = 0;
        consensus.upgrade9Height = 0;
        consensus.upgrade10Height = 0;
        consensus.upgrade11Height = 0;

        // Production prototype anchor: accepted Blue proofs average 15 minutes
        // at 100 TH/s. Pink and Gold are nested proof-quality achievements and
        // never alter subsidy or chainwork.
        consensus.asertAnchorParams = Consensus::ASERTAnchor{
            1,
            0x1a00ccf5,
            1784228400,
        };
        consensus.fASERTAnchorAtFirstBlockTime = true;

        // A small SHA-256 network must react much faster than BCH. Keep the
        // response fixed at 30 minutes; do not slow to a two-day half-life.
        consensus.nASERTHalfLife = Consensus::Params::ASERT_HALFLIFE_30_MINUTES;
        consensus.nASERTHalfLifeTransitionHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.nASERTAnchorEpochLength = 144;
        consensus.nASERTStallResetSeconds = 6 * 60 * 60;
        consensus.nASERTMaxStallEasingHalflives = 2;

        // Default block size (32MB for BCH)
        consensus.nDefaultConsensusBlockSize = 32000000;

        // BCHN-style automatic finalization (rolling checkpoints)
        // Blocks deeper than this from the tip are finalized and cannot be reorged
        consensus.maxReorgDepth = 100;
        // =========================================================================

        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing

        // Taproot - DISABLED (requires SegWit which BCH2 doesn't support)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0;

        // Disable TESTDUMMY deployment
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;

        consensus.MinBIP9WarningHeight = 0;

        // Mining/difficulty rules
        // The permanent floor is the 100 TH/s launch target. Idle wall-clock
        // time can never make the chain easier than this or create catch-up
        // issuance after an outage.
        consensus.powLimit = uint256S("00000000000000ccf50000000000000000000000000000000000000000000000");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 15 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x94;
        pchMessageStart[1] = 0x40;
        pchMessageStart[2] = 0x0d;
        pchMessageStart[3] = 0xb0;
        nDefaultPort = 57555;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        // The compiled bootstrap header declares the same target as block one,
        // so difficulty telemetry is meaningful from the first node start. Its
        // exact hash is admitted by the genesis-only exception in pow.cpp.
        genesis = Create5tratGenesisBlock(1784228400, 0, 0x1a00ccf5, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xaf4973599946fbe8c350eae4ff51ba9fbe3fc00fa07e8413b869874ee1be8310"));
        assert(genesis.hashMerkleRoot == uint256S("0xf18430f89ae896d596d5dba54f5303ddff124532015bdb07150ba3f9f4763335"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,125);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,80);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,181);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x54, 0x52};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x54, 0x50};

        bech32_hrp = "5trat";

        vFixedSeeds.clear();

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {{{0, consensus.hashGenesisBlock}}};

        m_assumeutxo_data = {
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{0, 0, 0};
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 420000;
        consensus.nInitialSubsidy = 5 * COIN;
        consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256S("0x00000000dd30457c001f4095d208cc1296b0eed002427aa599874af7a432b105"), SCRIPT_VERIFY_NONE);
        // BIP heights - active from early blocks for fresh BCH2 testnet
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.MinBIP9WarningHeight = 0;

        // =========================================================================
        // BCH2 Fork Parameters - Active from genesis for fresh testnet
        // =========================================================================
        consensus.BCH2ForkHeight = 0;
        consensus.uahfHeight = 0;
        consensus.daaHeight = 0;
        consensus.magneticAnomalyHeight = 0;
        consensus.gravitonHeight = 0;
        consensus.phononHeight = 0;
        consensus.axionHeight = 0;
        consensus.upgrade8Height = 0;
        consensus.upgrade9Height = 0;
        consensus.upgrade10Height = 0;
        consensus.upgrade11Height = 0;

        // Easy private test anchor at block one.
        consensus.asertAnchorParams = Consensus::ASERTAnchor{
            1,
            0x207fffff,
            1784116801,
        };

        // Default block size (32MB - BCH2 standard)
        consensus.nDefaultConsensusBlockSize = 32000000;

        // Testnet mirrors the production 30-minute response.
        consensus.nASERTHalfLife = Consensus::Params::ASERT_HALFLIFE_30_MINUTES;
        consensus.nASERTHalfLifeTransitionHeight = Consensus::NEVER_ACTIVE_HEIGHT;

        // BCHN-style automatic finalization (rolling checkpoints)
        consensus.maxReorgDepth = 100;
        // =========================================================================

        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;

        // Taproot - DISABLED (requires SegWit which BCH2 doesn't support)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0;

        consensus.nMinimumChainWork = uint256S("0x00");
        consensus.defaultAssumeValid = uint256S("0x00");

        // BCH2 testnet-specific magic (avoids collision with Bitcoin Core testnet3)
        pchMessageStart[0] = 0x6d;
        pchMessageStart[1] = 0xd5;
        pchMessageStart[2] = 0x5f;
        pchMessageStart[3] = 0x1b;
        nDefaultPort = 57565;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 42;
        m_assumed_chain_state_size = 3;

        genesis = CreateGenesisBlock(1784116801, 2, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x4b7fd4a73464c8d4a7348242b13ca3cea6597fb307b3b9b60418d4c4dee8408e"));
        assert(genesis.hashMerkleRoot == uint256S("0xe8c58936a0ad31eaea782ef0401f185bf466d71c0384c5b2ccfd0bd5a17e3377"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        // BCH2 DISABLED: vSeeds.emplace_back("testnet-seed.bitcoin.jonasschnelli.ch.");
        // BCH2 DISABLED: vSeeds.emplace_back("seed.tbtc.petertodd.net.");
        // BCH2 DISABLED: vSeeds.emplace_back("seed.testnet.bitcoin.sprovoost.nl.");
        // BCH2 DISABLED: vSeeds.emplace_back("testnet-seed.bluematt.me.");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "t5trat";

        vFixedSeeds.clear();

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        // BCH2 testnet: fresh chain from genesis, no pre-fork checkpoints
        checkpointData = {{{0, consensus.hashGenesisBlock}}};

        // BCH2 testnet: no assumeutxo data yet (fresh chain)
        m_assumeutxo_data = {};

        chainTxData = ChainTxData{
            // BCH2 testnet: no historical data yet
            .nTime    = 0,
            .nTxCount = 0,
            .dTxRate  = 0,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vSeeds.clear();

        if (!options.challenge) {
            bin = ParseHex("512103ad5e0edad18cb1f0fc0d28a3d4f1f3e445640337489abb10404f2d1e086be430210359ef5021964fe22d6f8e05b2463c9540ce96883fe3b278760f048f5189f2e6c452ae");
            // BCH2 DISABLED: vSeeds.emplace_back("seed.signet.bitcoin.sprovoost.nl.");

            // Hardcoded nodes can be removed once there are more DNS seeds
            // BCH2 DISABLED: vSeeds.emplace_back("178.128.221.177");
            // BCH2 DISABLED: vSeeds.emplace_back("v7ajjeirttkbnt32wpy3c6w3emwnfr3fkla7hpxcfokr3ysd3kqtzmqd.onion:38333");

            consensus.nMinimumChainWork = uint256S("0x00");
            consensus.defaultAssumeValid = uint256S("0x00");
            m_assumed_blockchain_size = 1;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                // Data from RPC: getchaintxstats 4096 0000000870f15246ba23c16e370a7ffb1fc8a3dcf8cb4492882ed4b0e3d4cd26
                .nTime    = 1706331472,
                .nTxCount = 2425380,
                .dTxRate  = 0.008277759863833788,
            };
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogPrintf("Signet with challenge %s\n", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;

        // =========================================================================
        // BCH2 Fork Parameters - Disabled on signet
        // =========================================================================
        consensus.BCH2ForkHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.uahfHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.daaHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.magneticAnomalyHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.gravitonHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.phononHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.axionHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.upgrade8Height = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.upgrade9Height = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.upgrade10Height = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.upgrade11Height = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.asertAnchorParams = Consensus::ASERTAnchor{};
        consensus.nDefaultConsensusBlockSize = 1000000;

        // ASERT half-life: 1 hour for signet (no transition, BCH2 fork disabled)
        consensus.nASERTHalfLife = Consensus::Params::ASERT_HALFLIFE_30_MINUTES;
        // =========================================================================

        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;

        // Taproot - DISABLED (requires SegWit which BCH2 doesn't support)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0;

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 57566;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1784116802, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x04ff25c33725671302ab7e6f0e43d9d633406f0790e243a618cd7afd297a10b5"));
        assert(genesis.hashMerkleRoot == uint256S("0xe8c58936a0ad31eaea782ef0401f185bf466d71c0384c5b2ccfd0bd5a17e3377"));

        vFixedSeeds.clear();

        m_assumeutxo_data = {
            {
                .height = 160'000,
                .hash_serialized = AssumeutxoHash{uint256S("0xfe0a44309b74d6b5883d246cb419c6221bcccf0b308c9b59b7d70783dbdf928a")},
                .nChainTx = 2289496,
                .blockhash = uint256S("0x0000003ca3c99aff040f2563c2ad8f8ec88bd0fd6b8f0895cfaf1ef90353a62c")
            }
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "s5trat";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP34Height = 1; // Always active unless overridden
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1;  // Always active unless overridden
        consensus.BIP66Height = 1;  // Always active unless overridden
        consensus.CSVHeight = 1;    // Always active unless overridden
        consensus.SegwitHeight = Consensus::NEVER_ACTIVE_HEIGHT;
        consensus.MinBIP9WarningHeight = 0;

        // =========================================================================
        // 5TRAT rules are active from the first block after genesis.
        // =========================================================================
        consensus.BCH2ForkHeight = 0;
        consensus.uahfHeight = 0;
        consensus.daaHeight = 0;
        consensus.magneticAnomalyHeight = 0;
        consensus.gravitonHeight = 0;
        consensus.phononHeight = 0;
        consensus.axionHeight = 0;
        consensus.upgrade8Height = 0;
        consensus.upgrade9Height = 0;
        consensus.upgrade10Height = 0;
        consensus.upgrade11Height = 0;

        // ASERT anchor for regtest - anchor at first post-fork block (201)
        // Same pattern as mainnet: fork at 200, anchor at 201
        // Anchor parent time = 0 means use block 200's timestamp dynamically
        consensus.asertAnchorParams = Consensus::ASERTAnchor{
            1,
            0x207fffff,   // regtest minimum difficulty
            0,            // 0 = dynamically use parent block's timestamp
        };

        // Default block size (32MB for BCH mode)
        consensus.nDefaultConsensusBlockSize = 32000000;

        // ASERT half-life: Starts at 1 hour, transitions to 2 days at block 432
        // Low transition height for regtest allows testing the transition
        consensus.nASERTHalfLife = Consensus::Params::ASERT_HALFLIFE_1_HOUR;
        consensus.nASERTHalfLifeTransitionHeight = 432;  // Low height for testing

        // BCHN-style automatic finalization (rolling checkpoints)
        // High value for regtest to allow deep reorg testing while still
        // exercising the finalization code path. Mainnet/testnet use 10.
        consensus.maxReorgDepth = 10000;
        // =========================================================================

        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0;

        // Taproot - DISABLED (requires SegWit which BCH2 doesn't support)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0;

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0x71;
        pchMessageStart[1] = 0xbd;
        pchMessageStart[2] = 0xaf;
        pchMessageStart[3] = 0xa5;
        nDefaultPort = 57575;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        // nonce=0 works with this easy difficulty
        genesis = CreateGenesisBlock(1784116803, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x65d191cc20c81b1d8a633e2a2f35d244d95f743aee4cbb0d5714191e2f62f13f"));
        assert(genesis.hashMerkleRoot == uint256S("0xe8c58936a0ad31eaea782ef0401f185bf466d71c0384c5b2ccfd0bd5a17e3377"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        // BCH2 DISABLED: vSeeds.emplace_back("dummySeed.invalid.");

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, consensus.hashGenesisBlock},
            }
        };

        m_assumeutxo_data = {};

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "r5trat";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}
