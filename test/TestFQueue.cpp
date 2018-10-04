#include <FQueue.h>
#include <unity.h>

unsigned long buf[32];

void setUp(void)
{
    // prefill buffer with dummy value
    for (int i=0; i<32; ++i){
	buf[i] = 0xaa;
    }
}

void tearDown(void)
{
    
}

void testEmptyAfterCreation(void)
{
    FQueue q(32, buf);
    TEST_ASSERT_TRUE(q.empty());    
}

void testCapacity(void)
{
    FQueue q(32, buf);
    TEST_ASSERT_EQUAL(32, q.capacity());
}

void testNotFullAfterCreation(void)
{
    FQueue q(32, buf);
    TEST_ASSERT_FALSE(q.full());    
}

void testZeroElementsAfterCreation(void)
{
    FQueue q(32, buf);
    TEST_ASSERT_EQUAL(0, q.size());
}

void testPushAndPeekOneElement(void)
{
    FQueue q(32, buf);
    q.push(42);
    TEST_ASSERT_EQUAL(42, q.front());
}

void testPushingAddsOneElement(void)
{
    FQueue q(32, buf);
    q.push(42);
    TEST_ASSERT_EQUAL(1, q.size());
    TEST_ASSERT_FALSE(q.empty());    
}
  
void testPushAndPopEmptiesQueue(void)
{
    FQueue q(32, buf);
    q.push(42);
    q.pop();
    TEST_ASSERT_EQUAL(0, q.size());    
}

void testPoppingInOrder(void)
{
    FQueue q(32, buf);
    for (int i=0; i<10; ++i){
	q.push(i);
    }
    for (int i=0; i<10; ++i){
	TEST_ASSERT_EQUAL(i, q.pop());
    }
}

void testPopEmptyIsZero(void)
{
    FQueue q(32, buf);
    TEST_ASSERT_EQUAL(0, q.pop());
}

void testFrontEmptyIsZero(void)
{
    FQueue q(32, buf);
    TEST_ASSERT_EQUAL(0, q.front());
}

void testBackEmptyIsZero(void)
{
    FQueue q(32, buf);
    TEST_ASSERT_EQUAL(0, q.back());
}

void testPoppingInOrderFrontIsOldest(void)
{
    FQueue q(32, buf);
    for (int i=0; i<10; ++i){
	q.push(i);
    }
    for (int i=0; i<10; ++i){
	TEST_ASSERT_EQUAL(i, q.front());
        q.pop();
    }
}

void testFrontIsIndexZero(void)
{
    FQueue q(32, buf);
    for (int i=0; i<10; ++i){
	q.push(i);
        TEST_ASSERT_EQUAL(q.at(0), q.front());
    }
    for (int i=0; i<10; ++i){
	TEST_ASSERT_EQUAL(q.at(0), q.front());
        q.pop();
    }
}

void testPoppingInOrderBackIsNewest(void)
{
    FQueue q(32, buf);
    for (int i=0; i<10; ++i){
	q.push(i);
    }
    for (int i=0; i<10; ++i){
	TEST_ASSERT_EQUAL(9, q.back());
        q.pop();
    }
}

void testBackIsHighestIndex(void)
{
    FQueue q(32, buf);
    for (int i=0; i<10; ++i){
	q.push(i);
        TEST_ASSERT_EQUAL(q.at(q.size()-1), q.back());
    }
    for (int i=0; i<10; ++i){
	TEST_ASSERT_EQUAL(q.at(q.size()-1), q.back());
        q.pop();
    }
}

void testFillQueue(void)
{
    FQueue q(32, buf);
    for (int i=0; i<32; ++i){
	TEST_ASSERT_FALSE(q.full());
	q.push(i);
    }
    TEST_ASSERT_TRUE(q.full());
}

void testDroppingAtBack(void)
{
    FQueue q(32, buf);
    for (int i=0; i<33; ++i){
	q.push(i);
    }
    TEST_ASSERT_EQUAL(1, q.front());    
}

void testLookupElementsByIndex(void)
{
    FQueue q(32, buf);
    for (int i=0; i<10; ++i){
	q.push(42);
	q.pop();
    }    
    for (int i=0; i<10; ++i){
	q.push(i);
    }
    for (int i=0; i<10; ++i){
	TEST_ASSERT_EQUAL(i, q.at(i));
    }
}

void testLookupOutOfRangeIsZero(void)
{
    FQueue q(32, buf);
    for (int i=-5; i<42; ++i){
	TEST_ASSERT_EQUAL(0, q.at(i));
    }    
}
   
