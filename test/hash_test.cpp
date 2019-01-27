//
// Created by savent on 19-1-25.
//

#include "gtest/gtest.h"
#include "hash.h"
#include <stdio.h>

namespace {
class testHash : public::testing::Test {
protected:
    Hash* hash;
protected:
    testHash() {
    }
    ~testHash() {
    }

    void SetUp() override {
        hash = new Hash(128, sizeof(int));
    }
    void TearDown() override {
        delete hash;
    }
};
}

TEST_F(testHash, checkBasic) {
    char name[] =  "Hel";
    int data = 10;
    int ans;

    for (int i = 0; i < 1000; i++)
    {
        data = i;
        name[0] = static_cast<char>(std::rand() % 127 + 1);
        ASSERT_EQ(hash->remove(name), true);
        ASSERT_EQ(hash->getData(name, &ans), false);
        ASSERT_EQ(hash->setData(name, &data), false);
        ASSERT_EQ(hash->isHas(name), false);

        ASSERT_EQ(hash->insert(name, &data), true);
        ASSERT_EQ(hash->insert(name, &data), false);
        ASSERT_EQ(hash->getData(name, &ans), true);
        ASSERT_EQ(ans, data);
        data++;
        ASSERT_EQ(hash->setData(name, &data), true);
        ASSERT_EQ(hash->getData(name, &ans), true);
        ASSERT_EQ(ans, data);
        ASSERT_EQ(hash->isHas(name), true);
        ASSERT_EQ(hash->remove(name), true);

        hash->clearDleted();
    }
}

TEST_F(testHash, checkMassInsert) {
    char name[2] = {1, 0 };
    for (int i = 0; i < 127; i++)
    {
        name[0] = static_cast<char>(i+1);
        ASSERT_EQ(true, hash->insert(name, &i));
    }
    for (int i = 0; i < 127; i++)
    {
        name[0] = static_cast<char>(i+1);
        int ans;
        ASSERT_EQ(true, hash->getData(name, &ans));
        ASSERT_EQ(ans, i);
    }
}

TEST_F(testHash, checkMassRemove) {
    char name[2] = {1, 0 };
    for (int cnt = 0; cnt < 1000; cnt++)
    {
        for (int i = 0; i < 127; i++)
        {
            name[0] = static_cast<char>(i+1);
            ASSERT_EQ(true, hash->insert(name, &i));
        }
        for (int i = 0; i < 127; i++)
        {
            name[0] = static_cast<char>(i+1);
            int ans;
            ASSERT_EQ(true, hash->getData(name, &ans));
            ASSERT_EQ(ans, i);
        }
        for (int i = 0; i < 127; i++)
        {
            name[0] = static_cast<char>(i+1);
            ASSERT_EQ(true, hash->remove(name));
        }
        // to clear deleted flag
        hash->clearDleted();
        for (int i = 0; i < 127; i++)
        {
            name[0] = static_cast<char>(i+1);
            ASSERT_EQ(true, hash->insert(name, &i));
        }
        for (int i = 0; i < 127; i++)
        {
            name[0] = static_cast<char>(i+1);
            int ans;
            ASSERT_EQ(true, hash->getData(name, &ans));
            ASSERT_EQ(ans, i);
        }
        for (int i = 0; i < 127; i++)
        {
            name[0] = static_cast<char>(i+1);
            ASSERT_EQ(true, hash->remove(name));
        }
        // to clear deleted flag
        hash->clearDleted();
    }

}
