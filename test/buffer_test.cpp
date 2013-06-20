#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "buffer.h"

#define BOOST_TEST_MODULE KafkaBuffer
#include <boost/test/unit_test.hpp>
#include "buffer.h"

BOOST_AUTO_TEST_SUITE (KafkaBuffer)

BOOST_AUTO_TEST_CASE (test_write)
{
	Kafka::Buffer::Pointer tmp = Kafka::Buffer::create_for_write();
	tmp->write(static_cast<int32_t>(10))
		->write(static_cast<uint32_t>(12))
		->write(static_cast<int16_t>(5))
		->write(static_cast<uint16_t>(3))
		->write(static_cast<int64_t>(100))
		->write(static_cast<uint64_t>(500))
		->write("Hello World");

	BOOST_REQUIRE(tmp->size() == 45);
}

BOOST_AUTO_TEST_CASE (test_read)
{
	Kafka::Buffer::Pointer tmp = Kafka::Buffer::create_for_write();
	tmp->write(static_cast<int32_t>(10))
		->write(static_cast<uint32_t>(12))
		->write(static_cast<int16_t>(5))
		->write(static_cast<uint16_t>(3))
		->write(static_cast<int64_t>(100))
		->write(static_cast<uint64_t>(500))
		->write("Hello World");

	Kafka::Buffer::ConstPointer tmp_read
                    = Kafka::Buffer::create_for_read(tmp->build());
    int32_t size = 0;
	int32_t intval = 0;
	uint32_t uintval = 0;
	int16_t shortval = 0;
	uint16_t ushortval = 0;
	int64_t longval = 0;
	uint64_t ulongval = 0;
	std::string val;
	tmp_read->read(size)
            ->read(intval)
			->read(uintval)
			->read(shortval)
			->read(ushortval)
			->read(longval)
			->read(ulongval)
			->read(val);
	BOOST_REQUIRE(intval == 10);
	BOOST_REQUIRE(uintval == 12);
	BOOST_REQUIRE(shortval == 5);
	BOOST_REQUIRE(ushortval == 3);
	BOOST_REQUIRE(longval == 100);
	BOOST_REQUIRE(ulongval == 500);
	BOOST_REQUIRE(ulongval == 500);
	BOOST_REQUIRE(val == "Hello World");
}

BOOST_AUTO_TEST_CASE (test_build)
{
	Kafka::Buffer::Pointer tmp = Kafka::Buffer::create_for_write();
	tmp->write(static_cast<int32_t>(10))
		->write(static_cast<uint32_t>(12))
		->write(static_cast<int16_t>(5))
		->write(static_cast<uint16_t>(3))
		->write(static_cast<int64_t>(100))
		->write(static_cast<uint64_t>(500))
		->write("Hello World")
        ->finalize_header(); //<--sums up remaining part to first int

	BOOST_REQUIRE(*(reinterpret_cast<int32_t *>(tmp->data())) == 41);
}

BOOST_AUTO_TEST_SUITE_END( )

