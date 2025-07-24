#include <stdexcept>
#include <string>
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
	NiceMock<MockFlashMemoryDevice> mockHardware;
	FlashMemoryDevice* hardware = &mockHardware;
	DeviceDriver driver{ hardware };
};

TEST_F(DeviceDriverFixture, ReadSuccess) {
	EXPECT_CALL(mockHardware, read)
		.WillRepeatedly(Return(0));

	int data = driver.read(0xFF);
	EXPECT_EQ(0, data);
}

TEST_F(DeviceDriverFixture, ReadFiveTimes) {
	EXPECT_CALL(mockHardware, read)
		.Times(5)
		.WillRepeatedly(Return(0));

	int data = driver.read(0xFF);
	EXPECT_EQ(0, data);
}

TEST_F(DeviceDriverFixture, ReadException) {
	EXPECT_CALL(mockHardware, read)
		.WillOnce(Return(0))
		.WillOnce(Return(0))
		.WillOnce(Return(0))
		.WillOnce(Return(0))
		.WillOnce(Return(1))
		.WillRepeatedly(Return(0));

	try {
		int data = driver.read(0xFF);
		FAIL();
	}
	catch (runtime_error& e) {
		EXPECT_EQ(string{ e.what() },
			string{ "Read Fail" });
	}
}

int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}