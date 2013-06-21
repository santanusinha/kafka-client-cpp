#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "buffer.h"

#define BOOST_TEST_MODULE KafkaBuffer
#include <boost/test/unit_test.hpp>
#include "buffer.h"
#include "serializable.h"
#include "deserializable.h"

namespace {

struct Point: public Kafka::Serializable, public Kafka::Deserializable {
    int32_t m_x;
    int32_t m_y;

    Point()
        :m_x(),
        m_y() {
    }

    Point(int32_t x, int32_t y)
        :m_x(x),
        m_y(y) {
    }

    bool
    operator =(const Point &rhs) {
        return m_x == rhs.m_x && m_y == rhs.m_y;
    }

    void
    save(const Kafka::BufferPtr &buffer) const {
        buffer->write(m_x)->write(m_y);
    }

    void
    read(const Kafka::ConstBufferPtr &buffer) {
        buffer->read(m_x)->read(m_y);
    }
};

}

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

BOOST_AUTO_TEST_CASE( test_object_ser )
{
    Point testpoint(21,72);
    Kafka::BufferPtr writebuf = Kafka::Buffer::create_for_write();
    Kafka::ConstBufferPtr readbuf = Kafka::Buffer::create_for_read(writebuf
                                                            ->write(testpoint)
                                                            ->finalize_header()
                                                            ->build());
    int32_t objsize = 0;
    Point readpoint;
    readbuf->read(objsize)->read(readpoint);
    BOOST_REQUIRE(objsize == 8);
    BOOST_REQUIRE(readpoint.m_x == 21 && readpoint.m_y == 72);
}

BOOST_AUTO_TEST_CASE( test_object_vector_ser )
{
    std::vector<Point> putpoints;
    putpoints.push_back(Point(21,72));
    putpoints.push_back(Point(23,79));
    putpoints.push_back(Point(27,89));
    Kafka::BufferPtr writebuf = Kafka::Buffer::create_for_write();

    std::vector<Point> readpoints;
    Kafka::ConstBufferPtr readbuf = Kafka::Buffer::create_for_read(writebuf
                                                            ->write(putpoints)
                                                            ->finalize_header()
                                                            ->build());
    int32_t objsize = 0;
    Point readpoint;
    readbuf->read(objsize)->read(readpoints);
    BOOST_REQUIRE(objsize == 28);
    BOOST_REQUIRE(readpoints[0].m_x == putpoints[0].m_x && readpoints[0].m_y == readpoints[0].m_y);
    BOOST_REQUIRE(readpoints[1].m_x == putpoints[1].m_x && readpoints[1].m_y == readpoints[1].m_y);
    BOOST_REQUIRE(readpoints[2].m_x == putpoints[2].m_x && readpoints[2].m_y == readpoints[2].m_y);
}

BOOST_AUTO_TEST_SUITE_END( )

