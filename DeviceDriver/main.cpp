#include "gmock/gmock.h"
#include "device_driver.h"

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

int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}