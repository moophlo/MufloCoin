#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-MFC");
    BOOST_CHECK(GetBoolArg("-MFC"));
    BOOST_CHECK(GetBoolArg("-MFC", false));
    BOOST_CHECK(GetBoolArg("-MFC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-MFCo"));
    BOOST_CHECK(!GetBoolArg("-MFCo", false));
    BOOST_CHECK(GetBoolArg("-MFCo", true));

    ResetArgs("-MFC=0");
    BOOST_CHECK(!GetBoolArg("-MFC"));
    BOOST_CHECK(!GetBoolArg("-MFC", false));
    BOOST_CHECK(!GetBoolArg("-MFC", true));

    ResetArgs("-MFC=1");
    BOOST_CHECK(GetBoolArg("-MFC"));
    BOOST_CHECK(GetBoolArg("-MFC", false));
    BOOST_CHECK(GetBoolArg("-MFC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noMFC");
    BOOST_CHECK(!GetBoolArg("-MFC"));
    BOOST_CHECK(!GetBoolArg("-MFC", false));
    BOOST_CHECK(!GetBoolArg("-MFC", true));

    ResetArgs("-noMFC=1");
    BOOST_CHECK(!GetBoolArg("-MFC"));
    BOOST_CHECK(!GetBoolArg("-MFC", false));
    BOOST_CHECK(!GetBoolArg("-MFC", true));

    ResetArgs("-MFC -noMFC");  // -MFC should win
    BOOST_CHECK(GetBoolArg("-MFC"));
    BOOST_CHECK(GetBoolArg("-MFC", false));
    BOOST_CHECK(GetBoolArg("-MFC", true));

    ResetArgs("-MFC=1 -noMFC=1");  // -MFC should win
    BOOST_CHECK(GetBoolArg("-MFC"));
    BOOST_CHECK(GetBoolArg("-MFC", false));
    BOOST_CHECK(GetBoolArg("-MFC", true));

    ResetArgs("-MFC=0 -noMFC=0");  // -MFC should win
    BOOST_CHECK(!GetBoolArg("-MFC"));
    BOOST_CHECK(!GetBoolArg("-MFC", false));
    BOOST_CHECK(!GetBoolArg("-MFC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--MFC=1");
    BOOST_CHECK(GetBoolArg("-MFC"));
    BOOST_CHECK(GetBoolArg("-MFC", false));
    BOOST_CHECK(GetBoolArg("-MFC", true));

    ResetArgs("--noMFC=1");
    BOOST_CHECK(!GetBoolArg("-MFC"));
    BOOST_CHECK(!GetBoolArg("-MFC", false));
    BOOST_CHECK(!GetBoolArg("-MFC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-MFC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MFC", "eleven"), "eleven");

    ResetArgs("-MFC -bar");
    BOOST_CHECK_EQUAL(GetArg("-MFC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MFC", "eleven"), "");

    ResetArgs("-MFC=");
    BOOST_CHECK_EQUAL(GetArg("-MFC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MFC", "eleven"), "");

    ResetArgs("-MFC=11");
    BOOST_CHECK_EQUAL(GetArg("-MFC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-MFC", "eleven"), "11");

    ResetArgs("-MFC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-MFC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-MFC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-MFC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-MFC", 0), 0);

    ResetArgs("-MFC -bar");
    BOOST_CHECK_EQUAL(GetArg("-MFC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-MFC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-MFC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-MFC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-MFC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--MFC");
    BOOST_CHECK_EQUAL(GetBoolArg("-MFC"), true);

    ResetArgs("--MFC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-MFC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noMFC");
    BOOST_CHECK(!GetBoolArg("-MFC"));
    BOOST_CHECK(!GetBoolArg("-MFC", true));
    BOOST_CHECK(!GetBoolArg("-MFC", false));

    ResetArgs("-noMFC=1");
    BOOST_CHECK(!GetBoolArg("-MFC"));
    BOOST_CHECK(!GetBoolArg("-MFC", true));
    BOOST_CHECK(!GetBoolArg("-MFC", false));

    ResetArgs("-noMFC=0");
    BOOST_CHECK(GetBoolArg("-MFC"));
    BOOST_CHECK(GetBoolArg("-MFC", true));
    BOOST_CHECK(GetBoolArg("-MFC", false));

    ResetArgs("-MFC --noMFC");
    BOOST_CHECK(GetBoolArg("-MFC"));

    ResetArgs("-noMFC -MFC"); // MFC always wins:
    BOOST_CHECK(GetBoolArg("-MFC"));
}

BOOST_AUTO_TEST_SUITE_END()
