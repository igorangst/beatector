#include <BeatPredictor.h>
#include <unity.h>

unsigned long  buf[32];
FQueue        *beatQ;
BeatPredictor *predictor;

void setUp(void)
{
    beatQ = new FQueue(32, buf);
    predictor = new BeatPredictor(beatQ);
}

void tearDown(void)
{
    delete predictor;
    delete beatQ;
}

void testCreatePredictorZeroPeriod(void)
{
    TEST_ASSERT_EQUAL(predictor->period(), 0);
}

void testEmptyQueueNoPrediction(void)
{
    TEST_ASSERT_FALSE(predictor->predict());
}

void testConstantDistanceEvents(void)
{
    for (int i=0; i<4; ++i){
	beatQ->push(i * 500);
    }
    TEST_ASSERT_TRUE(predictor->predict());
    TEST_ASSERT_EQUAL(500, predictor->period());
}
