LINK.o = $(LINK.cc)
CXXFLAGS = -O2 -std=c++14 -Wall

# The one-definition rule applies differently to classes and functions
# https://stackoverflow.com/questions/14654377/why-is-there-no-multiple-definition-error-when-you-define-a-class-in-a-header-fi

functions = common.o murmurhash3.o utils.o

all: correctness persistence

test_filter: ${functions} test_filter.o

test_skiplist: ${functions} test_skiplist.o

test_memtable: ${functions} test_memtable.o

test_sstable: ${functions} test_sstable.o

correctness: ${functions} kvstore.o correctness.o

persistence: ${functions} kvstore.o persistence.o

clean:
	-rm -f test_filter test_skiplist test_memtable test_sstable
	-rm -f correctness persistence *.o
