#include <stdexcept>
#include <string>
#include <vector>
#include "gmock/gmock.h"
#include "device_driver.h"

using namespace std;
using namespace testing;


class MockFlashMemoryDevice : public FlashMemoryDevice {
public:
	MOCK_METHOD(unsigned char, read, (long), (override));
	MOCK_METHOD(void, write, (long, unsigned char), (override));
};

class DeviceDriverFixture : public Test {
public:
	void setNormalReadCall(int expected) {
		EXPECT_CALL(mockHardware, read)
			.WillRepeatedly(Return(expected));
	}

	void setNormalReadCallWithTimes(int expected, int times) {
		EXPECT_CALL(mockHardware, read)
			.Times(times)
			.WillRepeatedly(Return(expected));
	}

	void setReadCalls(vector<int>& expected) {
		EXPECT_CALL(mockHardware, read)
			.WillOnce(Return(expected[0]))
			.WillOnce(Return(expected[1]))
			.WillOnce(Return(expected[2]))
			.WillOnce(Return(expected[3]))
			.WillOnce(Return(expected[4]))
			.WillRepeatedly(Return(expected[5]));
	}

	NiceMock<MockFlashMemoryDevice> mockHardware;
	FlashMemoryDevice* hardware = &mockHardware;
	DeviceDriver driver{ hardware };
};

TEST_F(DeviceDriverFixture, ReadSuccess) {
	setNormalReadCall(0);

	int data = driver.read(0xFF);
	EXPECT_EQ(0, data);
}


TEST_F(DeviceDriverFixture, ReadFiveTimes) {
	setNormalReadCallWithTimes(0, 5);

	int data = driver.read(0xFF);
	EXPECT_EQ(0, data);
}

TEST_F(DeviceDriverFixture, ReadException) {
	vector<int> mockCalls = { 0, 0, 0, 0, 1, 0 };
	setReadCalls(mockCalls);

	try {
		int data = driver.read(0xFF);
		FAIL();
	}
	catch (runtime_error& e) {
		EXPECT_EQ(string{ e.what() },
			string{ "Read Fail" });
	}
}

TEST_F(DeviceDriverFixture, ReadErrorAfterFive) {
	vector<int> mockCalls = { 0, 0, 0, 0, 0, 1 };
	setReadCalls(mockCalls);

	int data = driver.read(0xFF);
	EXPECT_EQ(0, data);

}

int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}