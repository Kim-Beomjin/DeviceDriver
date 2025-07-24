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
	void setNormalReadCall(unsigned char expected) {
		EXPECT_CALL(mockHardware, read)
			.WillRepeatedly(Return(expected));
	}

	void setNormalReadCallWithTimes(unsigned char expected, int times) {
		EXPECT_CALL(mockHardware, read)
			.Times(times)
			.WillRepeatedly(Return(expected));
	}

	void setReadCalls(vector<unsigned char>& expected) {
		EXPECT_CALL(mockHardware, read)
			.WillOnce(Return(expected[0]))
			.WillOnce(Return(expected[1]))
			.WillOnce(Return(expected[2]))
			.WillOnce(Return(expected[3]))
			.WillOnce(Return(expected[4]))
			.WillRepeatedly(Return(expected[5]));
	}

	void setSuccessWriteCall(void) {
		EXPECT_CALL(mockHardware, write)
			.Times(1);
	}

	void setFailedWriteCall(void) {
		EXPECT_CALL(mockHardware, write)
			.Times(0);
	}

	const unsigned char CORRECT_READ_VALUE = (unsigned char)0;
	const unsigned char INCORRECT_READ_VALUE = (unsigned char)1;
	const unsigned char ERASED_READ_VALUE = (unsigned char)0xFF;

	NiceMock<MockFlashMemoryDevice> mockHardware;
	FlashMemoryDevice* hardware = &mockHardware;
	DeviceDriver driver{ hardware };
};

TEST_F(DeviceDriverFixture, ReadSuccess) {
	setNormalReadCall(CORRECT_READ_VALUE);

	int data = driver.read(0xFF);
	EXPECT_EQ(0, data);
}


TEST_F(DeviceDriverFixture, ReadFiveTimes) {
	setNormalReadCallWithTimes(CORRECT_READ_VALUE, 5);

	int data = driver.read(0xFF);
	EXPECT_EQ(0, data);
}

TEST_F(DeviceDriverFixture, ReadException) {
	vector<unsigned char> mockCalls = { CORRECT_READ_VALUE,
										CORRECT_READ_VALUE,
										CORRECT_READ_VALUE,
										CORRECT_READ_VALUE,
										INCORRECT_READ_VALUE,
										CORRECT_READ_VALUE };

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
	vector<unsigned char> mockCalls = { CORRECT_READ_VALUE,
										CORRECT_READ_VALUE,
										CORRECT_READ_VALUE,
										CORRECT_READ_VALUE,
										CORRECT_READ_VALUE,
										INCORRECT_READ_VALUE };
	setReadCalls(mockCalls);

	int data = driver.read(0xFF);
	EXPECT_EQ(0, data);
}

TEST_F(DeviceDriverFixture, ReadBeforeWrite) {
	setNormalReadCallWithTimes(ERASED_READ_VALUE, 1);

	driver.write(0xFF, 1);
}

TEST_F(DeviceDriverFixture, WriteSuccess) {
	setNormalReadCall(ERASED_READ_VALUE);
	setSuccessWriteCall();

	driver.write(0xFF, 1);
}

TEST_F(DeviceDriverFixture, WriteException) {
	setNormalReadCall(INCORRECT_READ_VALUE);
	setFailedWriteCall();

	try {
		driver.write(0xFF, 1);
		FAIL();
	}
	catch (runtime_error& e) {
		EXPECT_EQ(string{ e.what() },
			string{ "Write Fail" });
	}
}

TEST_F(DeviceDriverFixture, ReadAndPrint) {
	Application app{ &driver };

	setNormalReadCallWithTimes(CORRECT_READ_VALUE, 25);

	app.readAndPrint(0x00, 0x04);
}


int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}