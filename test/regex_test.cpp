#include "re.h"
#include "gtest/gtest.h"

#define OK    0
#define NOK   -1

typedef struct {
    int res;
    std::string pattern;
    std::string text;
} regex_db;

regex_db db[] =
    {
            { OK,  "\\d",                       "5"                },
            { OK,  "\\w+",                      "hej"              },
            { OK,  "\\s",                       "\t \n"            },
            { NOK, "\\S",                       "\t \n"            },
            { OK,  "[\\s]",                     "\t \n"            },
            { NOK, "[\\S]",                     "\t \n"            },
            { NOK, "\\D",                       "5"                },
            { NOK, "\\W+",                      "hej"              },
            { OK,  "[0-9]+",                    "12345"            },
            { OK,  "\\D",                       "hej"              },
            { NOK, "\\d",                       "hej"              },
            { OK,  "[^\\w]",                    "\\"               },
            { OK,  "[\\W]",                     "\\"               },
            { NOK, "[\\w]",                     "\\"               },
            { OK,  "[^\\d]",                    "d"                },
            { NOK, "[\\d]",                     "d"                },
            { NOK, "[^\\D]",                    "d"                },
            { OK,  "[\\D]",                     "d"                },
            { OK,  "^.*\\\\.*$",                "c:\\Tools"        },
            { OK,  "^[\\+-]*[\\d]+$",           "+27"              },
            { OK,  "[abc]",                     "1c2"              },
            { NOK, "[abc]",                     "1C2"              },
            { OK,  "[1-5]+",                    "0123456789"       },
            { OK,  "[.2]",                      "1C2"              },
            { OK,  "a*$",                       "Xaa"              },
            { OK,  "a*$",                       "Xaa"              },
            { OK,  "[a-h]+",                    "abcdefghxxx"      },
            { NOK, "[a-h]+",                    "ABCDEFGH"         },
            { OK,  "[A-H]+",                    "ABCDEFGH"         },
            { NOK, "[A-H]+",                    "abcdefgh"         },
            { OK,  "[^\\s]+",                   "abc def"          },
            { OK,  "[^fc]+",                    "abc def"          },
            { OK,  "[^d\\sf]+",                 "abc def"          },
            { OK,  "\n",                        "abc\ndef"         },
            { OK,  "b.\\s*\n",                  "aa\r\nbb\r\ncc\r\n\r\n" },
            { OK,  ".*c",                       "abcabc"           },
            { OK,  ".+c",                       "abcabc"           },
            { OK,  "[b-z].*",                   "ab"               },
            { OK,  "b[k-z]*",                   "ab"               },
            { NOK, "[0-9]",                     "  - "             },
            { OK,  "[^0-9]",                    "  - "             },
            { OK,  "0|",                        "0|"               },
            { NOK, "\\d\\d:\\d\\d:\\d\\d",      "0s:00:00"         },
            { NOK, "\\d\\d:\\d\\d:\\d\\d",      "000:00"           },
            { NOK, "\\d\\d:\\d\\d:\\d\\d",      "00:0000"          },
            { NOK, "\\d\\d:\\d\\d:\\d\\d",      "100:0:00"         },
            { NOK, "\\d\\d:\\d\\d:\\d\\d",      "00:100:00"        },
            { NOK, "\\d\\d:\\d\\d:\\d\\d",      "0:00:100"         },
            { OK,  "\\d\\d?:\\d\\d?:\\d\\d?",   "0:0:0"            },
            { OK,  "\\d\\d?:\\d\\d?:\\d\\d?",   "0:00:0"           },
            { OK,  "\\d\\d?:\\d\\d?:\\d\\d?",   "0:0:00"           },
            { OK,  "\\d\\d?:\\d\\d?:\\d\\d?",   "00:0:0"           },
            { OK,  "\\d\\d?:\\d\\d?:\\d\\d?",   "00:00:0"          },
            { OK,  "\\d\\d?:\\d\\d?:\\d\\d?",   "00:0:00"          },
            { OK,  "\\d\\d?:\\d\\d?:\\d\\d?",   "0:00:00"          },
            { OK,  "\\d\\d?:\\d\\d?:\\d\\d?",   "00:00:00"         },
            { OK,  "[Hh]ello [Ww]orld\\s*[!]?", "Hello world !"    },
            { OK,  "[Hh]ello [Ww]orld\\s*[!]?", "hello world !"    },
            { OK,  "[Hh]ello [Ww]orld\\s*[!]?", "Hello World !"    },
            { OK,  "[Hh]ello [Ww]orld\\s*[!]?", "Hello world!   "  },
            { OK,  "[Hh]ello [Ww]orld\\s*[!]?", "Hello world  !"   },
            { OK,  "[Hh]ello [Ww]orld\\s*[!]?", "hello World    !" },
            { NOK, "\\d\\d?:\\d\\d?:\\d\\d?",   "a:0"              }, /* Failing test case reported in https://github.com/kokke/tiny-regex-c/issues/12 */
            { OK,  ".?bar",                      "real_bar"        },
            { NOK, ".?bar",                      "real_foo"        },
            { NOK, "X?Y",                        "Z"               },
    };
TEST(regex, match1)
{
    const char *pattern;
    const char *test;
    bool should_fail;
    bool isFail;
    size_t testNum = sizeof(db) / sizeof(*db);
    for (int i = 0; i < testNum; i++)
    {
        pattern = db[i].pattern.c_str();
        test = db[i].text.c_str();
        should_fail = db[i].res != 0;

        int m = reMatch(pattern, test);
        isFail = m == -1;

        assert(should_fail == isFail);
        ASSERT_EQ(should_fail == isFail, true);
        if (should_fail != m)
        {
            printf("%s match %s error\r\n", pattern, test);
        }
    }
}
